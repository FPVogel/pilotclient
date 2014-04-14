/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sampleschangeobject.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/avheading.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/aviotransponder.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/indexvariantmap.h"
#include "blackmisc/predicates.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QDebug>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesChangeObject::samples()
    {
        // ATC station
        QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        QDateTime dtUntil = dtFrom.addSecs(60 * 60.0); // 1 hour
        QDateTime dtFrom2 = dtUntil;
        QDateTime dtUntil2 = dtUntil.addSecs(60 * 60.0);
        CCoordinateGeodetic geoPos =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(1487, CLengthUnit::ft()));
        CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                             CFrequency(118.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
        CAtcStation station2(station1);
        CAtcStation station3(CCallsign("eddm_app"), CUser("654321", "Jen Doe"),
                             CFrequency(120.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(100, CLengthUnit::km()), false, dtFrom2, dtUntil2);

        // ATC List
        CAtcStationList atcList;
        atcList.push_back(station1);
        atcList.push_back(station2);
        atcList.push_back(station3);
        atcList.push_back(station1);
        atcList.push_back(station2);
        atcList.push_back(station3);
        qDebug() << "-- before update";
        qDebug() << atcList.toQString();

        // put Jane in the tower
        CIndexVariantMap newController;
        newController.addValue(CAtcStation::IndexController, QVariant::fromValue(CUser("112233", "Jane Doe")));
        atcList.applyIf(
            BlackMisc::Predicates::MemberEqual<CAtcStation>(&CAtcStation::getCallsign, CCallsign("eddm_twr")),
            newController);
        qDebug() << "-- after update via predicates";
        qDebug() << atcList.toQString();

        // now Jane's time is over
        CIndexVariantMap anotherController;
        anotherController.addValue(CAtcStation::IndexController, QVariant::fromValue(CUser("445566", "Fuzzy")));
        atcList.applyIf(newController, anotherController);

        qDebug() << "-- after update via value map";
        qDebug() << atcList.toQString();

        qDebug() << "-----------------------------------------------";
        return 0;
    }

} // namespace
