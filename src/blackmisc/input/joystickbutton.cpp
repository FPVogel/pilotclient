/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Input
    {
        CJoystickButton::CJoystickButton(const QString &deviceName, int index) :
            m_deviceName(deviceName), m_buttonIndex(index)
        {}

        QString CJoystickButton::getButtonAsStringWithDeviceName() const
        {
            return QStringLiteral("%1%2 - %3").arg(buttonIndentifier()).arg(m_buttonIndex).arg(m_deviceName);
        }

        void CJoystickButton::setButtonIndex(int buttonIndex)
        {
            m_buttonIndex = buttonIndex;
        }

        bool CJoystickButton::isValid() const
        {
            return (!m_deviceName.isEmpty() && m_buttonIndex >= 0);
        }

        void CJoystickButton::setButtonObject(CJoystickButton button)
        {
            m_deviceName  = button.m_deviceName;
            m_buttonIndex = button.m_buttonIndex;
        }

        void CJoystickButton::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CJoystickButton>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDeviceName:
                this->setDeviceName(variant.value<QString>());
                break;
            case IndexButton:
            case IndexButtonAsString:
                this->setButtonIndex(buttonIndexFromString(variant.value<QString>()));
                break;
            case IndexButtonObject:
                this->setButtonObject(variant.value<BlackMisc::Input::CJoystickButton>());
                break;
            default:
                Q_ASSERT_X(false, "CJoystickButton", "index unknown (setter)");
                break;
            }
        }

        CVariant CJoystickButton::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDeviceName:     return CVariant::from(this->getDeviceName());
            case IndexButton:         return CVariant::from(this->getButtonIndex());
            case IndexButtonAsString: return CVariant::from(this->getButtonAsString());
            case IndexButtonObject:   return CVariant::from(*this);
            }

            Q_ASSERT_X(false, "CJoystickButton", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return CVariant::fromValue(m);
        }

        QString CJoystickButton::buttonIndexToString(qint32 buttonIndex)
        {
            return buttonIndentifier() + QString::number(buttonIndex);
        }

        int CJoystickButton::buttonIndexFromString(const QString &buttonName)
        {
            if (!buttonName.startsWith(buttonIndentifier())) { return getInvalidIndex(); }
            QString name(buttonName);
            name.remove(buttonIndentifier());
            if (name.contains('-')) { name = name.mid(0, name.indexOf('-')); }
            return name.trimmed().toInt();
        }

        QString CJoystickButton::convertToQString(bool /* i18n */) const
        {
            return getButtonAsString().trimmed();
        }

    } // namespace
} // namespacexs
