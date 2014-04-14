#include "servicetool.h"
#include "testservice.h"
#include "testservice_interface.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/indexvariantmap.h"
#include "blackmisc/nwserver.h"
#include <QTextStream>
#include <QString>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Math;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMiscTest;

namespace BlackMiscTest
{

    /*
     * Start a new process
     */
    QProcess *ServiceTool::startNewProcess(const QString &executable, const QStringList &arguments, QObject *parent)
    {
        QProcess *process = new QProcess(parent);
        process->startDetached(executable, arguments);
        return process;
    }

    /*
     * Display DBus arguments
     */
    void ServiceTool::displayQDBusArgument(const QDBusArgument &arg, qint32 level)
    {
        arg.beginArray();
        QVariant qv;
        QString indent(level * 2, ' ');

        while (!arg.atEnd())
        {
            QDBusArgument::ElementType type = arg.currentType();
            QString signature = arg.currentSignature();
            qv = arg.asVariant(); // this advances in the stream
            if (qv.canConvert<QDBusArgument>())
            {
                qDebug() << indent << type << "signature" << signature;
                ServiceTool::displayQDBusArgument(qv.value<QDBusArgument>(), level + 1);
            }
            else
            {
                qDebug() << indent << "type:" << type << "signature" << signature << "value" << qv;
            }
        }
        arg.endArray();
    }

    /*
     * Server side on testserver
     */
    void ServiceTool::dataTransferTestServer(BlackCore::CDBusServer *dBusServer)
    {
        QDBusConnection sessionBusConnection = QDBusConnection::sessionBus();
        if (sessionBusConnection.interface()->isServiceRegistered(Testservice::ServiceName))
        {
            qFatal("Testservice already registed on session bus");
        }

        // as this is the receiver side, the slots can be debugged too
        Testservice *testservice = ServiceTool::registerTestservice(sessionBusConnection, QCoreApplication::instance());
        dBusServer->addObject(Testservice::ServicePath, testservice);
    }

    /*
     * Client side on testserver
     */
    void ServiceTool::dataTransferTestClient(const QString &address)
    {
        // send data via session
        QDBusConnection sessionBusConnection = QDBusConnection::sessionBus();
        qDebug() << "------------ sending via session bus" << sessionBusConnection.name();
        ServiceTool::sendDataToTestservice(sessionBusConnection);

        // send data as P2P to server (this can be session bus, too, but usually is P2P)
        QDBusConnection p2pConnection = address == "session" ?
                                        QDBusConnection::sessionBus() :
                                        QDBusConnection::connectToPeer(address, "p2pConnection");
        qDebug() << "server connection has interface?" << p2pConnection.interface(); // returns 0 with server and a real interface with session bus
        qDebug() << "------------ sending via P2P server " << p2pConnection.name();
        ServiceTool::sendDataToTestservice(p2pConnection);
    }

    /*
     * Get callsign
     */
    CCallsign ServiceTool::getRandomCallsign()
    {
        static QList<CCallsign> callsigns;
        if (callsigns.isEmpty())
        {
            callsigns << CCallsign("EDDM_TWR");
            callsigns << CCallsign("EDDM_APP");
            callsigns << CCallsign("EDDM_GND");
            callsigns << CCallsign("EDDF_TWR");
            callsigns << CCallsign("EDDF_APP");
            callsigns << CCallsign("EDDF_GND");
        }
        int i = (rand() % (6));
        CCallsign cs = callsigns.at(i);
        return cs;
    }

