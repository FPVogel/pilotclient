/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avaltitude.h"

using BlackMisc::PhysicalQuantities::CLength;
using BlackMisc::PhysicalQuantities::CLengthUnit;

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Own implementation for streaming
         */
        QString CAltitude::convertToQString(bool /* i18n */) const
        {
            QString s = this->CLength::convertToQString();
            return s.append(this->isMeanSeaLevel() ? " MSL" : " AGL");
        }

        /*
         * Marshall to DBus
         */
        void CAltitude::marshallToDbus(QDBusArgument &argument) const
        {
            this->CLength::marshallToDbus(argument);
            argument << qint32(this->m_datum);
        }

        /*
         * Unmarshall from DBus
         */
        void CAltitude::unmarshallFromDbus(const QDBusArgument &argument)
        {
            this->CLength::unmarshallFromDbus(argument);
            qint32 datum;
            argument >> datum;
            this->m_datum = static_cast<ReferenceDatum>(datum);
        }

        /*
         * Equal?
         */
        bool CAltitude::operator ==(const CAltitude &other) const
        {
            return other.m_datum == this->m_datum && this->CLength::operator ==(other);
        }

        /*
         * Unequal?
         */
        bool CAltitude::operator !=(const CAltitude &other) const
        {
            return !((*this) == other);
        }

        /*
         * metaTypeId
         */
        int CAltitude::getMetaTypeId() const
        {
            return qMetaTypeId<CAltitude>();
        }

        /*
         * is a
         */
        bool CAltitude::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAltitude>()) { return true; }

            return this->CLength::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CAltitude::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAltitude &>(otherBase);

            if (this->isMeanSeaLevel() && other.isAboveGroundLevel()) { return 1; }
            if (this->isAboveGroundLevel() && other.isMeanSeaLevel()) { return -1; }
            if (*this < other) { return -1; }
            if (*this > other) { return 1; }
            return 0;
        }

        /*
         * Register metadata
         */
        void CAltitude::registerMetadata()
        {
            qRegisterMetaType<CAltitude>();
            qDBusRegisterMetaType<CAltitude>();
        }

        /*
         * To JSON
         */
        QJsonObject CAltitude::toJson() const
        {
            QJsonObject json = BlackMisc::serializeJson(CAltitude::jsonMembers(), TupleConverter<CAltitude>::toTuple(*this));
            return BlackMisc::Json::appendJsonObject(json, CLength::toJson());
        }

        /*
         * To JSON
         */
        void CAltitude::fromJson(const QJsonObject &json)
        {
            CLength::fromJson(json);
            BlackMisc::deserializeJson(json, CAltitude::jsonMembers(), TupleConverter<CAltitude>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CAltitude::jsonMembers()
        {
            return TupleConverter<CAltitude>::jsonMembers();
        }

    } // namespace
} // namespace
