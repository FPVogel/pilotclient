/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_VATSIMBOOKINGREADER_H
#define BLACKCORE_VATSIMBOOKINGREADER_H

//! \file

#include "blackmisc/threadedreader.h"
#include "blackmisc/avatcstationlist.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    /*!
     * Read bookings from VATSIM
     */
    class CVatsimBookingReader : public QObject, public BlackMisc::CThreadedReader<void>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimBookingReader(const QString &url, QObject *parent = nullptr);

        //! Read / re-read bookings
        void read();

    private slots:
        //! Bookings have been read
        void ps_loadFinished(QNetworkReply *nwReply);

    private:
        QString m_serviceUrl; /*!< URL of the service */
        QNetworkAccessManager *m_networkManager;

        //! Parse received bookings
        //! \threadsafe
        void parseBookings(QNetworkReply *nwReply);

    signals:
        //! Bookings have been read and converted to BlackMisc::Aviation::CAtcStationList
        void dataRead(const BlackMisc::Aviation::CAtcStationList &bookedStations);
    };
}
#endif // guard
