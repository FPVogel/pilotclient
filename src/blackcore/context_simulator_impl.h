/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXTSIMULATOR_IMPL_H

#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackcore/simulator.h"

#include "blacksim/simulatorinfo.h"
#include "blacksim/simulatorinfolist.h"

#include <QTimer>
#include <QDir>
#include <QtConcurrent/QtConcurrent>

namespace BlackCore
{
    /*!
     * Network simulator concrete implementation
     */
    class CContextSimulator : public IContextSimulator
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
        friend class CRuntime;
        friend class IContextSimulator;

    public:
        //! Destructor
        virtual ~CContextSimulator();

    public slots:

        //! \copydoc IContextSimulator::getSimulatorPluginList()
        virtual BlackSim::CSimulatorInfoList getAvailableSimulatorPlugins() const override;

        //! \copydoc IContextSimulator::isConnected()
        virtual bool isConnected() const override;

        //! \copydoc IContextSimulator::canConnect
        virtual bool canConnect() override;

        //! \copydoc IContextSimulator::connectTo
        virtual bool connectTo() override;

        //! \copydoc IContextSimulator::asyncConnectTo
        virtual void asyncConnectTo() override;

        //! \copydoc IContextSimulator::disconnectFrom
        virtual bool disconnectFrom() override;

        //! \copydoc IContextSimulator::getSimulatorInfo()
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

        //! \copydoc IContextSimulator::getAircraftModel()
        virtual BlackMisc::Network::CAircraftModel getOwnAircraftModel() const override;

        //! \copydoc IContextSimulator::getAirportsInRange()
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

        //! \copydoc IContextSimulator::isTimeSynchronized
        virtual bool isTimeSynchronized() const override;

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

        //! \copydoc IContextSimulator::loadSimulatorPlugin()
        virtual bool loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo) override;

        //! \copydoc IContextSimulator::loadSimulatorPluginFromSettings()
        virtual bool loadSimulatorPluginFromSettings();

        //! \copydoc IContextSimulator::unloadSimulatorPlugin()
        virtual void unloadSimulatorPlugin() override;

        //! \copydoc IContextSimulator::settingsChanged
        virtual void settingsChanged(uint type) override;

        //! \copydoc IContextSimulator::isSimulatorPaused
        virtual bool isSimulatorPaused() const override;

    protected:
        //! \brief Constructor
        CContextSimulator(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextSimulator *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(CContextSimulator::ObjectPath(), this);
            return this;
        }

    private slots:
        //! Update own aircraft, because simulator has changed something
        void ps_updateOwnAircraft();

        //! \copydoc ISimulator::addAircraftSituation
        void ps_addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation);

        //! Update cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
        //! \remarks set by runtime, only to be used locally (not via DBus)
        void ps_updateCockpitFromContext(const BlackMisc::Aviation::CAircraft &ownAircraft, const QString &originator);

        //! Set new connection status
        void ps_setConnectionStatus(ISimulator::Status status);

        //! Status message received
        void ps_statusMessageReceived(const BlackMisc::CStatusMessage &statusMessage);

        //! Status messages received
        void ps_statusMessagesReceived(const BlackMisc::CStatusMessageList &statusMessages);

        //! Text message received
        void ps_textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

    private:
        static const auto c_logContext = CRuntime::LogForSimulator;

        //! \brief find and catalog all simulator plugins
        void findSimulatorPlugins();

        BlackMisc::Network::CAircraftModel m_aircraftModel;
        BlackMisc::Aviation::CAircraft m_ownAircraft;
        BlackCore::ISimulator *m_simulator;

        QTimer *m_updateTimer;
        QDir m_pluginsDir;
        QSet<ISimulatorFactory *> m_simulatorFactories;
        QFuture<bool> m_canConnectResult;
    };

} // namespace BlackCore

#endif // guard
