/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEB_DATASERVICES_H
#define BLACKCORE_WEB_DATASERVICES_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/restricted.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/metarlist.h"

#include <QDateTime>
#include <QList>
#include <QSet>
#include <QObject>
#include <QString>
#include <QStringList>

namespace BlackMisc
{
    class CLogCategoryList;
    template <typename T> class Restricted;

    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    class CApplication;

    namespace Vatsim
    {
        class CVatsimBookingReader;
        class CVatsimDataFileReader;
        class CVatsimMetarReader;
        class CVatsimStatusFileReader;
    }

    namespace Db
    {
        class CAirportDataReader;
        class CDatabaseWriter;
        class CDatabaseReader;
        class CIcaoDataReader;
        class CModelDataReader;
        class CInfoDataReader;
    }

    /*!
     * Encapsulates reading data from web sources
     */
    class BLACKCORE_EXPORT CWebDataServices :
        public QObject
    {
        Q_OBJECT

    public:
        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Constructor, only allowed from BlackCore::CApplication
        CWebDataServices(CWebReaderFlags::WebReader readerFlags, const BlackCore::Db::CDatabaseReaderConfigList &dbReaderConfig, BlackMisc::Restricted<CApplication>, QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

        //! Read ATC bookings (used to re-read)
        void readAtcBookingsInBackground() const;

        //! Data file reader
        Vatsim::CVatsimDataFileReader *getVatsimDataFileReader() const { return m_vatsimDataFileReader; }

        //! Booking reader
        Vatsim::CVatsimBookingReader *getBookingReader() const { return m_vatsimBookingReader; }

        //! Metar reader
        Vatsim::CVatsimMetarReader *getMetarReader() const { return m_vatsimMetarReader; }

        //! Info data reader
        Db::CInfoDataReader *getInfoDataReader() const { return m_infoDataReader; }

        //! Currently used URL for shared DB data
        BlackMisc::Network::CUrl getDbReaderCurrentSharedDbDataUrl() const;

        //! DB writer class
        Db::CDatabaseWriter *getDatabaseWriter() const { return m_databaseWriter; }

        //! Reader flags
        CWebReaderFlags::WebReader getReaderFlags() const { return m_readers; }

        //! All DB entities for those readers used and not ignored
        BlackMisc::Network::CEntityFlags::Entity allDbEntitiesForUsedReaders() const;

        //! FSD servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVatsimFsdServers() const;

        //! Voice servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVatsimVoiceServers() const;

        //! METAR URLs (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimMetarUrls() const;

        //! Data file locations (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimDataFileUrls() const;

        //! Users by callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! ATC stations by callsign
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Voice capabilities for given callsign
        //! \threadsafe
        BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Update with web data
        //! \threadsafe
        void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const;

        //! Distributors
        //! \threadsafe
        BlackMisc::Simulation::CDistributorList getDistributors() const;

        //! Distributors count
        //! \threadsafe
        int getDistributorsCount() const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector() const;

        //! Best match specified by distributor / model
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor, const BlackMisc::Simulation::CAircraftModel &model) const;

        //! Liveries
        //! \threadsafe
        BlackMisc::Aviation::CLiveryList getLiveries() const;

        //! Liveries count
        //! \threadsafe
        int getLiveriesCount() const;

        //! Livery for its combined code
        //! \threadsafe
        BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

        //! The temp. livery if available
        //! \threadsafe
        BlackMisc::Aviation::CLivery getTempLiveryOrDefault() const;

        //! Standard livery for airline code
        //! \threadsafe
        BlackMisc::Aviation::CLivery getStdLiveryForAirlineCode(const BlackMisc::Aviation::CAirlineIcaoCode &icao) const;

        //! Livery for id
        //! \threadsafe
        BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const;

        //! Use a livery as template and select the best complete livery from DB for it
        //! \threadsafe
        BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const;

        //! Models
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModelList getModels() const;

        //! Models count
        //! \threadsafe
        int getModelsCount() const;

        //! Model keys
        //! \threadsafe
        QSet<int> getModelDbKeys() const;

        //! Model strings
        //! \threadsafe
        QStringList getModelStrings() const;

        //! Model completer string
        //! \threadsafe
        QStringList getModelCompleterStrings(bool sorted = true) const;

        //! Models for combined code and aircraft designator
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

        //! Model for key if any
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelKey) const;

        //! Aircraft ICAO codes
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Aircraft ICAO codes count
        //! \threadsafe
        int getAircraftIcaoCodesCount() const;

