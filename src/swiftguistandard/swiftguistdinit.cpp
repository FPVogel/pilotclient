/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextnetwork.h"
#include "blackgui/components/aircraftcomponent.h"
#include "blackgui/components/atcstationcomponent.h"
#include "blackgui/components/cockpitcomponent.h"
#include "blackgui/components/flightplancomponent.h"
#include "blackgui/components/logcomponent.h"
#include "blackgui/components/logincomponent.h"
#include "blackgui/components/maininfoareacomponent.h"
#include "blackgui/components/mainkeypadareacomponent.h"
#include "blackgui/components/mappingcomponent.h"
#include "blackgui/components/navigatordialog.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/components/textmessagecomponent.h"
#include "blackgui/components/usercomponent.h"
#include "blackgui/dockwidgetinfobar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/buildconfig.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logpattern.h"
#include "blackmisc/statusmessage.h"
#include "swiftguistd.h"
#include "ui_swiftguistd.h"

#include <QAction>
#include <QHBoxLayout>
#include <QScopedPointer>
#include <QStackedWidget>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>

class QHBoxLayout;

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Input;
using namespace BlackGui;
using namespace BlackGui::Components;

void SwiftGuiStd::init()
{
    // POST(!) GUI init

    if (this->m_init) { return; }

    this->setVisible(false); // hide all, so no flashing windows during init
    this->m_mwaStatusBar = &this->m_statusBar;
    this->m_mwaOverlayFrame = this->ui->fr_CentralFrameInside;
    this->m_mwaLogComponent = this->ui->comp_MainInfoArea->getLogComponent();

    sGui->initMainApplicationWindow(this);
    this->initStyleSheet();

    // with frameless window, we shift menu and statusbar into central widget
    // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    if (this->isFrameless())
    {
        // wrap menu in layout, add button to menu bar and insert on top
        QHBoxLayout *menuBarLayout = this->addFramelessCloseButton(this->ui->mb_MainMenuBar);
        this->ui->vl_CentralWidgetOutside->insertLayout(0, menuBarLayout, 0);

        // now insert the dock widget info bar into the widget
        this->ui->vl_CentralWidgetOutside->insertWidget(1, this->ui->dw_InfoBarStatus);

        // move the status bar into the frame
        // (otherwise it is dangling outside the frame as it belongs to the window)
        this->ui->sb_MainStatusBar->setParent(this->ui->wi_CentralWidgetOutside);
        this->ui->vl_CentralWidgetOutside->addWidget(this->ui->sb_MainStatusBar, 0);

        // grip
        this->addFramelessSizeGripToStatusBar(this->ui->sb_MainStatusBar);
    }

    // timers
    if (this->m_timerContextWatchdog == nullptr)
    {
        this->m_timerContextWatchdog = new QTimer(this);
        this->m_timerContextWatchdog->setObjectName(this->objectName().append(":m_timerContextWatchdog"));
    }

    // info bar and status bar
    this->m_statusBar.initStatusBar(this->ui->sb_MainStatusBar);
    this->ui->dw_InfoBarStatus->allowStatusBar(false);
    this->ui->dw_InfoBarStatus->setPreferredSizeWhenFloating(this->ui->dw_InfoBarStatus->size()); // set floating size

    // navigator
    this->m_navigator->addAction(this->getToggleWindowVisibilityAction(this->m_navigator.data()));
    this->m_navigator->addActions(this->ui->comp_MainInfoArea->getInfoAreaToggleFloatingActions(this->m_navigator.data()));
    this->m_navigator->addAction(this->getWindowNormalAction(this->m_navigator.data()));
    this->m_navigator->addAction(this->getWindowMinimizeAction(this->m_navigator.data()));
    this->m_navigator->addAction(this->getToggleStayOnTopAction(this->m_navigator.data()));
    this->m_navigator->buildNavigator(1);

    // wire GUI signals
    this->initGuiSignals();

    // signal / slots contexts / timers
    connect(sGui->getIContextNetwork(), &IContextNetwork::connectionTerminated, this, &SwiftGuiStd::ps_onConnectionTerminated);
    connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &SwiftGuiStd::ps_onConnectionStatusChanged);
    connect(sGui->getIContextNetwork(), &IContextNetwork::textMessagesReceived, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::onTextMessageReceived);
    connect(sGui->getIContextNetwork(), &IContextNetwork::textMessageSent, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::onTextMessageSent);
    connect(this->m_timerContextWatchdog, &QTimer::timeout, this, &SwiftGuiStd::ps_handleTimerBasedUpdates);

    // log messages
    m_logSubscriber.changeSubscription(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo));

    // start timers, update timers will be started when network is connected
    this->m_timerContextWatchdog->start(2500);

    // init availability
    this->setContextAvailability();

    // data
    this->initialDataReads();

    // start screen and complete menu
    this->ps_setMainPageToInfoArea();
    this->initMenus();

    // info
    this->ui->comp_MainInfoArea->getLogComponent()->appendPlainTextToConsole(sGui->swiftVersionString());
    this->ui->comp_MainInfoArea->getLogComponent()->appendPlainTextToConsole(CBuildConfig::compiledWithInfo());

    // update timers
    this->startUpdateTimersWhenConnected();

    // do this as last statement, so it can be used as flag
    // whether init has been completed
    this->setVisible(true);

    sGui->startUpCompleted(true);
    this->m_init = true;
}

