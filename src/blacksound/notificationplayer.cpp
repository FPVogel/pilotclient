/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "notificationplayer.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/fileutils.h"
#include "blacksound/sampleprovider/samples.h"
#include <QTimer>
#include <QSound>

using namespace BlackMisc;
using namespace BlackMisc::Audio;

namespace BlackSound
{
    CNotificationPlayer::CNotificationPlayer(QObject *parent) :
        QObject(parent)
    {
        // lazy init at play
    }

    void CNotificationPlayer::play(CNotificationSounds::NotificationFlag notification, int volume)
    {
        QSoundEffect *effect = m_effects.value(notification, nullptr);
        if (effect)
        {
            if (!m_playingEffect && effect->isLoaded() && !effect->isPlaying())
            {
                const qreal v = volume / 100.0f;
                m_playingEffect = effect;
                effect->setVolume(v); // 0..1
                effect->play();

                /** for testing
                QString fn = effect->source().toLocalFile();
                QSound s(fn);
                s.play();
                **/

                // used for too long or hanging sounds
                QPointer<CNotificationPlayer> myself(this);
                QTimer::singleShot(2000, effect, [ = ]
                {
                    if (!myself || !m_playingEffect) { return; }
                    m_playingEffect->stop();
                });
            }
        }
    }

    void CNotificationPlayer::updateDirectory(const QString &directory)
    {
        if (directory == m_directory && !m_effects.isEmpty()) { return; }
        m_directory = directory;

        const QStringList types = QSoundEffect::supportedMimeTypes();
        CLogMessage(this).info(u"Notification mime types: %1") << types.join(", ");

        this->updateEffect(CNotificationSounds::NotificationError, directory, "error.wav");
        this->updateEffect(CNotificationSounds::NotificationLogin, directory, "login.wav");
        this->updateEffect(CNotificationSounds::NotificationLogoff, directory, "logoff.wav");
        this->updateEffect(CNotificationSounds::NotificationTextMessagePrivate, directory, "privatemessage.wav");
        this->updateEffect(CNotificationSounds::NotificationTextMessageSupervisor, directory, "supervisormessage.wav");
        this->updateEffect(CNotificationSounds::NotificationTextCallsignMentioned, directory, "callsignmentioned.wav");
        this->updateEffect(CNotificationSounds::NotificationNoAudioTransmission, directory, "noaudiotransmission.wav");
        this->updateEffect(CNotificationSounds::PTTBlocked, directory, "pttblocked.wav");
        this->updateEffect(CNotificationSounds::PTTClickKeyDown, directory, SampleProvider::Samples::fnClick());
        this->updateEffect(CNotificationSounds::PTTClickKeyUp,   directory, "pttclick.wav");
    }

    void CNotificationPlayer::onPlayingChanged()
    {
        if (!m_playingEffect) { return; }
        if (m_playingEffect->isPlaying()) { return; }
        m_playingEffect.clear();
    }

    void CNotificationPlayer::updateEffect(CNotificationSounds::NotificationFlag f, const QString &directory, const QString &name)
    {
        QSoundEffect *e = nullptr;
        if (m_effects.contains(f)) { e = m_effects[f]; }
        if (e) { e->deleteLater(); }

        // file if existing
        const QUrl url = CFileUtils::soundFileQUrlOrDefault(directory, name);
        if (url.isEmpty() || !url.isLocalFile())
        {
            // remove notification as not existing
            m_effects.remove(f);
            return;
        }

        // new effect
        // QString fn = url.toLocalFile();
        QSoundEffect *effect = new QSoundEffect(this);
        effect->setSource(url);
        effect->setLoopCount(1);
        effect->setMuted(false);
        m_effects[f] = effect;
        connect(effect, &QSoundEffect::playingChanged, this, &CNotificationPlayer::onPlayingChanged, Qt::QueuedConnection);
    }
} // ns
