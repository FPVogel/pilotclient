/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbownmodelsdialog.h"
#include "dbownmodelscomponent.h"
#include "ui_dbownmodelsdialog.h"

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CDbOwnModelsDialog::CDbOwnModelsDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CDbOwnModelsDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
            connect(ui->comp_OwnModels, &CDbOwnModelsComponent::successfullyLoadedModels, this, &CDbOwnModelsDialog::successfullyLoadedModels);
        }

        CDbOwnModelsDialog::~CDbOwnModelsDialog()
        { }

        void CDbOwnModelsDialog::setSimulator(const CSimulatorInfo &simulator)
        {
            ui->comp_OwnModels->setSimulator(simulator);
        }

        bool CDbOwnModelsDialog::requestModelsInBackground(const CSimulatorInfo &simulator, bool onlyIfNotEmpty)
        {
            return ui->comp_OwnModels->requestModelsInBackground(simulator, onlyIfNotEmpty);
        }

        const CDbOwnModelsComponent *CDbOwnModelsDialog::modelsComponent() const
        {
            return ui->comp_OwnModels;
        }
    } // ns
} // ns
