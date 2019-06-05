/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DATABASEOBJECTLIST_H
#define BLACKMISC_DB_DATABASEOBJECTLIST_H

#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/jsonexception.h"
#include <QJsonArray>
#include <QSet>
#include <QMap>
#include <QString>

namespace BlackMisc
{
    namespace Db
    {
        //! List of objects read from database.
        //! Such objects should implement \sa ITimestampBased and \sa IDatastoreObjectWithIntegerKey or \sa IDatastoreObjectWithStringKey
        template<class OBJ, class CONTAINER, typename KEYTYPE> class IDatastoreObjectList : public ITimestampObjectList<OBJ, CONTAINER>
        {
        public:
            //! Object with key, notFound otherwise
            OBJ findByKey(KEYTYPE key, const OBJ &notFound = OBJ()) const;

            //! Object with key, notFound otherwise
            CONTAINER findByKeys(const QSet<KEYTYPE> &keys) const;

            //! Objects with DB key
            CONTAINER findObjectsWithDbKey() const;

            //! Objects without DB key
            CONTAINER findObjectsWithoutDbKey() const;

            //! Object with max.key
            OBJ maxKeyObject() const;

            //! Sort by timestamp
            void sortByKey();

            //! All keys as set
            QSet<KEYTYPE> toDbKeySet() const;

            //! As map with DB key/object
            QMap<KEYTYPE, OBJ> toDbKeyValueMap() const;

            //! All keys as string set (also integer keys will be converted to string)
            QSet<QString> toDbKeyStringSet() const;

            //! The DB keys as string
            QString dbKeysAsString(const QString &separator) const;

            //! Max.key value (making sense with integer key)
            KEYTYPE getMaxKey(bool *ok = nullptr) const;

            //! Remove objects with keys
            int removeObjectsWithKeys(const QSet<KEYTYPE> &keys);

            //! Remove objects without key
            int removeObjectsWithoutDbKey();

            //! Update or insert data (based on DB key)
            int replaceOrAddObjectsByKey(const CONTAINER &container);

            //! Latest DB timestamp (means objects with DB key)
            QDateTime latestDbTimestamp() const;

            //! Latest DB timestamp (means objects with DB key)
            QDateTime oldestDbTimestamp() const;

            //! Number of objects with/without key
            int countWithValidDbKey(bool withKey) const;

            //! Number of entries with valid DB key
            int countWithValidDbKey() const;

            //! Any object without key?
            bool containsAnyObjectWithoutKey() const;

            //! Contains object with key?
            bool containsDbKey(KEYTYPE key) const;

            //! From multiple JSON formats
            //! \remark supports native swift C++ format, DB format, and cache format
            static CONTAINER fromMultipleJsonFormats(const QJsonObject &jsonObject);

            //! From multiple JSON formats
            //! \remark supports native swift C++ format, DB format, and cache format
            static CONTAINER fromMultipleJsonFormats(const QString &jsonString);

            //! From DB JSON with default prefixes
            //! \remark Specialized classes might have their own fromDatabaseJson implementation
            static CONTAINER fromDatabaseJson(const QJsonArray &array);

            //! From DB JSON
            //! \remark Specialized classes might have their own fromDatabaseJson implementation
            static CONTAINER fromDatabaseJson(const QJsonArray &array, const QString &prefix);

        protected:
            //! Constructor
            IDatastoreObjectList();
        };

        //! \cond PRIVATE
        class CDbInfo;
        class CDbInfoList;

        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAircraftCategory, BlackMisc::Aviation::CAircraftCategoryList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Db::CDbInfo, BlackMisc::Db::CDbInfoList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Db::CArtifact, BlackMisc::Db::CArtifactList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Db::CDistribution, BlackMisc::Db::CDistributionList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, QString>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IDatastoreObjectList<BlackMisc::CCountry, BlackMisc::CCountryList, QString>;
        //! \endcond
    } // ns
} //ns

#endif //guard
