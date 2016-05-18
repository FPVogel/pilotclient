/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEATHERMANAGER_H
#define BLACKCORE_WEATHERMANAGER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/pluginmanagerweatherdata.h"
#include "blackmisc/slot.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/weather/weathergridprovider.h"

#include <QObject>
#include <QVector>

namespace BlackCore
{
    class IWeatherData;

    /*!
     * CWeatherManager
     */
    class BLACKCORE_EXPORT CWeatherManager :
            public QObject,
            public BlackMisc::Weather::IWeatherGridProvider
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Weather::IWeatherGridProvider)

    public:
        //! Default constructor
        CWeatherManager(QObject *parent = nullptr);

        //! Override weather to clear
        void setWeatherToClear(bool value);

        //! Is weather overwritten to clear?
        bool isWeatherClear() const { return m_isWeatherClear; }

        //! Request weather by grid
        virtual void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid,
                                        const BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> &callback) override;

    private:
        struct WeatherRequest
        {
            BlackMisc::Weather::CWeatherGrid weatherGrid;
            BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> callback;
        };

        bool loadWeatherDataPlugins();

        void fetchNextWeatherData();
        void handleNextRequest();

        CPluginManagerWeatherData m_pluginManagerWeatherData { this };
        QVector<IWeatherData *> m_weatherDataPlugins;
        QVector<WeatherRequest> m_pendingRequests;
        BlackMisc::Weather::CWeatherGrid m_weatherGrid;
        bool m_isWeatherClear = false;
    };

} // ns

#endif // guard
