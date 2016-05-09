/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/datastoreutility.h"
#include <tuple>
#include <QThreadStorage>
#include <QRegularExpression>
#include "blackmisc/logmessage.h"

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {
        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator)
            : m_designator(airlineDesignator.trimmed().toUpper())
        {}

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const BlackMisc::CCountry &country, const QString &telephony, bool virtualAirline, bool operating)
            : m_designator(airlineDesignator.trimmed().toUpper()), m_name(airlineName), m_country(country), m_telephonyDesignator(telephony), m_isVa(virtualAirline), m_isOperating(operating)
        {}

        const QString CAirlineIcaoCode::getVDesignator() const
        {
            if (!isVirtualAirline()) { return this->m_designator; }
            return QString("V").append(this->m_designator);
        }

        void CAirlineIcaoCode::setDesignator(const QString &icaoDesignator)
        {
            this->m_designator = icaoDesignator.trimmed().toUpper();
            if (this->m_designator.length() == 4 && this->m_designator.startsWith("V"))
            {
                // a virtual designator was provided
                this->setVirtualAirline(true);
                this->m_designator = this->m_designator.right(3);
            }
        }

        QString CAirlineIcaoCode::getDesignatorNameCountry() const
        {
            QString s(this->getDesignator());
            if (this->hasName()) { s = s.append(" ").append(this->getName()); }
            if (this->hasValidCountry()) { s = s.append(" ").append(this->getCountryIso()); }
            return s.trimmed();
        }

        bool CAirlineIcaoCode::hasValidCountry() const
        {
            return this->m_country.isValid();
        }

        bool CAirlineIcaoCode::hasValidDesignator() const
        {
            return isValidAirlineDesignator(m_designator);
        }

        bool CAirlineIcaoCode::hasIataCode() const
        {
            return !this->m_iataCode.isEmpty();
        }

        bool CAirlineIcaoCode::matchesDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return designator.trimmed().toUpper() == this->m_designator;
        }

        bool CAirlineIcaoCode::matchesVDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return designator.trimmed().toUpper() == this->getVDesignator();
        }

        bool CAirlineIcaoCode::matchesIataCode(const QString &iata) const
        {
            if (iata.isEmpty()) { return false; }
            return iata.trimmed().toUpper() == this->m_iataCode;
        }

        bool CAirlineIcaoCode::matchesDesignatorOrIataCode(const QString &candidate) const
        {
            if (candidate.isEmpty()) { return false; }
            return this->matchesDesignator(candidate) || this->matchesIataCode(candidate);
        }

        bool CAirlineIcaoCode::matchesVDesignatorOrIataCode(const QString &candidate) const
        {
            if (candidate.isEmpty()) { return false; }
            return this->matchesVDesignator(candidate) || this->matchesIataCode(candidate);
        }

        bool CAirlineIcaoCode::hasCompleteData() const
        {
            return this->hasValidDesignator() && this->hasValidCountry() && this->hasName();
        }

        CIcon CAirlineIcaoCode::toIcon() const
        {
            if (this->m_designator.length() > 2)
            {
                // relative to images
                return CIcon("airlines/" + m_designator.toLower() + ".png",
                             this->convertToQString());
            }
            else
            {
                return CIcon::iconByIndex(CIcons::StandardIconEmpty);
            }
        }

        QString CAirlineIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_designator);
            if (this->m_name.isEmpty()) { return ""; }
            if (!this->m_name.isEmpty()) { s.append(" (").append(this->m_name).append(")"); }

            s.append(" Op: ").append(boolToYesNo(this->isOperating()));
            s.append(" VA: ").append(boolToYesNo(this->isVirtualAirline()));
            s.append(" Mil: ").append(boolToYesNo(this->isMilitary()));

            return s;
        }

        CVariant CAirlineIcaoCode::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                return CVariant::fromValue(this->m_designator);
            case IndexIataCode:
                return CVariant::fromValue(this->m_iataCode);
            case IndexAirlineCountryIso:
                return CVariant::fromValue(this->getCountryIso());
            case IndexAirlineCountry:
                return this->m_country.propertyByIndex(index.copyFrontRemoved());
            case IndexAirlineName:
                return CVariant::fromValue(this->m_name);
            case IndexTelephonyDesignator:
                return CVariant::fromValue(this->m_telephonyDesignator);
            case IndexIsVirtualAirline:
                return CVariant::fromValue(this->m_isVa);
            case IndexIsOperating:
                return CVariant::fromValue(this->m_isOperating);
            case IndexIsMilitary:
                return CVariant::fromValue(this->m_isMilitary);
            case IndexDesignatorNameCountry:
                return CVariant::fromValue(this->getDesignatorNameCountry());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAirlineIcaoCode::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAirlineIcaoCode>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                this->setDesignator(variant.value<QString>());
                break;
            case IndexIataCode:
                this->setIataCode(variant.value<QString>());
                break;
            case IndexAirlineCountry:
                this->setCountry(variant.value<CCountry>());
                break;
            case IndexAirlineName:
                this->setName(variant.value<QString>());
                break;
            case IndexTelephonyDesignator:
                this->setTelephonyDesignator(variant.value<QString>());
                break;
            case IndexIsVirtualAirline:
                this->setVirtualAirline(variant.toBool());
                break;
            case IndexIsOperating:
                this->setOperating(variant.toBool());
                break;
            case IndexIsMilitary:
                this->setMilitary(variant.toBool());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CAirlineIcaoCode::comparePropertyByIndex(const CPropertyIndex &index, const CAirlineIcaoCode &compareValue) const
        {
            if (index.isMyself()) { return m_designator.compare(compareValue.getDesignator(), Qt::CaseInsensitive); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                return this->m_designator.compare(compareValue.getDesignator());
            case IndexIataCode:
                return this->m_iataCode.compare(compareValue.getIataCode());
            case IndexAirlineCountry:
                return this->m_country.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCountry());
            case IndexDesignatorNameCountry:
                return this->m_country.getName().compare(compareValue.getCountry().getName(), Qt::CaseInsensitive);
            case IndexAirlineName:
                return this->m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexTelephonyDesignator:
                return this->m_telephonyDesignator.compare(compareValue.getTelephonyDesignator(), Qt::CaseInsensitive);
            case IndexIsVirtualAirline:
                return Compare::compare(this->isVirtualAirline(), compareValue.isVirtualAirline());
            case IndexIsOperating:
                return Compare::compare(this->isOperating(), compareValue.isOperating());
            case IndexIsMilitary:
                return Compare::compare(this->isMilitary(), compareValue.isMilitary());
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No compare function");
            return 0;
        }

        CStatusMessageList CAirlineIcaoCode::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation() }));
            CStatusMessageList msgs;
            if (!hasValidDesignator()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Airline: missing designator")); }
            if (!hasValidCountry()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Airline: missing country")); }
            if (!hasName()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Airline: no name")); }
            return msgs;
        }

        bool CAirlineIcaoCode::isValidAirlineDesignator(const QString &airline)
        {
            if (airline.length() < 2 || airline.length() > 5) return false;

            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[A-Z0-9]+$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(airline).hasMatch());
        }

        QString CAirlineIcaoCode::getCombinedStringWithKey() const
        {
            QString s(getVDesignator());
            if (hasName()) { s = s.append(" ").append(getName()); }
            return s.append(" ").append(getDbKeyAsStringInParentheses());
        }

        QString CAirlineIcaoCode::getNameWithKey() const
        {
            if (!hasValidDbKey()) { return getName(); }
            if (hasName())
            {
                return QString(getName()).append(" ").append(getDbKeyAsStringInParentheses());
            }
            else
            {
                return getDbKeyAsStringInParentheses();
            }
        }

        void CAirlineIcaoCode::updateMissingParts(const CAirlineIcaoCode &otherIcaoCode)
        {
            if (!this->hasValidDbKey() && otherIcaoCode.hasValidDbKey())
            {
                // we have no DB data, but the other one has
                // so we change roles. We take the DB object as base, and update our parts
                CAirlineIcaoCode copy(otherIcaoCode);
                copy.updateMissingParts(*this);
                *this = copy;
                return;
            }

            if (!this->hasValidDesignator()) { this->setDesignator(otherIcaoCode.getDesignator()); }
            if (!this->hasValidCountry()) { this->setCountry(otherIcaoCode.getCountry()); }
            if (!this->hasName()) { this->setName(otherIcaoCode.getName()); }
            if (!this->hasTelephonyDesignator()) { this->setTelephonyDesignator(otherIcaoCode.getTelephonyDesignator()); }
            if (!this->hasValidDbKey())
            {
                this->setDbKey(otherIcaoCode.getDbKey());
                this->setUtcTimestamp(otherIcaoCode.getUtcTimestamp());
            }
        }

        CAirlineIcaoCode CAirlineIcaoCode::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            if (!existsKey(json, prefix))
            {
                // when using relationship, this can be null
                return CAirlineIcaoCode();
            }

            QString designator(json.value(prefix + "designator").toString());
            QString iata(json.value(prefix + "iata").toString());
            QString telephony(json.value(prefix + "callsign").toString());
            QString name(json.value(prefix + "name").toString());
            QString countryIso(json.value(prefix + "country").toString());
            QString countryName(json.value(prefix + "countryname").toString());
            bool va = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "va").toString());
            bool operating = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "operating").toString());
            bool military = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "military").toString());

            CAirlineIcaoCode code(
                designator, name,
                CCountry(countryIso, countryName),
                telephony, va, operating
            );
            code.setIataCode(iata);
            code.setMilitary(military);
            code.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return code;
        }
    } // namespace
} // namespace
