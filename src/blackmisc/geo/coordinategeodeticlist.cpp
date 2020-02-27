/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "coordinategeodeticlist.h"
#include "math/mathutils.h"

#include <QJsonValue>
#include <QString>
#include <QtGlobal>

using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Geo
    {
        CCoordinateGeodeticList::CCoordinateGeodeticList()
        { }

        CCoordinateGeodeticList::CCoordinateGeodeticList(const CSequence<CCoordinateGeodetic> &other) :
            CSequence<CCoordinateGeodetic>(other)
        { }

        CElevationPlane CCoordinateGeodeticList::averageGeodeticHeight(const CCoordinateGeodetic &reference, const CLength &range, const CLength &maxDeviation, int minValues) const
        {
            if (this->size() < minValues) { return CElevationPlane::null(); } // no change to succeed

            QList<double> valuesInFt;
            int count = 0;
            for (const CCoordinateGeodetic &coordinate : *this)
            {
                if (!coordinate.hasMSLGeodeticHeight()) { continue; }
                if (!coordinate.isWithinRange(reference, range)) { continue; }
                const double elvFt = coordinate.geodeticHeight().value(CLengthUnit::ft());
                valuesInFt.push_back(elvFt);
                count++;
                if (count > 5 && valuesInFt.size() > 3 * minValues) { break; }
            }

            if (count < minValues) { return CElevationPlane::null(); }

            const double MaxDevFt = maxDeviation.value(CLengthUnit::ft());
            const QPair<double, double> elvStdDevMean = CMathUtils::standardDeviationAndMean(valuesInFt);
            if (elvStdDevMean.first > MaxDevFt) { return CElevationPlane::null(); }
            return CElevationPlane(reference, elvStdDevMean.second, CElevationPlane::singlePointRadius());
        }

    } // namespace
} // namespace
