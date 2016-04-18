/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbcountrycomponent.h"
#include "ui_dbcountrycomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include <functional>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbCountryComponent::CDbCountryComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbCountryComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(this->ui->tvp_Countries);
            this->ui->tvp_Countries->setResizeMode(CViewBaseNonTemplate::ResizingOnce);
            connect(this->ui->tvp_Countries, &CCountryView::requestNewBackendData, this, &CDbCountryComponent::ps_reload);

            // filter and drag and drop
            this->ui->tvp_Countries->setFilterWidget(this->ui->filter_CountryComponent);
            this->ui->tvp_Countries->allowDragDrop(true, false);

            connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbCountryComponent::ps_countriesRead);
            this->ps_countriesRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getCountriesCount());
        }

        CDbCountryComponent::~CDbCountryComponent()
        { }

        void CDbCountryComponent::ps_countriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::CountryEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_Countries->updateContainerMaybeAsync(sApp->getWebDataServices()->getCountries());
            }
        }

        void CDbCountryComponent::ps_reload()
        {
            if (!sGui) { return; }
            sApp->getWebDataServices()->triggerRead(CEntityFlags::CountryEntity);
        }
    } // ns
} // ns
