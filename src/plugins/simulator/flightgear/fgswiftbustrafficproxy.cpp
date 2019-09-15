/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "fgswiftbustrafficproxy.h"
#include <QLatin1String>
#include <QDBusConnection>

#define FGSWIFTBUS_SERVICENAME "org.swift-project.fgswiftbus"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace Flightgear
    {
        CFGSwiftBusTrafficProxy::CFGSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(FGSWIFTBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (!dummy)
            {
                bool s;
                s = connection.connect(QString(), "/fgswiftbus/traffic", "org.swift_project.fgswiftbus.traffic",
                                       "simFrame", this, SIGNAL(simFrame()));
                Q_ASSERT(s);

                s = connection.connect(QString(), "/fgswiftbus/traffic", "org.swift_project.fgswiftbus.traffic",
                                       "remoteAircraftAdded", this, SIGNAL(remoteAircraftAdded(QString)));
                Q_ASSERT(s);

                s = connection.connect(QString(), "/fgswiftbus/traffic", "org.swift_project.fgswiftbus.traffic",
                                       "remoteAircraftAddingFailed", this, SIGNAL(remoteAircraftAddingFailed(QString)));
                Q_ASSERT(s);
            }
        }

        MultiplayerAcquireInfo CFGSwiftBusTrafficProxy::acquireMultiplayerPlanes()
        {
            QDBusPendingReply<bool, QString> reply =  m_dbusInterface->asyncCall(QLatin1String("acquireMultiplayerPlanes"));
            reply.waitForFinished();
            if (reply.isError())
            {
                BlackMisc::CLogMessage(this).debug(u"CFGSwiftBusTrafficProxy::acquireMultiplayerPlanes returned: %1") << reply.error().message();
            }
            MultiplayerAcquireInfo info;
            info.hasAcquired = reply.argumentAt<0>();
            info.owner = reply.argumentAt<1>();
            return info;
        }

        bool CFGSwiftBusTrafficProxy::initialize()
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("initialize"));
        }

        void CFGSwiftBusTrafficProxy::cleanup()
        {
            m_dbusInterface->callDBus(QLatin1String("cleanup"));
        }

        void CFGSwiftBusTrafficProxy::addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlane"), callsign, modelName, aircraftIcao, airlineIcao, livery);
        }

        void CFGSwiftBusTrafficProxy::removePlane(const QString &callsign)
        {
            m_dbusInterface->callDBus(QLatin1String("removePlane"), callsign);
        }

        void CFGSwiftBusTrafficProxy::removeAllPlanes()
        {
            m_dbusInterface->callDBus(QLatin1String("removeAllPlanes"));
        }

        void CFGSwiftBusTrafficProxy::setPlanesPositions(const PlanesPositions &planesPositions)
        {
            m_dbusInterface->callDBus(QLatin1String("setPlanesPositions"),
                                      planesPositions.callsigns, planesPositions.latitudesDeg, planesPositions.longitudesDeg,
                                      planesPositions.altitudesFt, planesPositions.pitchesDeg, planesPositions.rollsDeg,
                                      planesPositions.headingsDeg, planesPositions.onGrounds);
        }

        void CFGSwiftBusTrafficProxy::getRemoteAircraftData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter) const
        {
            std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
            {
                QDBusPendingReply<QStringList, QList<double>, QList<double>, QList<double>, QList<double>> reply = *watcher;
                if (!reply.isError())
                {
                    const QStringList   callsigns       = reply.argumentAt<0>();
                    const QList<double> latitudesDeg    = reply.argumentAt<1>();
                    const QList<double> longitudesDeg   = reply.argumentAt<2>();
                    const QList<double> elevationsM     = reply.argumentAt<3>();
                    const QList<double> verticalOffsets = reply.argumentAt<4>();
                    setter(callsigns,   latitudesDeg, longitudesDeg, elevationsM, verticalOffsets);
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getRemoteAircraftData"), callback, callsigns);
        }
    } // ns
} // ns