void SwiftGuiStd::initStyleSheet()
{
    const QString s = sGui->getStyleSheetUtility().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameStandardWidget(),
        CStyleSheetUtility::fileNameSwiftStandardGui()
    }
    );
    this->setStyleSheet(s);
}

void SwiftGuiStd::initGuiSignals()
{
    // Remark: With new style, only methods of same signature can be connected
    // This is why we still have some "old" SIGNAL/SLOT connections here

    // main window
    connect(this->ui->sw_MainMiddle, &QStackedWidget::currentChanged, this, &SwiftGuiStd::ps_onCurrentMainWidgetChanged);

    // main keypad
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::selectedMainInfoAreaDockWidget, this, &SwiftGuiStd::ps_setMainPageInfoArea);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::connectPressed, this, &SwiftGuiStd::ps_loginRequested);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::changedOpacity, this , &SwiftGuiStd::ps_onChangedWindowOpacity);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::identPressed, this->ui->comp_MainInfoArea->getCockpitComponent(), &CCockpitComponent::setSelectedTransponderModeStateIdent);
    connect(this->ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::commandEntered, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::handleGlobalCommandLine);
    connect(this->ui->comp_MainInfoArea, &CMainInfoAreaComponent::changedInfoAreaStatus, ui->comp_MainKeypadArea, &CMainKeypadAreaComponent::onMainInfoAreaChanged);

    // menu
    connect(this->ui->menu_TestLocationsEDDF, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDDM, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDNX, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsEDRY, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_TestLocationsLOWW, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);

    connect(this->ui->menu_WindowFont, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowMinimize, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowToggleOnTop, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);
    connect(this->ui->menu_WindowToggleNavigator, &QAction::triggered, this->m_navigator.data(), &CNavigatorDialog::toggleNavigator);
    connect(this->ui->menu_InternalsPage, &QAction::triggered, this, &SwiftGuiStd::ps_onMenuClicked);

    // command line / text messages
    connect(this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::displayInInfoWindow, this->ui->fr_CentralFrameInside, &COverlayMessagesFrame::showOverlayVariant);

    // settings (GUI component), styles
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedWindowsOpacity, this, &SwiftGuiStd::ps_onChangedWindowOpacity);
    connect(sGui, &CGuiApplication::styleSheetsChanged, this, &SwiftGuiStd::ps_onStyleSheetsChanged);

    // sliders
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedUsersUpdateInterval, this->ui->comp_MainInfoArea->getUserComponent(), &CUserComponent::setUpdateIntervalSeconds);
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAircraftUpdateInterval, this->ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::setUpdateIntervalSeconds);
    connect(this->ui->comp_MainInfoArea->getSettingsComponent(), &CSettingsComponent::changedAtcStationsUpdateInterval, this->ui->comp_MainInfoArea->getAtcStationComponent(), &::CAtcStationComponent::setUpdateIntervalSeconds);

    // login
    connect(this->ui->comp_Login, &CLoginComponent::loginOrLogoffCancelled, this, &SwiftGuiStd::ps_setMainPageToInfoArea);
    connect(this->ui->comp_Login, &CLoginComponent::loginOrLogoffSuccessful, this, &SwiftGuiStd::ps_setMainPageToInfoArea);
    connect(this->ui->comp_Login, &CLoginComponent::loginOrLogoffSuccessful, this->ui->comp_MainInfoArea->getFlightPlanComponent(), &CFlightPlanComponent::loginDataSet);
    connect(this, &SwiftGuiStd::currentMainInfoAreaChanged, this->ui->comp_Login, &CLoginComponent::mainInfoAreaChanged);
    connect(this->ui->comp_Login, &CLoginComponent::requestNetworkSettings, this->ui->comp_MainInfoArea->getFlightPlanComponent(), [ = ]()
    {
        this->ps_setMainPageInfoArea(CMainInfoAreaComponent::InfoAreaSettings);
        this->ui->comp_MainInfoArea->getSettingsComponent()->setSettingsTab(CSettingsComponent::SettingTabNetworkServers);
    });

    // text messages
    connect(this->ui->comp_MainInfoArea->getAtcStationComponent(), &CAtcStationComponent::requestTextMessageWidget, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab);
    connect(this->ui->comp_MainInfoArea->getMappingComponet(), &CMappingComponent::requestTextMessageWidget, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab);
    connect(this->ui->comp_MainInfoArea->getAircraftComponent(), &CAircraftComponent::requestTextMessageWidget, this->ui->comp_MainInfoArea->getTextMessageComponent(), &CTextMessageComponent::showCorrespondingTab);

    // main info area
    connect(this->ui->comp_MainInfoArea, &CMainInfoAreaComponent::changedWholeInfoAreaFloating, this, &SwiftGuiStd::ps_onChangedMainInfoAreaFloating);
}

