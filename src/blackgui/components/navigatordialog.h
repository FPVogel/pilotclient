/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H
#define BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/settings/navigatorsettings.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>
#include <QWidgetAction>
#include <QTimer>
#include <QGridLayout>
#include <QTimer>
#include <QPoint>

class QEvent;
class QMenu;
class QMouseEvent;
class QWidget;

namespace Ui { class CNavigatorDialog; }
namespace BlackGui
{
    namespace Components
    {
        class CMarginsInput;

        /*!
         * Navigator dialog
         */
        class BLACKGUI_EXPORT CNavigatorDialog :
            public QDialog,
            public CEnableForFramelessWindow
        {
            Q_OBJECT

        public:
            //! Constructor
            CNavigatorDialog(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CNavigatorDialog() override;

            //! Navigator
            void buildNavigator(int columns);

            //! Called when dialog is closed
            virtual void reject() override;

            //! Toggle frameless mode
            void toggleFrameless();

            //! Visibility visibility
            void showNavigator(bool visible);

            //! Toggle visibility
            void toggleNavigatorVisibility();

            //! Restore from settings
            void restoreFromSettings();

            //! Save to settings
            void saveToSettings();

        signals:
            //! Navigator closed
            void navigatorClosed();

        protected:
            //! \name Base class events
            //! @{
            virtual void mouseMoveEvent(QMouseEvent *event) override;
            virtual void mousePressEvent(QMouseEvent *event) override;
            virtual void mouseReleaseEvent(QMouseEvent *event) override;
            virtual void changeEvent(QEvent *evt) override;
            virtual void windowFlagsChanged() override;
            virtual void paintEvent(QPaintEvent *event) override;
            virtual void enterEvent(QEvent *event) override;
            //! @}

        private:
            //! Margins context menu
            void menuChangeMargins(const QMargins &margins);

            //! Change the layout
            void changeLayout();

            //! Dummy slot
            void dummyFunction();

            //! Context menu
            void showContextMenu(const QPoint &pos);

            //! Style sheet has changed
            void onStyleSheetsChanged();

            //! Changed settigs
            void onSettingsChanged();

            //! Insert own actions
            void insertOwnActions();

            //! Contribute to menu
            void addToContextMenu(QMenu *contextMenu) const;

            //! How many columns for given rows
            int columnsForRows(int rows);

            //! Get my own grid layout
            QGridLayout *myGridLayout() const;

            //! Adjust navigator size
            void adjustNavigatorSize(QGridLayout *layout = nullptr);

            //! On watchdog
            void onWatchdog();

            QScopedPointer<Ui::CNavigatorDialog> ui;
            bool m_firstBuild = true;
            int  m_currentColumns = 1;
            QWidgetAction *m_marginMenuAction = nullptr; //!< menu widget(!) action for margin widget
            CMarginsInput *m_input = nullptr; //!< margins widget
            QTimer         m_watchdog;        //!< navigator watchdog
            BlackMisc::CSetting<BlackGui::Settings::TNavigator> m_settings { this, &CNavigatorDialog::onSettingsChanged };
        };
    } // ns
} // ns

#endif // guard
