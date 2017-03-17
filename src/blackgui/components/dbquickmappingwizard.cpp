/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbquickmappingwizard.h"
#include "ui_dbquickmappingwizard.h"
#include "dbaircrafticaoselectorcomponent.h"
#include "dbairlinenameselectorcomponent.h"
#include "blackgui/views/aircrafticaoview.h"
#include "blackgui/views/liveryview.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/databasewriter.h"

using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CDbQuickMappingWizard::CDbQuickMappingWizard(QWidget *parent) :
            QWizard(parent),
            ui(new Ui::CDbQuickMappingWizard)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");
            Q_ASSERT_X(sGui->getWebDataServices()->getDatabaseWriter(), Q_FUNC_INFO, "Missing writer");

            ui->setupUi(this);
            ui->selector_AircraftIcaoCode->setFocus();
            ui->selector_AircraftIcaoCode->displayWithIcaoDescription(false);
            ui->selector_AircraftIcaoCode->displayMode(CDbAircraftIcaoSelectorComponent::DisplayCompleterString);
            ui->selector_AirlineIcaoCode->displayWithIcaoDescription(false);
            ui->editor_AircraftModel->allowDrop(false);
            ui->editor_AircraftModel->setReadOnly(true);

            connect(sGui->getWebDataServices(), &CWebDataServices::allSwiftDbDataRead, this, &CDbQuickMappingWizard::ps_webDataRead);
            connect(sGui->getWebDataServices()->getDatabaseWriter(), &CDatabaseWriter::publishedModels, this, &CDbQuickMappingWizard::ps_publishedModels);

            connect(this, &CDbQuickMappingWizard::currentIdChanged, this, &CDbQuickMappingWizard::ps_currentWizardPageChanged);
            connect(ui->selector_AircraftIcaoCode, &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this, &CDbQuickMappingWizard::ps_aircraftSelected);
            connect(ui->selector_AirlineIcaoCode, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CDbQuickMappingWizard::ps_airlineSelected);
            connect(ui->selector_AirlineName, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CDbQuickMappingWizard::ps_airlineSelected);

            ui->comp_Log->showFilterDialog(); // filter for log normally not needed, so dialog (not bar)

            // init if data already available
            this->ps_webDataRead();
        }

        CDbQuickMappingWizard::~CDbQuickMappingWizard()
        { }

        const CLogCategoryList &CDbQuickMappingWizard::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { CLogCategory::mapping(), CLogCategory::guiComponent() };
            return cats;
        }

        void CDbQuickMappingWizard::keyPressEvent(QKeyEvent *event)
        {
            Qt::Key key = static_cast<Qt::Key>(event->key());
            if (key == Qt::Key_Enter || key == Qt::Key_Return)
            {
                // disable enter, interferes with filter returnPressed
                event->accept();
            }
            else
            {
                QWizard::keyPressEvent(event);
            }
        }

        void CDbQuickMappingWizard::presetAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao)
        {
            this->clear();
            ui->selector_AircraftIcaoCode->setAircraftIcao(aircraftIcao);
            ui->selector_AircraftIcaoCode->setFocus();
        }

        void CDbQuickMappingWizard::presetModel(const BlackMisc::Simulation::CAircraftModel &model)
        {
            QString ms = model.getModelString();
            if (!model.getDescription().isEmpty())
            {
                ms += " (" + model.getDescription() + ")";
            }

            this->presetAircraftIcao(model.getAircraftIcaoCode());
            ui->selector_AirlineIcaoCode->setAirlineIcao(model.getAirlineIcaoCode());
            ui->selector_AirlineName->setAirlineIcao(model.getAirlineIcaoCode());
            ui->comp_Distributor->view()->selectDbKey(model.getDistributor().getDbKey());
            ui->le_ModelString->setText(model.hasModelString() ? ms : "<no model string>");
            ui->selector_AircraftIcaoCode->setFocus();

            const CLivery livery(model.getLivery());
            if (livery.isColorLivery())
            {
                ui->comp_ColorSearch->presetColorLivery(livery);
                ui->rb_ColorLivery->setChecked(true);
            }

            this->m_model = model;
        }

        void CDbQuickMappingWizard::clear()
        {
            this->m_lastId = 0;
            ui->editor_AircraftModel->clear();
            ui->comp_Log->clear();
            this->restart();
        }

        void CDbQuickMappingWizard::setAircraftIcaoFilter()
        {
            const CAircraftIcaoCode icao(ui->selector_AircraftIcaoCode->getAircraftIcao());
            if (icao.isLoadedFromDb())
            {
                ui->comp_AircraftIcao->view()->sortByPropertyIndex(CAircraftIcaoCode::IndexRank);
                ui->comp_AircraftIcao->filter(icao);
                ui->comp_AircraftIcao->view()->selectDbKey(icao.getDbKey());
            }
        }

        void CDbQuickMappingWizard::setAirlineIcaoFilter()
        {
            const CAirlineIcaoCode icao(ui->selector_AirlineIcaoCode->getAirlineIcao());
            if (icao.isLoadedFromDb())
            {
                ui->comp_Livery->view()->sortByPropertyIndex(CLivery::IndexDbIntegerKey);
                ui->comp_Livery->filterByAirline(icao);
                ui->comp_Livery->view()->selectRow(0);
            }
        }

        void CDbQuickMappingWizard::setColorFilter()
        {
            const CLivery colorLivery(ui->comp_ColorSearch->getLivery());
            if (colorLivery.isLoadedFromDb())
            {
                ui->comp_Livery->filter(colorLivery);
                ui->comp_Livery->view()->selectRow(0);
            }
        }

        CLivery CDbQuickMappingWizard::getFirstSelectedOrDefaultLivery() const
        {
            return ui->comp_Livery->view()->firstSelectedOrDefaultObject();
        }

        CAircraftIcaoCode CDbQuickMappingWizard::getFirstSelectedOrDefaultAircraftIcao() const
        {
            return ui->comp_AircraftIcao->view()->firstSelectedOrDefaultObject();
        }

        BlackMisc::Simulation::CDistributor CDbQuickMappingWizard::getFirstSelectedOrDefaultDistributor() const
        {
            return ui->comp_Distributor->view()->firstSelectedOrDefaultObject();
        }

        void CDbQuickMappingWizard::ps_webDataRead()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
        }

        void CDbQuickMappingWizard::ps_publishedModels(const CAircraftModelList &modelsPublished, const CAircraftModelList &modelsSkipped, const CStatusMessageList &messages, bool requestSuccessful, bool directWrite)
        {
            Q_UNUSED(modelsPublished);
            Q_UNUSED(modelsSkipped);
            Q_UNUSED(directWrite);
            CStatusMessageList msgs;
            if (requestSuccessful)
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, "Publishing request sent"));
            }
            else
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, "Publishing request failed"));
            }
            msgs.push_back(messages);
            ui->comp_Log->appendStatusMessagesToList(msgs);
        }

        void CDbQuickMappingWizard::ps_currentWizardPageChanged(int id)
        {
            const bool forward = id > this->m_lastId;
            const bool colorMode = ui->rb_ColorLivery->isChecked();
            this->m_lastId = id;

            const Pages page = static_cast<Pages>(id);
            switch (page)
            {
            case PageAircraftSelect:
                {
                    this->setAircraftIcaoFilter();
                }
                break;
            case PageColor:
                if (!colorMode)
                {
                    forward ? this->next() : this->back();
                }
                break;
            case PageLiverySelect:
                {
                    if (colorMode)
                    {
                        this->setColorFilter();
                    }
                    else
                    {
                        this->setAirlineIcaoFilter();
                    }
                }
                break;
            case PageConfirmation:
                {
                    ui->editor_AircraftModel->setLivery(this->getFirstSelectedOrDefaultLivery());
                    ui->editor_AircraftModel->setDistributor(this->getFirstSelectedOrDefaultDistributor());
                    ui->editor_AircraftModel->setAircraftIcao(this->getFirstSelectedOrDefaultAircraftIcao());
                    const CStatusMessageList msgs(this->validateData());
                    const bool errorFree = !msgs.hasWarningOrErrorMessages();
                    ui->fr_ConfirmationOk->setVisible(errorFree);
                    ui->fr_ConfirmationStillErrors->setVisible(!errorFree);
                    if (!errorFree)
                    {
                        ui->editor_AircraftModel->showOverlayMessages(msgs);
                    }
                }
                break;
            case PageCredentials:
                {
                    ui->comp_Log->clear();
                }
                break;
            case PageSendStatus:
                {
                    this->writeModelToDb();
                }
                break;
            default:
                break;
            }
        }

        bool CDbQuickMappingWizard::validateCurrentPage()
        {
            const Pages page = static_cast<Pages>(this->currentId());
            bool ok = false;
            switch (page)
            {
            case PageConfirmation:
                {
                    const CStatusMessageList msgs(this->validateData());
                    ok = !msgs.hasWarningOrErrorMessages();
                }
                break;
            case PageCredentials:
                {
                    ok = ui->comp_DbLogin->isUserAuthenticated();
                }
                break;
            default:
                {
                    ok = true;
                }
                break;
            }

            return ok;
        }

        CStatusMessageList CDbQuickMappingWizard::validateData() const
        {
            CStatusMessageList msgs(ui->editor_AircraftModel->validate(true));
            if (ui->le_ModelString->text().isEmpty())
            {
                const CStatusMessage error(this, CStatusMessage::SeverityError, "Missing model string", true);
                msgs.push_back(error);
            }
            return msgs;
        }

        void CDbQuickMappingWizard::consolidateModel()
        {
            CAircraftModel model = this->m_model;
            model.setAircraftIcaoCode(ui->editor_AircraftModel->getAircraftIcao());
            model.setDistributor(ui->editor_AircraftModel->getDistributor());
            model.setLivery(ui->editor_AircraftModel->getLivery());
            this->m_model = model;
        }

        void CDbQuickMappingWizard::writeModelToDb()
        {
            this->consolidateModel();
            const CStatusMessageList msgs = sGui->getWebDataServices()->getDatabaseWriter()->asyncPublishModel(this->m_model);
            ui->comp_Log->appendStatusMessagesToList(msgs);
        }

        void CDbQuickMappingWizard::ps_airlineSelected(const CAirlineIcaoCode &icao)
        {
            if (icao.isLoadedFromDb())
            {
                ui->cb_VirtualAirline->setChecked(icao.isVirtualAirline());
                ui->cb_Military->setChecked(icao.isMilitary());
                ui->selector_AirlineName->setAirlineIcao(icao);
                ui->selector_AirlineIcaoCode->setAirlineIcao(icao);
                // already trigger sorting, if sorting is already correct it does nothing
                // avoids issue with later selection overridden by sorting/filtering
                this->setAirlineIcaoFilter();
            }
        }

        void CDbQuickMappingWizard::ps_aircraftSelected(const CAircraftIcaoCode &icao)
        {
            if (icao.isLoadedFromDb())
            {
                ui->cb_Military->setChecked(icao.isMilitary());
                // already trigger sorting, if sorting is already correct it does nothing
                // avoids issue with later selection overridden by sorting/filtering
                this->setAircraftIcaoFilter();
            }
        }
    } // ns
} // ns
