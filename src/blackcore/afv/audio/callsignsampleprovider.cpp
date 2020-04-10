/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "callsignsampleprovider.h"
#include "callsigndelaycache.h"

#include "blackcore/afv/audio/receiversampleprovider.h"
#include "blacksound/sampleprovider/samples.h"
#include "blacksound/audioutilities.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

#include <QtMath>
#include <QDebug>

using namespace BlackMisc;
using namespace BlackSound::SampleProvider;
using namespace BlackConfig;

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            CallsignSampleProvider::CallsignSampleProvider(const QAudioFormat &audioFormat, const CReceiverSampleProvider *receiver, QObject *parent) :
                ISampleProvider(parent),
                m_audioFormat(audioFormat),
                m_receiver(receiver),
                m_decoder(audioFormat.sampleRate(), 1)
            {
                Q_ASSERT(audioFormat.channelCount() == 1);

                this->setObjectName("CallsignSampleProvider");
                m_mixer = new CMixingSampleProvider(this);
                m_crackleSoundProvider = new CResourceSoundSampleProvider(Samples::instance().crackle(), m_mixer);
                m_crackleSoundProvider->setLooping(true);
                m_crackleSoundProvider->setGain(0.0);
                m_whiteNoise = new CResourceSoundSampleProvider(Samples::instance().whiteNoise(), m_mixer);
                m_whiteNoise->setLooping(true);
                m_whiteNoise->setGain(0.0);
                m_hfWhiteNoise = new CResourceSoundSampleProvider(Samples::instance().hfWhiteNoise(), m_mixer);
                m_hfWhiteNoise->setLooping(true);
                m_hfWhiteNoise->setGain(0.0);
                m_acBusNoise = new CSawToothGenerator(400, m_mixer);
                m_audioInput = new CBufferedWaveProvider(audioFormat, m_mixer);

                // Create the compressor
                m_simpleCompressorEffect = new CSimpleCompressorEffect(m_audioInput, m_mixer);
                m_simpleCompressorEffect->setMakeUpGain(-5.5);

                // Create the voice EQ
                m_voiceEq = new CEqualizerSampleProvider(m_simpleCompressorEffect, EqualizerPresets::VHFEmulation, m_mixer);

                m_mixer->addMixerInput(m_whiteNoise);
                m_mixer->addMixerInput(m_acBusNoise);
                m_mixer->addMixerInput(m_hfWhiteNoise);
                m_mixer->addMixerInput(m_voiceEq);

                m_timer = new QTimer(this);
                m_timer->setObjectName(this->objectName() +  "m_timer");

                m_timer->setInterval(100);
                connect(m_timer, &QTimer::timeout, this, &CallsignSampleProvider::timerElapsed);
            }

            int CallsignSampleProvider::readSamples(QVector<float> &samples, qint64 count)
            {
                int noOfSamples = m_mixer->readSamples(samples, count);

                if (m_inUse && m_lastPacketLatch && m_audioInput->getBufferedBytes() == 0)
                {
                    idle();
                    m_lastPacketLatch = false;
                }

                if (m_inUse && !m_underflow && m_audioInput->getBufferedBytes() == 0)
                {
                    if (verbose()) { CLogMessage(this).debug(u"[%1] [Delay++]") << m_callsign; }
                    CallsignDelayCache::instance().underflow(m_callsign);
                    m_underflow = true;
                }

                return noOfSamples;
            }

            void CallsignSampleProvider::timerElapsed()
            {
                if (m_inUse && m_audioInput->getBufferedBytes() == 0 && m_lastSamplesAddedUtc.msecsTo(QDateTime::currentDateTimeUtc()) > m_idleTimeoutMs)
                {
                    idle();
                }
            }

            void CallsignSampleProvider::active(const QString &callsign, const QString &aircraftType)
            {
                m_callsign = callsign;
                CallsignDelayCache::instance().initialise(callsign);
                m_type = aircraftType;
                m_decoder.resetState();
                m_inUse = true;
                setEffects();
                m_underflow = false;

                int delayMs = CallsignDelayCache::instance().get(callsign);
                if (verbose()) { CLogMessage(this).debug(u"[%1] [Delay %2ms]") << m_callsign << delayMs; }
                if (delayMs > 0)
                {
                    int phaseDelayLength = (m_audioFormat.sampleRate() / 1000) * delayMs;
                    QVector<float> phaseDelay(phaseDelayLength * 2, 0);
                    m_audioInput->addSamples(phaseDelay);
                }
            }

            void CallsignSampleProvider::activeSilent(const QString &callsign, const QString &aircraftType)
            {
                m_callsign = callsign;
                CallsignDelayCache::instance().initialise(callsign);
                m_type = aircraftType;
                m_decoder.resetState();
                m_inUse = true;
                setEffects(true);
                m_underflow = true;
            }

            void CallsignSampleProvider::clear()
            {
                idle();
                m_audioInput->clearBuffer();
            }

            void CallsignSampleProvider::addOpusSamples(const IAudioDto &audioDto, float distanceRatio)
            {
                m_distanceRatio = distanceRatio;
                setEffects();

                QVector<qint16> audio = decodeOpus(audioDto.audio);
                m_audioInput->addSamples(BlackSound::convertFromShortToFloat(audio));
                m_lastPacketLatch = audioDto.lastPacket;
                if (audioDto.lastPacket && !m_underflow) { CallsignDelayCache::instance().success(m_callsign); }
                m_lastSamplesAddedUtc = QDateTime::currentDateTimeUtc();
                if (!m_timer->isActive()) { m_timer->start(); }
            }

            void CallsignSampleProvider::addSilentSamples(const IAudioDto &audioDto)
            {
                // Disable all audio effects
                setEffects(true);

                // TODO audioInput->addSamples(decoderByteBuffer, 0, frameCount * 2);
                m_lastPacketLatch = audioDto.lastPacket;

                m_lastSamplesAddedUtc = QDateTime::currentDateTimeUtc();
                if (!m_timer->isActive()) { m_timer->start(); }
            }

            void CallsignSampleProvider::idle()
            {
                m_timer->stop();
                m_inUse = false;
                setEffects();
                m_callsign.clear();
                m_type.clear();
            }

            QVector<qint16> CallsignSampleProvider::decodeOpus(const QByteArray &opusData)
            {
                int decodedLength = 0;
                QVector<qint16> decoded = m_decoder.decode(opusData, opusData.size(), &decodedLength);
                return decoded;
            }

            void CallsignSampleProvider::setEffects(bool noEffects)
            {
                if (noEffects || m_bypassEffects || !m_inUse)
                {
                    m_crackleSoundProvider->setGain(0.0);
                    m_whiteNoise->setGain(0.0);
                    m_hfWhiteNoise->setGain(0.0);
                    m_acBusNoise->setGain(0.0);
                    m_simpleCompressorEffect->setEnabled(false);
                    m_voiceEq->setBypassEffects(true);
                }
                else
                {
                    if (m_receiver->getFrequencyHz() < 30000000)
                    {
                        /**
                        double crackleFactor = (((qExp(m_distanceRatio) * qPow(m_distanceRatio, -4.0)) / 350.0) - 0.00776652);
                        if (crackleFactor < 0.0f)  { crackleFactor = 0.00f; }
                        if (crackleFactor > 0.20f) { crackleFactor = 0.20f; }
                        **/

                        m_hfWhiteNoise->setGain(m_hfWhiteNoiseGainMin);
                        m_acBusNoise->setGain(m_acBusGainMin + 0.001f);
                        m_simpleCompressorEffect->setEnabled(true);
                        m_voiceEq->setBypassEffects(false);
                        m_voiceEq->setOutputGain(0.38);
                        m_whiteNoise->setGain(0.0);
                    }
                    else
                    {
                        double crackleFactor = (((qExp(m_distanceRatio) * qPow(m_distanceRatio, -4.0)) / 350.0) - 0.00776652);

                        if (crackleFactor < 0.0)  { crackleFactor = 0.0;  }
                        if (crackleFactor > 0.20) { crackleFactor = 0.20; }

                        m_crackleSoundProvider->setGain(crackleFactor * 2);
                        m_whiteNoise->setGain(m_whiteNoiseGainMin);
                        m_acBusNoise->setGain(m_acBusGainMin);
                        m_simpleCompressorEffect->setEnabled(true);
                        m_voiceEq->setBypassEffects(false);
                        m_voiceEq->setOutputGain(1.0 - crackleFactor * 3.7);
                    }
                }
            }

            void CallsignSampleProvider::setBypassEffects(bool bypassEffects)
            {
                m_bypassEffects = bypassEffects;
                setEffects();
            }

        } // ns
    } // ns
} // ns
