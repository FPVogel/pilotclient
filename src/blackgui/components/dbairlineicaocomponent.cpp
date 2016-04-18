/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbairlineicaocomponent.h"
#include "ui_dbairlineicaocomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include <functional>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackGui;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbAirlineIcaoComponent::CDbAirlineIcaoComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAirlineIcaoComponent)
        {
            ui->setupUi(this);
            this->setViewWithIndicator(this->ui->tvp_AirlineIcao);
            this->ui->tvp_AirlineIcao->setResizeMode(CAirlineIcaoCodeView::ResizingOff);
            this->ui->tvp_AirlineIcao->setResizeMode(CAirlineIcaoCodeView::ResizingOff);
            this->ui->tvp_AirlineIcao->allowDragDrop(true, false);
            this->ui->tvp_AirlineIcao->setFilterWidget(this->ui->filter_AirlineIcao);
            connect(this->ui->tvp_AirlineIcao, &CAirlineIcaoCodeView::requestNewBackendData, this, &CDbAirlineIcaoComponent::ps_reload);

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAirlineIcaoComponent::ps_icaoRead);
            this->ps_icaoRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getAirlineIcaoCodesCount());
        }

        CDbAirlineIcaoComponent::~CDbAirlineIcaoComponent()
        { }

        void CDbAirlineIcaoComponent::ps_icaoRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::AirlineIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_AirlineIcao->updateContainerMaybeAsync(sGui->getWebDataServices()->getAirlineIcaoCodes());
            }
        }

        void CDbAirlineIcaoComponent::ps_reload()
        {
            if (!sGui) { return; }
            sGui->getWebDataServices()->triggerRead(CEntityFlags::AirlineIcaoEntity, QDateTime());
        }

    } // ns
} // ns
