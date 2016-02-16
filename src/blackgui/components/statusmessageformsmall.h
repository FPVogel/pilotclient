/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H
#define BLACKGUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include <QFrame>

namespace Ui { class CStatusMessageFormSmall; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Display details about a single status message
         */
        class BLACKGUI_EXPORT CStatusMessageFormSmall : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CStatusMessageFormSmall(QWidget *parent = nullptr);

            //! Destructor
            ~CStatusMessageFormSmall();

        public slots:
            //! Set message
            void setVariant(const BlackMisc::CVariant &messageVariant);

            //! Set message
            void setValue(const BlackMisc::CStatusMessage &message);

            //! Toggle visibility
            void toggleVisibility();

        private:
            QScopedPointer<Ui::CStatusMessageFormSmall> ui;
        };
    } // ns
} // ns
#endif // guard