        //! ICAO code for designator
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int id) const;

        //! Use an ICAO object to select the best complete ICAO object from DB for it
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const;

        //! Airline ICAO codes
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Airline ICAO codes count
        //! \threadsafe
        int getAirlineIcaoCodesCount() const;

        //! ICAO codes for designator
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int id) const;

        //! Smart airline selector
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode smartAirlineIcaoSelector(const BlackMisc::Aviation::CAirlineIcaoCode &code, const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign()) const;

        //! ICAO code for callsign (e.g. DLH123 -> DLH)
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode findBestMatchByCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Countries
        //! \threadsafe
        BlackMisc::CCountryList getCountries() const;

        //! Country count
        //! \threadsafe
        int getCountriesCount() const;

        //! Country by ISO code (GB, US...)
        //! \threadsafe
        BlackMisc::CCountry getCountryForIsoCode(const QString &iso) const;

        //! Country by name (France, China ..)
        //! \threadsafe
        BlackMisc::CCountry getCountryForName(const QString &name) const;

        //! Get airports
        //! \\threadsafe
        BlackMisc::Aviation::CAirportList getAirports() const;

        //! Get airports count
        //! \\threadsafe
        int getAirportsCount() const;

        //! Get METARs
        //! \threadsafe
        BlackMisc::Weather::CMetarList getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

        //! Get METARs count
        //! \threadsafe
        int getMetarsCount() const;

        //! Validate for publishing
        //! \remark More detailed check than BlackMisc::Simulation::CAircraftModelList::validateForPublishing
        BlackMisc::CStatusMessageList validateForPublishing(const BlackMisc::Simulation::CAircraftModelList &models, BlackMisc::Simulation::CAircraftModelList &validModels, BlackMisc::Simulation::CAircraftModelList &invalidModels) const;

        //! Publish models to database
        BlackMisc::CStatusMessageList asyncPublishModels(const BlackMisc::Simulation::CAircraftModelList &models) const;

        //! Trigger read of info objects
        void triggerReadOfInfoObjects();

        //! Trigger read of new data
        //! \note requires info objects loaded upfront and uses the full cache logic
        BlackMisc::Network::CEntityFlags::Entity triggerRead(BlackMisc::Network::CEntityFlags::Entity whatToRead, const QDateTime &newerThan = QDateTime());

        //! Trigger reload from DB, only loads the DB data and bypasses the caches checks and info objects
        BlackMisc::Network::CEntityFlags::Entity triggerLoadingDirectlyFromDb(BlackMisc::Network::CEntityFlags::Entity whatToRead, const QDateTime &newerThan = QDateTime());

        //! Trigger reload from shared files, only loads the data and bypasses caches
        BlackMisc::Network::CEntityFlags::Entity triggerLoadingDirectlyFromSharedFiles(BlackMisc::Network::CEntityFlags::Entity whatToRead, bool checkCacheTsUpfront);

        //! Trigger loading of the HTTP headers for the shared files
        //! \note allows to obtain the timestamps
        bool triggerLoadingOfSharedFilesHeaders(BlackMisc::Network::CEntityFlags::Entity requestedEntities = BlackMisc::Network::CEntityFlags::AllDbEntities);

        //! Corresponding cache timestamp if applicable
        //! \threadsafe
        QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Corresponding DB timestamp if applicable
        //! \threadsafe
        QDateTime getDbLatestEntityTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Corresponding shared file timestamp
        QDateTime getSharedFileTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Request (updated) HTTP header for shared file of entity
        bool requestHeaderOfSharedFile(BlackMisc::Network::CEntityFlags::Entity entity);

        //! Are the shared headers for the given entities loaded
        bool areSharedHeadersLoaded(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllDbEntities) const;

        //! Those entities where the timestamp of header is newer than the cache timestamp
        BlackMisc::Network::CEntityFlags::Entity getEntitiesWithNewerHeaderTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const;

        //! Cache count for entity
        //! \threadsafe
        int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Count for entity from entity objects
        //! \threadsafe
        int getDbInfoCount(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Can connect to swift DB?
        bool canConnectSwiftDb() const;

        //! Reset the flags what was already signaled
        void resetSignalFlags();

        //! All DB data for an aircraft entity available?
        bool hasDbAircraftData() const;

        //! Admit all DB caches
        void admitDbCaches(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Write data to disk (mainly for testing scenarios)
        bool writeDbDataToDisk(const QString &dir) const;

        //! Load DB data from disk (mainly for testing scenarios)
        bool readDbDataFromDisk(const QString &dir, bool inBackground);

    signals:
        //! Combined read signal
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        //! Header of shared file read
        void sharedFileHeaderRead(BlackMisc::Network::CEntityFlags::Entity entity, const QString &fileName, bool success);

        // simplified signals
        // 1) simple signature
        // 2) fired direct after read, no need to wait for other entities

        //! \name Simplified read signals
        //! @{
        //! All swift DB data have been read
        void allSwiftDbDataRead();

        //! All headers received
        void allSwiftSharedAllHeadersReceived();

        //! All models read
        void swiftDbModelsRead();

        //! Aircraft ICAO data read
        void swiftDbAircraftIcaoRead();

        //! Airline ICAO data read
        void swiftDbAirlineIcaoRead();

        //! Airports read
        void swiftDbAirportsRead();

        //! All ICAO entities
        void swiftDbAllIcaoEntities();

        //! All entities needed for model matching
        void swiftDbModelMatchingEntities();
        //! @}

    public slots:
        //! Call CWebDataServices::readInBackground by single shot
        void readDeferredInBackground(BlackMisc::Network::CEntityFlags::Entity entities, int delayMs);

        //! First read (allows to immediately read in background)
        //! \remark ensures info objects (if and only if needed) are read upfront
        void readInBackground(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllEntities);

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Received METAR data
        void ps_receivedMetars(const BlackMisc::Weather::CMetarList &metars);

        //! Data file has been read
        void ps_vatsimDataFileRead(int lines);

        //! Read finished from reader
        void ps_readFromSwiftDb(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        //! A shared file header has been received
        void ps_sharedFileHeaderReceived(BlackMisc::Network::CEntityFlags::Entity entity, const QString &fileName, bool success);

    private:
        //! Init the readers
        void initReaders(CWebReaderFlags::WebReader flags, BlackMisc::Network::CEntityFlags::Entity entities);

        //! Init the info objects readers
        void initInfoObjectReaderAndTriggerRead();

        //! DB reader for given entity
        Db::CDatabaseReader *getDbReader(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Init the writers
        void initWriters();

        //! Remember this entity/those enties already have been signaled
        bool signalEntitiesRead(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Wait for info objects to be read
        bool waitForInfoObjects(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Wait for shared headers to be read
        bool waitForSharedHeaders(BlackMisc::Network::CEntityFlags::Entity entities);

        CWebReaderFlags::WebReader               m_readers = CWebReaderFlags::WebReaderFlag::None; //!< which readers are available
        BlackCore::Db::CDatabaseReaderConfigList m_dbReaderConfig;                                 //!< how to read DB data
        BlackMisc::Network::CEntityFlags::Entity m_entitiesPeriodicallyRead = BlackMisc::Network::CEntityFlags::NoEntity; //!< those entities which are permanently updated by timers
        BlackMisc::Network::CEntityFlags::Entity m_swiftDbEntitiesRead      = BlackMisc::Network::CEntityFlags::NoEntity; //!< entities read
        bool                                     m_initialRead = false;      //!< Initial read started
        bool                                     m_signalledHeaders = false; //!< headers loading has been signalled
        int                                      m_infoObjectTrials = 0;     //!< Tried to read info objects
        int                                      m_sharedHeadersTrials = 0;  //!< Tried to read shared file headers
        QSet<BlackMisc::Network::CEntityFlags::Entity> m_signalledEntities;  //!< remember signalled entites

        // for reading XML and VATSIM data files
        Vatsim::CVatsimStatusFileReader *m_vatsimStatusReader   = nullptr;
        Vatsim::CVatsimBookingReader    *m_vatsimBookingReader  = nullptr;
        Vatsim::CVatsimDataFileReader   *m_vatsimDataFileReader = nullptr;
        Vatsim::CVatsimMetarReader      *m_vatsimMetarReader    = nullptr;
        Db::CIcaoDataReader             *m_icaoDataReader       = nullptr;
        Db::CModelDataReader            *m_modelDataReader      = nullptr;
        Db::CInfoDataReader             *m_infoDataReader       = nullptr;
        Db::CAirportDataReader          *m_airportDataReader    = nullptr;

        // writing objects directly into DB
        Db::CDatabaseWriter *m_databaseWriter = nullptr;
    };
} // namespace

#endif
