/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/weather/presentweather.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Weather
    {

        void CPresentWeather::registerMetadata()
        {
            CValueObject<CPresentWeather>::registerMetadata();
            qRegisterMetaType<Intensity>();
            qRegisterMetaType<Descriptor>();
        }

        CPresentWeather::CPresentWeather(Intensity intensity, Descriptor descriptor, int weatherPhenomena) :
            m_intensity(intensity), m_descriptor(descriptor), m_weatherPhenomena(weatherPhenomena)
        { }

        CVariant CPresentWeather::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIntensity:
                return CVariant::fromValue(m_intensity);
            case IndexDescriptor:
                return CVariant::fromValue(m_descriptor);
            case IndexWeatherPhenomena:
                return CVariant::fromValue(m_weatherPhenomena);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CPresentWeather::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CPresentWeather>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIntensity:
                setIntensity(variant.value<Intensity>());
                break;
            case IndexDescriptor:
                setDescriptor(variant.value<Descriptor>());
                break;
            case IndexWeatherPhenomena:
                setWeatherPhenomena(variant.toInt());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CPresentWeather::convertToQString(bool /** i18n **/) const
        {
            static const QHash<Intensity, QString> intensityHash =
            {
                { Moderate, "moderate" },
                { Light, "light" },
                { Heavy, "heavy" },
                { InVincinity, "in vincinity" },
            };

            static const QHash<Descriptor, QString> descriptorHash =
            {
                { None, "" },
                { Shallow, "shallow" },
                { Patches, "patches" },
                { Partial, "partial" },
                { Drifting, "drifting" },
                { Blowing, "blowing" },
                { Showers, "showers" },
                { Thunderstorm, "thunderstorm" },
                { Freezing, "freezing" },
            };

            static const QHash<WeatherPhenomenon, QString> weatherPhenomenaHash =
            {
                { Drizzle, "drizzle" },
                { Rain, "rain" },
                { Snow, "snow" },
                { SnowGrains, "snow srains" },
                { IceCrystals, "ice crystals" },
                { IcePellets, "ice pellets" },
                { Hail, "hail" },
                { SnowPellets, "snow pellets" },
                { Unknown, "unknown" },
                { Mist, "mist" },
                { Fog, "fog" },
                { Smoke, "smoke" },
                { VolcanicAsh, "volcanic ash" },
                { Dust, "dust" },
                { Sand, "sand" },
                { Haze, "haze" },
                { DustSandWhirls, "dustSand whirls" },
                { Squalls, "squalls" },
                { TornadoOrWaterspout, "tornado or waterspout" },
                { FunnelCloud, "funnel cloud" },
                { Sandstorm, "sandstorm" },
                { Duststorm, "duststorm" }
            };

            QString weatherPhenomenaAsString;
            for (const auto &wp : weatherPhenomenaHash.keys())
            {
                if (m_weatherPhenomena & wp)
                {
                    if (!weatherPhenomenaAsString.isEmpty()) weatherPhenomenaAsString += " and ";
                    weatherPhenomenaAsString += weatherPhenomenaHash.value(wp);
                }
            }

            QString str;
            str += intensityHash.value(m_intensity);
            str += " ";
            str += descriptorHash.value(m_descriptor);
            str += " ";
            str += weatherPhenomenaAsString;
            return str;
        }

    } // namespace
} // namespace
