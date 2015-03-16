/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEXTMESSAGECOMPONENT_H
#define BLACKGUI_TEXTMESSAGECOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/textmessagetextedit.h"
#include "blackmisc/nwtextmessage.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackcore/context_network.h"
#include "blackcore/context_ownaircraft.h"
#include <QFrame>
#include <QLineEdit>
#include <QScopedPointer>

namespace Ui { class CTextMessageComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Text message widget
        class CTextMessageComponent :
            public QFrame,
            public CEnableForRuntime,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Tabs
            enum Tab
            {
                TextMessagesAll,
                TextMessagesUnicom,
                TextMessagesCom1,
                TextMessagesCom2
            };

            //! Constructor
            explicit CTextMessageComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CTextMessageComponent();

        signals:
            //! Message to be displayed in info window
            void displayInInfoWindow(const BlackMisc::CVariant &message, int displayDurationMs) const;

            //! Command line was entered
            void commandEntered(const QString commandLine, const QString &orignator);

        public slots:
            //! Text messages received
            void onTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);

            //! Text messages sent
            void onTextMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

            //! Used to allow direct input from global command line when visible
            bool handleGlobalCommandLine(const QString &commandLine, const QString &originator);

            //! Display the tab for given callsign
            void showCorrespondingTab(const BlackMisc::Aviation::CCallsign &callsign);

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private:
            QScopedPointer<Ui::CTextMessageComponent> ui;

            //! Enum to widget
            QWidget *getTabWidget(Tab tab) const;

            //! Select given tab
            void selectTabWidget(Tab tab);

            //! New message tab for given callsign
            QWidget *addNewTextMessageTab(const BlackMisc::Aviation::CCallsign &callsign);

            //! Add new text message tab
            //! \param tabName   name of the new tab, usually the channel name
            //! \return
            QWidget *addNewTextMessageTab(const QString &tabName);

            //! Find text message tab by callsign
            QWidget *findTextMessageTabByCallsign(const BlackMisc::Aviation::CCallsign &callsign, bool callsignResolution = false) const;

            //! Find text message tab by its name
            QWidget *findTextMessageTabByName(const QString &name) const;

            //! Private channel text message
            void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage);

            //! own aircraft
            const BlackMisc::Aviation::CAircraft getOwnAircraft() const;

            //! For this text message's recepient, is the current tab selected?
            bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

            //! Network connected?
            bool isNetworkConnected() const;

            //! Show current frequencies
            void showCurrentFrequenciesFromCockpit();

            //! Append text messages (received, to be sent) to GUI
            void displayTextMessage(const BlackMisc::Network::CTextMessageList &messages);

            //! \copydoc IContextOwnAircraft::parseCommandLine
            QString textMessageToCommand(const QString &enteredLine);

            //! Originator
            static const QString &componentOriginator();

            //! Handle a text message entered
            void handleEnteredTextMessage(const QString &textMessage);

        private slots:

            //! Cockpit values changed, used to updated some components
            void ps_onChangedAircraftCockpit();

            //! Close text message tab
            void ps_closeTextMessageTab();

            //! Top level was changed (used to enable elements when floating)
            void ps_topLevelChanged(QWidget *widget, bool topLevel);

            //! Command line entered
            void ps_textMessageEntered();
        };
    }
}
#endif // guard