    /*
     * Stations
     */
    CAtcStationList ServiceTool::getStations(qint32 number)
    {
        BlackMisc::Aviation::CAtcStationList list;
        for (int i = 0; i < number; i++)
        {
            BlackMisc::Aviation::CAtcStation s;
            s.setCallsign(QString::number(i));
            s.setFrequency(BlackMisc::PhysicalQuantities::CFrequency(i, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            s.setController(CUser(QString::number(i), "FooBar", "", ""));
            s.setOnline(i % 2 == 0 ? true : false);
            s.setPosition(CCoordinateGeodetic(i, i, i));
            list.push_back(s);
        }
        return list;

    }

    /*
     * Register testservice (server side)
     */
    Testservice *ServiceTool::registerTestservice(QDBusConnection &connection, QObject *parent)
    {
        Testservice *pTestservice = new Testservice(parent);  // just a QObject with signals / slots and  Q_CLASSINFO("D-Bus Interface", some service name)
        if (!connection.registerService(Testservice::ServiceName))
        {
            QDBusError err = connection.lastError();
            qWarning() << err.message();
            qWarning() << "Started dbus-daemon.exe --session (Windows)?";
            qWarning() << "Created directory session.d (e.g. ../Qt/Qt5.1.0Vatsim/5.1.0-32/qtbase/etc/dbus-1/session.d)?";
            qWarning() << "See https://dev.vatsim-germany.org/projects/vatpilotclient/wiki/DBusExample#Running-the-example";
            qFatal("Could not register service!");
        }

        if (!connection.registerObject(Testservice::ServicePath, pTestservice, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAdaptors))
        {
            qFatal("Could not register service object!");
        }

        qDebug() << "Registration running as pid:" << ServiceTool::getPid();
        if (pTestservice) qDebug() << "Service registered";

        QString service; // service not needed
        if (connection.connect(
                    service, Testservice::ServicePath, Testservice::ServiceName,
                    "sendStringMessage", pTestservice, SLOT(receiveStringMessage(const QString &))))
        {
            qDebug() << "Connected object with bus sendStringMessage";
        }
        else
        {
            qFatal("Cannot connect service with DBus");
        }
        return pTestservice;
    }

    /*
     * Send data to testservice, this sends data to the slots on the server
     */
    void ServiceTool::sendDataToTestservice(const QDBusConnection &connection)
    {
        // on the client's side
        TestServiceInterface testserviceInterface(Testservice::ServiceName, Testservice::ServicePath, connection);

        CSpeed speed(200, BlackMisc::PhysicalQuantities::CSpeedUnit::km_h());
        CAltitude al(1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        bool loop = true;

        while (loop)
        {
            QDBusMessage m = QDBusMessage::createSignal(
                                 Testservice::ServicePath, Testservice::ServiceName,
                                 "sendStringMessage");

            // The << operator is used to add the parameters for the slot
            QDateTime dtnow = QDateTime::currentDateTimeUtc();
            QString msg = QString("Message from %1 at %2").arg(ServiceTool::getPid()).arg(dtnow.toString("MM/dd/yyyy @ hh:mm:ss"));
            m << msg;

            // We send this as a non-replying message. This is used for sending errors, replys, signals,
            // and method calls (slots) that don't return
            if (connection.send(m))
            {
                qDebug() << "Send via low level method" << m;
            }

            // same as interface message
            // but call the slot
            testserviceInterface.receiveStringMessage(msg);
            qDebug() << "Send string via interface" << msg;

            // a list
            QList<double> list;
            list << 1.0 << 2.0 << 3.0;
            testserviceInterface.receiveList(list);
            qDebug() << "Send list via interface" << list;

            // PQs
            testserviceInterface.receiveSpeed(speed);
            qDebug() << "Send speed via interface" << speed;

            speed.switchUnit(CSpeedUnit::kts());
            testserviceInterface.receiveSpeed(speed);
            qDebug() << "Send speed via interface" << speed;
            QThread::msleep(2500);
            speed.switchUnit(CSpeedUnit::km_h());
            speed.addValueSameUnit(1.0);

            // Network
            CServer trafficServer("fooserver", "a foo server", "localhost", 1234,
                                  CUser("112233", "Some real name", "email@xyz.com", "secret"));
            QVariant tsqv = QVariant::fromValue(trafficServer);
            QDBusVariant tsv(tsqv);
            testserviceInterface.receiveVariant(tsv, tsqv.userType());
            qDebug() << "Send server via interface and variant" << trafficServer <<  tsqv.userType();

            // Aviation
            CComSystem comSystem = CComSystem("DBUS COM1", CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            testserviceInterface.receiveComUnit(comSystem);
            qDebug() << "Send COM via interface" << comSystem;

            QVariant qvAl = QVariant::fromValue(al);
            QDBusVariant qv(qvAl);
            testserviceInterface.receiveVariant(qv, qvAl.userType());
            testserviceInterface.receiveAltitude(al);
            qDebug() << "Send altitude via interface and variant" << al << qvAl.userType();
            al.addValueSameUnit(1);

            CTransponder transponder("transponder", 7000, CTransponder::ModeC);
            testserviceInterface.receiveTransponder(transponder);
            qDebug() << "Send transponder via interface" << transponder;

            CTrack track(123.45, CTrack::Magnetic, CAngleUnit::deg());
            testserviceInterface.receiveTrack(track);
            qDebug() << "Send track via interface" << track;

            CLength len(33, CLengthUnit::m());
            testserviceInterface.receiveLength(len);
            qDebug() << "Send length via interface" << len;

            CAltitude alt(44, CAltitude::MeanSeaLevel, CLengthUnit::m());
            testserviceInterface.receiveLength(alt);
            qDebug() << "Send altitude via interface" << alt;

            CCallsign callsign("d-ambz");
            testserviceInterface.receiveCallsign(callsign);
            qDebug() << "Send callsign via interface" << callsign;

            CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(1487, CLengthUnit::ft())); // Munich
            CAtcStation station(CCallsign("eddm_twr"), CUser("123456", "Joe Controller"),
                                CFrequency(118.7, CFrequencyUnit::MHz()),
                                geoPos, CLength(50, CLengthUnit::km()));

            testserviceInterface.receiveAtcStation(station);

            CAtcStation stationReceived  = testserviceInterface.pingAtcStation(station);
            qDebug() << "Pinged ATC station via interface"
                     << ((station == stationReceived) ? "OK" : "ERROR!") << stationReceived;

            CAircraftSituation situation;
            CAircraft aircraft(callsign, CUser("123456", "Joe Pilot"), situation);
            CAircraft aircraftReceived = testserviceInterface.pingAircraft(aircraft);
            qDebug() << "Pinged aircraft via interface"
                     << ((aircraft == aircraftReceived) ? "OK" : "ERROR!") << aircraftReceived;

            CAtcStationList AtcStationList;
            AtcStationList.push_back(station);
            AtcStationList.push_back(station);
            AtcStationList.push_back(station);
            AtcStationList = testserviceInterface.pingAtcStationList(AtcStationList);
            qDebug() << "Pinged ATC stations list via interface" << AtcStationList.size() << AtcStationList;

            AtcStationList = ServiceTool::getStations(10);
            qDebug() << "Pinged ATC stations list via interface" << AtcStationList.size() << AtcStationList;

            // test variant lists with different types wrapped in QVariant
            QVariantList qvList;
            qvList << QVariant::fromValue(len);
            qvList << QVariant::fromValue(alt);
            QList<QVariant> lengthsV;
            lengthsV.append(QVariant::fromValue(len));
            lengthsV.append(QVariant::fromValue(alt));
            testserviceInterface.receiveLengthsQvl(qvList);
            qDebug() << "Send lengths via interface as QListVariant:";
            testserviceInterface.receiveLengthsQl(lengthsV);
            qDebug() << "Send lengths via interface as QList<QVariant>:";
            foreach(QVariant lv, qvList)
            {
                qDebug() << "   " << "Send length in list:" << BlackMisc::qVariantToString(lv);
            }
            QThread::msleep(2500);

            // Math
            CMatrix3x3 m33;
            m33.setCellIndex();
            testserviceInterface.receiveMatrix(m33);
            qDebug() << "Send matrix" << m33;

            // Geo
            // EDDF: 50° 2′ 0″ N, 8° 34′ 14″ E, 100m MSL
            geoPos = CCoordinateGeodetic::fromWgs84("50° 2′ 1″ 23 N", "8° 34′ 14″ E", CLength(111, CLengthUnit::m()));
            testserviceInterface.receiveGeoPosition(geoPos);
            qDebug() << "Send geo position" << geoPos;

            // Value map
            CIndexVariantMap valueMap;
            valueMap.addValue(1, 111.222);
            valueMap.addValue(2, callsign);
            valueMap.addValue(3, alt);
            valueMap.addValue(4, track);
            valueMap.addValue(5, QDateTime::currentDateTime().addDays(1));
            valueMap.addValue(6, QString("foobar"));
            testserviceInterface.receiveValueMap(valueMap);
            qDebug() << "Send value map" << valueMap;

            // Performance tools
            QThread::msleep(2500);
            qDebug() << "--- PERFORMANCE testing";
            QElapsedTimer timer;
            timer.start();
            for (int i = 0; i < 10; i++)
            {
                CSpeed speedDummy(i, CSpeedUnit::km_h());
                speedDummy = testserviceInterface.getSpeed();
            }
            qint64 t10 = timer.nsecsElapsed() / 1000000; // ms
            timer.restart();
            for (int i = 0; i < 100; i++)
            {
                CSpeed speedDummy(i, CSpeedUnit::km_h());
                speedDummy = testserviceInterface.getSpeed();
            }
            qint64 t100 = timer.nsecsElapsed() / 1000000; // ms
            timer.restart();
            for (int i = 0; i < 1000; i++)
            {
                CSpeed speedDummy(i, CSpeedUnit::km_h());
                speedDummy = testserviceInterface.getSpeed();
            }
            qint64 t1000 = timer.nsecsElapsed() / 1000000; // ms
            timer.invalidate();
            qDebug() << "Reading speed objects 10/100/1000 in ms:" << t10 << t100 << t1000;

            timer.start();
            for (int i = 0; i < 10; i++)
            {
                CAtcStation stationDummy(QString::number(i));
                stationDummy = testserviceInterface.getAtcStation();
            }
            t10 = timer.nsecsElapsed() / 1000000; // ms
            timer.restart();
            for (int i = 0; i < 100; i++)
            {
                CAtcStation stationDummy(QString::number(i));
                stationDummy = testserviceInterface.getAtcStation();
            }
            t100 = timer.nsecsElapsed() / 1000000; // ms
            timer.restart();
            for (int i = 0; i < 1000; i++)
            {
                CAtcStation stationDummy(QString::number(i));
                stationDummy = testserviceInterface.getAtcStation();
            }
            t1000 = timer.nsecsElapsed() / 1000000; // ms
            qDebug() << "Reading station objects 10/100/1000 in ms:" << t10 << t100 << t1000;

            timer.restart();
            AtcStationList = testserviceInterface.getAtcStationList(10);
            if (AtcStationList.size() != 10) qDebug() << "wrong list size" << AtcStationList.size();
            t10 = timer.nsecsElapsed() / 1000000; // ms
            QThread::msleep(1000);

            timer.restart();
            AtcStationList = testserviceInterface.getAtcStationList(100);
            if (AtcStationList.size() != 100) qDebug() << "wrong list size" << AtcStationList.size();
            t100 = timer.nsecsElapsed() / 1000000; // ms
            QThread::msleep(1000);

            timer.restart();
            AtcStationList = testserviceInterface.getAtcStationList(1000);
            if (AtcStationList.size() != 1000) qDebug() << "wrong list size" << AtcStationList.size();
            t1000 = timer.nsecsElapsed() / 1000000; // ms

            qDebug() << "Reading station list 10/100/1000 in ms:" << t10 << t100 << t1000;
            timer.invalidate();

            timer.restart();
            QList<QDBusObjectPath> objectPaths = testserviceInterface.getObjectPaths(10);
            if (objectPaths.size() != 10) qDebug() << "wrong list size" << objectPaths.size();
            t10 = timer.nsecsElapsed() / 1000000; // ms

            timer.restart();
            objectPaths = testserviceInterface.getObjectPaths(100);
            if (objectPaths.size() != 100) qDebug() << "wrong list size" << objectPaths.size();
            t100 = timer.nsecsElapsed() / 1000000; // ms

            timer.restart();
            objectPaths = testserviceInterface.getObjectPaths(1000);
            if (objectPaths.size() != 1000) qDebug() << "wrong list size" << objectPaths.size();
            t1000 = timer.nsecsElapsed() / 1000000; // ms

            qDebug() << "Reading paths list 10/100/1000 in ms:" << t10 << t100 << t1000;
            timer.invalidate();

            // next round?
            qDebug() << "Key  ....... x to exit, pid:" << ServiceTool::getPid();
            int ch = getchar();
            if (ch == 'x') loop = false;
        }
    }
} // namespace
