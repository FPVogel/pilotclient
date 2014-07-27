/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIMDATAFILEREADER_H
#define BLACKCORE_VATSIMDATAFILEREADER_H

#include "blackmisc/threadedreader.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/nwserverlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/avcallsignlist.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    /*!
     * Read bookings from VATSIM
     */
    class CVatsimDataFileReader : public QObject, public BlackMisc::CThreadedReader<void>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimDataFileReader(const QStringList &urls, QObject *parent = nullptr);

        //! Read / re-read bookings
        void read();

        //! Get aircrafts
        //! \threadsafe
        BlackMisc::Aviation::CAircraftList getAircrafts() const;

        //! Get aircrafts
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStations() const;

        //! Get all voice servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVoiceServers() const;

        //! Users for callsign(s)
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns);

        //! User for callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Controllers for callsigns
        //! \threadsafe
        BlackMisc::Network::CUserList getControllersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns);

        //! Controllers for callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getControllersForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Users for callsigns
        //! \threadsafe
        BlackMisc::Network::CUserList getPilotsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns);

        //! Users for callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getPilotsForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! ICAO info for callsign
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcao getIcaoInfo(const  BlackMisc::Aviation::CCallsign &callsign);

        //! Update aircraft with VATSIM aircraft data from data file
        //! \threadsafe
        void updateWithVatsimDataFileData(BlackMisc::Aviation::CAircraft &aircraftToBeUdpated) const;

    private slots:
        //! Data have been read
        void ps_loadFinished(QNetworkReply *nwReply);

    private:
        QStringList m_serviceUrls; /*!< URL of the service */
        int m_currentUrlIndex;
        QNetworkAccessManager *m_networkManager;
        BlackMisc::Network::CServerList m_voiceServers;
        BlackMisc::Aviation::CAtcStationList m_atcStations;
        BlackMisc::Aviation::CAircraftList m_aircrafts;
        static const QMap<QString, QString> clientPartsToMap(const QString &currentLine, const QStringList &clientSectionAttributes);

        //! Section in file
        enum Section
        {
            SectionNone,
            SectionVoiceServer,
            SectionClients,
            SectionGeneral
        };

        //! Parse the VATSIM data file in backgroun
        void parseVatsimFileInBackground(QNetworkReply *nwReply);

    signals:
        //! Data have been read
        void dataRead();

    };
}

#endif // guard
