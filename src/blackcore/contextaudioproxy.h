/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTVOICE_PROXY_H
#define BLACKCORE_CONTEXTVOICE_PROXY_H

#include <QObject>
#include <QString>

#include "blackcore/blackcoreexport.h"
#include "blackcore/contextaudio.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/userlist.h"

class QDBusConnection;

namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Audio { class CAudioDeviceInfo; }
    namespace Aviation { class CCallsign; }
}

namespace BlackCore
{
    class CCoreFacade;

    //! \brief Audio context proxy
    //! \ingroup dbus
    class BLACKCORE_EXPORT CContextAudioProxy : public IContextAudio
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
        friend class IContextAudio;

    public:

        //! Destructor
        virtual ~CContextAudioProxy() {}

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        //! Relay connection signals to local signals
        //! No idea why this has to be wired and is not done automatically
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        //! Contructor
        CContextAudioProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextAudio(mode, runtime), m_dBusInterface(nullptr) {}

        //! DBus version constructor
        CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

    public slots:
        //! \copydoc IContextAudio::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override;

        //! \copydoc IContextAudio::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override;

        //! \copydoc IContextAudio::getVoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue, bool withAudioStatus) const override;

        //! \copydoc IContextAudio::setComVoiceRooms()
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms) override;

        //! \copydoc IContextAudio::setOwnCallsignForRooms
        virtual void setOwnCallsignForRooms(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IContextAudio::getRoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignSet getRoomCallsigns(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const override;

        //! \copydoc IContextAudio::getRoomUsers()
        virtual BlackMisc::Network::CUserList getRoomUsers(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const override;

        //! \copydoc IContextAudio::leaveAllVoiceRooms
        virtual void leaveAllVoiceRooms() override;

        //! \copydoc IContextAudio::getAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const override;

        //! \copydoc IContextAudio::getCurrentAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const override;

        //! \copydoc IContextAudio::setCurrentAudioDevice()
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice) override;

        //!\copydoc IContextAudio::setVoiceOutputVolume
        virtual void setVoiceOutputVolume(int volume) override;

        //! \copydoc IContextAudio::getVoiceOutputVolume
        virtual int getVoiceOutputVolume() const override;

        //! \copydoc IContextAudio::setMute
        virtual void setMute(bool muted) override;

        //! \copydoc IContextAudio::isMuted()
        virtual bool isMuted() const override;

        //! \copydoc IContextAudio::playSelcalTone
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override;

        //! \copydoc IContextAudio::playNotification
        virtual void playNotification(BlackMisc::Audio::CNotificationSounds::Notification notification, bool considerSettings) const override;

        //! \copydoc IContextAudio::enableAudioLoopback()
        virtual void enableAudioLoopback(bool enable = true) override;

        //! \copydoc IContextAudio::isAudioLoopbackEnabled()
        virtual bool isAudioLoopbackEnabled() const override;

        //! \copydoc IContextOwnAircraft::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

    };
}

#endif // guard
