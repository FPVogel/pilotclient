/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context_application_impl.h"
#include "context_runtime.h"
#include "input_manager.h"
#include "settingscache.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include <QtMsgHandler>
#include <QFile>

using namespace BlackMisc;

namespace BlackCore
{
    CContextApplication::CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextApplication(mode, runtime)
    {}

    CContextApplication *CContextApplication::registerWithDBus(CDBusServer *server)
    {
        if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) { return this; }
        server->addObject(IContextApplication::ObjectPath(), this);
        return this;
    }

    void CContextApplication::logMessage(const CStatusMessage &message, const CIdentifier &origin)
    {
        if (!origin.isFromSameProcess())
        {
            CLogHandler::instance()->logRemoteMessage(message);
        }
        if (subscribersOf(message).containsAnyNotIn(CIdentifierList({ origin, {} })))
        {
            emit this->messageLogged(message, origin);
        }
    }

    void CContextApplication::addLogSubscription(const CIdentifier &subscriber, const CLogPattern &pattern)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        emit this->logSubscriptionAdded(subscriber, pattern);
    }

    void CContextApplication::removeLogSubscription(const CIdentifier &subscriber, const CLogPattern &pattern)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        emit this->logSubscriptionRemoved(subscriber, pattern);
    }

    CLogSubscriptionHash CContextApplication::getAllLogSubscriptions() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return m_logSubscriptions;
    }

    void CContextApplication::synchronizeLogSubscriptions()
    {
        // no-op: proxy implements this method by calling getAllLogSubscriptions
    }

    void CContextApplication::changeSettings(const CValueCachePacket &settings, const CIdentifier &origin)
    {
        // Intentionally don't check for round trip here
        emit this->settingsChanged(settings, origin);
    }

    BlackMisc::CValueCachePacket CContextApplication::getAllSettings() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return CSettingsCache::instance()->getAllValuesWithTimestamps();
    }

    void CContextApplication::synchronizeLocalSettings()
    {
        // no-op: proxy implements this method by calling getAllSettings
    }

    BlackMisc::CStatusMessage CContextApplication::saveSettings(const QString &keyPrefix)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << keyPrefix; }
        return CSettingsCache::instance()->saveToStore(keyPrefix);
    }

    BlackMisc::CStatusMessage CContextApplication::loadSettings()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return CSettingsCache::instance()->loadFromStore();
    }

    void CContextApplication::registerHotkeyActions(const QStringList &actions, const CIdentifier &origin)
    {
        // Intentionally don't check for round trip here
        emit this->hotkeyActionsRegistered(actions, origin);
    }

    void CContextApplication::callHotkeyAction(const QString &action, bool argument, const CIdentifier &origin)
    {
        // Intentionally don't check for round trip here
        emit this->remoteHotkeyAction(action, argument, origin);
    }

    bool CContextApplication::writeToFile(const QString &fileName, const QString &content)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName << content.left(25); }
        if (fileName.isEmpty()) { return false; }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << content;
            return true;
        }
        else
        {
            return false;
        }
    }

    CIdentifier CContextApplication::registerApplication(const CIdentifier &application)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << application; }
        if (!m_registeredApplications.contains(application))
        {
            m_registeredApplications.push_back(application);
            emit registrationChanged();
            emit this->hotkeyActionsRegistered(CInputManager::instance()->allAvailableActions(), {});
        }
        else
        {
            m_registeredApplications.replace(application, application);
        }

        return application;
    }

    void CContextApplication::unregisterApplication(const CIdentifier &application)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << application; }
        int r = this->m_registeredApplications.remove(application);
        if (r > 0) { emit registrationChanged(); }
    }

    CIdentifierList CContextApplication::getRegisteredApplications() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return m_registeredApplications;
    }

    QString CContextApplication::readFromFile(const QString &fileName) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName; }
        QFile file(fileName);
        QString content;
        if (fileName.isEmpty()) return content;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            in >> content;
            file.close();
        }
        return content;
    }

    bool CContextApplication::removeFile(const QString &fileName)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName; }
        if (fileName.isEmpty()) { return false; }
        return QFile::remove(fileName);
    }

    bool CContextApplication::existsFile(const QString &fileName) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName; }
        if (fileName.isEmpty()) return false;
        return QFile::exists(fileName);
    }

} // namespace
