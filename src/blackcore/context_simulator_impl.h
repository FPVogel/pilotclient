/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXTSIMULATOR_IMPL_H

#include "blackcore/context_simulator.h"
#include "blackcore/context_network_interface.h"
#include "blackcore/simulator.h"

#include <QTimer>

namespace BlackCore
{
    /*!
     * \brief Network simulator concrete implementation
     */
    class CContextSimulator : public IContextSimulator
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)

    public:
        //! \brief Constructor
        CContextSimulator(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CContextSimulator();

        /*!
         * \brief Register myself in DBus
         * \param server
         */
        void registerWithDBus(CDBusServer *server)
        {
            Q_ASSERT(server);
            server->addObject(CContextSimulator::ObjectPath(), this);
        }

        /*!
         * \brief Runtime
         * \return
         */
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        /*!
         * \brief Runtime
         * \return
         */
        CCoreRuntime *getRuntime()
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        /*!
         * \brief Using local objects?
         * \return
         */
        virtual bool usingLocalObjects() const override { return true; }

        virtual BlackMisc::Aviation::CAircraft ownAircraft() const override;

    public slots:

        virtual bool isConnected() const override;

    private slots:
        virtual void updateOwnAircraft();

        /*!
         * \brief Set new connection status
         * \param value
         */
        void setConnectionStatus(bool value);

    private:
        BlackMisc::Aviation::CAircraft m_ownAircraft;
        BlackCore::ISimulator *m_simulator;

        QTimer *m_updateTimer;
        BlackCore::IContextNetwork *m_contextNetwork;
    };

} // namespace BlackCore

#endif // BLACKCORE_CONTEXTSIMULATOR_IMPL_H
