/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENT_MODELMATCHERCOMPONENT_H
#define BLACKGUI_COMPONENT_MODELMATCHERCOMPONENT_H

#include "blackcore/aircraftmatcher.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QFrame>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>

namespace Ui { class CModelMatcherComponent; }
namespace BlackGui
{
    namespace Views { class CAircraftModelView; }
    namespace Components
    {
        class CSettingsMatchingDialog;

        /*!
         * Model matcher testing and configuration
         */
        class BLACKGUI_EXPORT CModelMatcherComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelMatcherComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CModelMatcherComponent() override;

            //! Tab (where this component is embedded) has been changed
            void tabIndexChanged(int index);

            //! Set an external
            void setWorkbenchView(Views::CAircraftModelView *workbenchView);

        private:
            //! Simulator switched
            void onSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Workbench toggled
            void onWorkbenchToggled(bool checked);

            //! Cache changed
            void onCacheChanged(BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Web data have been read
            void onWebDataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

            //! Display settings dialog
            void displaySettingsDialog();

            //! Run the matcher
            void testModelMatching();

            //! Reverse lookup
            void reverseLookup();

            //! Init
            void redisplay();

            //! The current model set models
            BlackMisc::Simulation::CAircraftModelList getModelSetModels() const;

            //! The current model set models size
            int getMatcherModelsCount() const;

            //! Use workbench data
            bool useWorkbench() const;

            //! Pseudo aircraft created from entries
            BlackMisc::Simulation::CSimulatedAircraft createAircraft() const;

            //! Pseudo default aircraft
            BlackMisc::Simulation::CAircraftModel defaultModel() const;

            QScopedPointer<Ui::CModelMatcherComponent> ui;

            QPointer<Views::CAircraftModelView> m_workbenchView; //!< an external workbenc
            CSettingsMatchingDialog    *m_settingsDialog = nullptr; //!< matching settings as dialog
            BlackCore::CAircraftMatcher m_matcher { this };         //!< used matcher
        };
    } // ns
} // ns

#endif // guard
