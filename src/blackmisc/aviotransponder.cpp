/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviotransponder.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Valid values?
         */
        bool CTransponder::validValues() const
        {
            if (this->isDefaultValue()) return true; // special case
            return CTransponder::isValidTransponderCode(this->m_transponderCode);
        }

        /*
         * Validate
         */
        bool CTransponder::validate(bool strict) const
        {
            if (this->isDefaultValue()) return true;
            bool valid = this->validValues();
            if (!strict) return valid;
            Q_ASSERT_X(valid, "CTransponder::validate", "illegal values");
            if (!valid)
                throw std::range_error("Illegal values in CTransponder::validate");
            return true;
        }

        /*
         * String representation
         */
        QString CTransponder::convertToQString(bool /* i18n */) const
        {
            QString s = this->getName();
            s = s.append(" ").append(this->getTransponderCodeFormatted()).append(" ").append(this->getModeAsString());
            return s;
        }

        /*
         * Mode as readable string
         */
        QString CTransponder::getModeAsString() const
        {
            QString m;
            switch (this->getTransponderMode())
            {
            case StateIdent:
                m = "Ident";
                break;
            case StateStandby:
                m = "Standby";
                break;
            case ModeC:
                m = "Mode C";
                break;
            case ModeS:
                m = "Mode S";
                break;
            case ModeMil1:
                m = "Mil.Mode 1";
                break;
            case ModeMil2:
                m = "Mil.Mode 2";
                break;
            case ModeMil3:
                m = "Mil.Mode 3";
                break;
            case ModeMil4:
                m = "Mil.Mode 4";
                break;
            case ModeMil5:
                m = "Mil.Mode 5";
                break;
            default:
                throw std::range_error("Illegal Transponder Mode");
            }
            return m;
        }

        /*
         * Mode as readable string
         */
        void CTransponder::setModeAsString(const QString &m)
        {
            if (m == "Ident")
            {
                this->setTransponderMode(StateIdent);
            }
            else if (m == "Standby")
            {
                this->setTransponderMode(StateStandby);
            }
            else if (m == "Mode C")
            {
                this->setTransponderMode(ModeC);
            }
            else if (m == "Mode S")
            {
                this->setTransponderMode(ModeS);
            }
            else if (m == "Mil.Mode 1")
            {
                this->setTransponderMode(ModeMil1);
            }
            else if (m == "Mil.Mode 2")
            {
                this->setTransponderMode(ModeMil2);
            }
            else if (m == "Mil.Mode 3")
            {
                this->setTransponderMode(ModeMil3);
            }
            else if (m == "Mil.Mode 4")
            {
                this->setTransponderMode(ModeMil4);
            }
            else if (m == "Mil.Mode 5")
            {
                this->setTransponderMode(ModeMil5);
            }
            else
            {
                throw std::range_error("Illegal Transponder Mode");
            }
        }

        /*
         * Formatted transponder code
         */
        QString CTransponder::getTransponderCodeFormatted() const
        {
            QString f("0000");
            f = f.append(QString::number(this->m_transponderCode));
            return f.right(4);
        }

        /*
         * Formatted transponder code + mode
         */
        QString CTransponder::getTransponderCodeAndModeFormatted() const
        {
            QString s = this->getTransponderCodeFormatted();
            s.append(' ').append(this->getModeAsString());
            return s;
        }

        /*
         * Transponder by string
         */
        void CTransponder::setTransponderCode(const QString &transponderCode)
        {
            if (CTransponder::isValidTransponderCode(transponderCode))
            {
                bool ok;
                this->setTransponderCode(transponderCode.toInt(&ok));
            }
            else
            {
                Q_ASSERT_X(false, "CTransponder::setTransponderCode", "illegal transponder value");
            }
        }

        /*
         * Valid code?
         */
        bool CTransponder::isValidTransponderCode(const QString &transponderCode)
        {
            if (transponderCode.isEmpty() || transponderCode.length() > 4) return false;
            bool number;
            qint32 tc = transponderCode.toInt(&number);
            if (!number) return false;
            if (tc < 0 || tc > 7777) return false;
            QRegExp rx("[0-7]{1,4}");
            return rx.exactMatch(transponderCode);
        }

        /*
         * Valid code?
         */
        bool CTransponder::isValidTransponderCode(qint32 transponderCode)
        {
            if (transponderCode < 0 || transponderCode > 7777) return false;
            return CTransponder::isValidTransponderCode(QString::number(transponderCode));
        }

        /*
         * Marshall
         */
        void CTransponder::marshallToDbus(QDBusArgument &argument) const
        {
            CAvionicsBase::marshallToDbus(argument);
            argument << TupleConverter<CTransponder>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CTransponder::unmarshallFromDbus(const QDBusArgument &argument)
        {
            CAvionicsBase::unmarshallFromDbus(argument);
            argument >> TupleConverter<CTransponder>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CTransponder::getValueHash() const
        {
            QList<uint> hashs;
            hashs << CAvionicsBase::getValueHash();
            hashs << qHash(TupleConverter<CTransponder>::toTuple(*this));
            return BlackMisc::calculateHash(hashs, "CTransponder");
        }

        /*
         * Compare
         */
        int CTransponder::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CTransponder &>(otherBase);
            int result = compare(TupleConverter<CTransponder>::toTuple(*this), TupleConverter<CTransponder>::toTuple(other));
            return result == 0 ? CAvionicsBase::compareImpl(otherBase) : result;
        }

        /*
         * Register metadata of unit and quantity
         */
        void CTransponder::registerMetadata()
        {
            qRegisterMetaType<CTransponder>();
            qDBusRegisterMetaType<CTransponder>();
        }

        /*
         * Members
         */
        const QStringList &CTransponder::jsonMembers()
        {
            return TupleConverter<CTransponder>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CTransponder::toJson() const
        {
            return BlackMisc::serializeJson(CTransponder::jsonMembers(), TupleConverter<CTransponder>::toTuple(*this));
        }

        /*
         * From Json
         */
        void CTransponder::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CTransponder::jsonMembers(), TupleConverter<CTransponder>::toTuple(*this));
        }
    } // namespace
} // namespace