void SwiftGuiStd::initialDataReads()
{
    this->setContextAvailability();
    if (!this->m_coreAvailable)
    {
        CLogMessage(this).error("No initial data read as network context is not available");
        return;
    }

    this->ps_reloadOwnAircraft(); // init read, independent of traffic network
    CLogMessage(this).info("Initial data read");
}

void SwiftGuiStd::startUpdateTimersWhenConnected()
{
    this->ui->comp_MainInfoArea->getAtcStationComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAtcUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getAircraftComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getAircraftUpdateIntervalSeconds());
    this->ui->comp_MainInfoArea->getUserComponent()->setUpdateIntervalSeconds(this->ui->comp_MainInfoArea->getSettingsComponent()->getUsersUpdateIntervalSeconds());
}

void SwiftGuiStd::stopUpdateTimersWhenDisconnected()
{
    this->ui->comp_MainInfoArea->getAtcStationComponent()->stopTimer();
    this->ui->comp_MainInfoArea->getAircraftComponent()->stopTimer();
    this->ui->comp_MainInfoArea->getUserComponent()->stopTimer();
}

void SwiftGuiStd::stopAllTimers(bool disconnectSignalSlots)
{
    this->m_timerContextWatchdog->stop();
    this->stopUpdateTimersWhenDisconnected();
    if (!disconnectSignalSlots) { return; }
    this->disconnect(this->m_timerContextWatchdog);
}
