/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"

#include <QDir>
#include <QList>
#include <QPair>
#include <QSettings>
#include <QStringList>
#include <QVariant>

using namespace BlackConfig;
using namespace BlackMisc;

//
// all FSX/P3D keys: http://www.fsdeveloper.com/forum/threads/registry-keys-fsx-fsx-se-p3dv1-p3dv2.432633/
//

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            using FsRegistryPathPair = QList<QPair<QString, QString>>;

            QString fsxDirFromRegistryImpl()
            {
                QString fsxPath;
                if (CBuildConfig::isCompiledWithFsxSupport())
                {
                    FsRegistryPathPair fsxRegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft Games\\Flight Simulator - Steam Edition\\10.0"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\Flight Simulator\\10.0"), QStringLiteral("SetupPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft Games\\Flight simulator\\10.0"), QStringLiteral("SetupPath") }
                    };

                    for (const auto &registryPair : fsxRegistryPathPairs)
                    {
                        QSettings fsxRegistry(registryPair.first, QSettings::NativeFormat);
                        fsxPath = fsxRegistry.value(registryPair.second).toString().trimmed();

                        if (!fsxPath.isEmpty()) break;
                    }
                }
                return fsxPath;
            }

            const QString &CFsCommonUtil::fsxDirFromRegistry()
            {
                static const QString fsxPath(fsxDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxDirImpl()
            {
                QString dir(CFsCommonUtil::fsxDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                QStringList someDefaultDirs(
                {
                    "C:/Program Files (x86)/Microsoft Games/Microsoft Flight Simulator X",
                    "C:/FSX"
                });
                if (!CBuildConfig::isShippedVersion())
                {
                    someDefaultDirs.push_back("P:/FSX (MSI)");
                }
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsCommonUtil::fsxDir()
            {
                static const QString dir(fsxDirImpl());
                return dir;
            }

            QString fsxSimObjectsDirFromRegistryImpl()
            {
                const QString fsxPath = CFsCommonUtil::fsxDirFromRegistry();
                if (fsxPath.isEmpty()) { return ""; }
                return QDir(fsxPath).filePath("SimObjects");
            }

            const QString &CFsCommonUtil::fsxSimObjectsDirFromRegistry()
            {
                static const QString fsxPath(fsxSimObjectsDirFromRegistryImpl());
                return fsxPath;
            }

            QString fsxSimObjectsDirImpl()
            {
                QString dir(CFsCommonUtil::fsxDir());
                if (dir.isEmpty()) { return ""; }
                return QDir(dir).filePath("SimObjects");
            }

            const QString &CFsCommonUtil::fsxSimObjectsDir()
            {
                static const QString dir(fsxSimObjectsDirImpl());
                return dir;
            }

            const QStringList &CFsCommonUtil::fsxSimObjectsExcludeDirectoryPatterns()
            {
                static const QStringList exclude
                {
                    "SimObjects/Animals",
                    "SimObjects/Misc",
                    "SimObjects/GroundVehicles",
                    "SimObjects/Boats"
                };
                return exclude;
            }

            QString p3dDirFromRegistryImpl()
            {
                QString p3dPath;
                if (CBuildConfig::isCompiledWithP3DSupport())
                {
                    FsRegistryPathPair p3dRegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v3"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\Lockheed Martin\\Prepar3d v2"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_CURRENT_USER\\Software\\LockheedMartin\\Prepar3d"), QStringLiteral("AppPath") }
                    };
                    for (const auto &registryPair : p3dRegistryPathPairs)
                    {
                        QSettings p3dRegistry(registryPair.first, QSettings::NativeFormat);
                        p3dPath = p3dRegistry.value(registryPair.second).toString().trimmed();

                        if (!p3dPath.isEmpty()) break;
                    }
                }
                return p3dPath;
            }

            const QString &CFsCommonUtil::p3dDirFromRegistry()
            {
                static const QString p3dPath(p3dDirFromRegistryImpl());
                return p3dPath;
            }

            QString p3dDirImpl()
            {
                QString dir(CFsCommonUtil::p3dDirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                const QStringList someDefaultDirs(
                {
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D v3",
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D v2",
                    "C:/Program Files (x86)/Lockheed Martin/Prepar3D"
                });
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsCommonUtil::p3dDir()
            {
                static const QString dir(p3dDirImpl());
                return dir;
            }

            QString p3dSimObjectsDirFromRegistryImpl()
            {
                const QString p3dPath = CFsCommonUtil::p3dDirFromRegistry();
                if (p3dPath.isEmpty()) { return ""; }
                return QDir(p3dPath).filePath("SimObjects");
            }

            const QString &CFsCommonUtil::p3dSimObjectsDirFromRegistry()
            {
                static const QString p3dPath(p3dSimObjectsDirFromRegistryImpl());
                return p3dPath;
            }

            QString p3dSimObjectsDirImpl()
            {
                QString dir(CFsCommonUtil::p3dDir());
                if (dir.isEmpty()) { return ""; }
                return QDir(dir).filePath("SimObjects");
            }

            const QString &CFsCommonUtil::p3dSimObjectsDir()
            {
                static const QString dir(p3dSimObjectsDirImpl());
                return dir;
            }

            const QStringList &CFsCommonUtil::p3dSimObjectsExcludeDirectoryPatterns()
            {
                return fsxSimObjectsExcludeDirectoryPatterns();
            }

            QString fs9DirFromRegistryImpl()
            {
                QString fs9Path;
                if (CBuildConfig::isCompiledWithFs9Support())
                {
                    FsRegistryPathPair fs9RegistryPathPairs =
                    {
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DirectPlay\\Applications\\Microsoft Flight Simulator 2004"), QStringLiteral("AppPath") },
                        { QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\DirectPlay\\Applications\\Microsoft Flight Simulator 2004"), QStringLiteral("AppPath") }
                    };

                    for (const auto &registryPair : fs9RegistryPathPairs)
                    {
                        QSettings fs9Registry(registryPair.first, QSettings::NativeFormat);
                        fs9Path = fs9Registry.value(registryPair.second).toString().trimmed();

                        if (!fs9Path.isEmpty()) break;
                    }
                }
                return fs9Path;
            }

            const QString &CFsCommonUtil::fs9DirFromRegistry()
            {
                static const QString fs9Path(fs9DirFromRegistryImpl());
                return fs9Path;
            }

            QString fs9DirImpl()
            {
                QString dir(CFsCommonUtil::fs9DirFromRegistry());
                if (!dir.isEmpty()) { return dir; }
                const QStringList someDefaultDirs(
                {
                    "C:/Flight Simulator 9",
                    "C:/FS9"
                });
                return CFileUtils::findFirstExisting(someDefaultDirs);
            }

            const QString &CFsCommonUtil::fs9Dir()
            {
                static const QString v(fs9DirImpl());
                return v;
            }

            QString fs9AircraftDirFromRegistryImpl()
            {
                QString fs9Path = CFsCommonUtil::fs9DirFromRegistry();
                if (fs9Path.isEmpty()) { return ""; }
                fs9Path = QDir(fs9Path).filePath("Aircraft");
                return fs9Path;
            }

            const QString &CFsCommonUtil::fs9AircraftDirFromRegistry()
            {
                static const QString v(fs9AircraftDirFromRegistryImpl());
                return v;
            }

            QString fs9AircraftDirImpl()
            {
                const QString dir(CFsCommonUtil::fs9Dir());
                if (dir.isEmpty()) { return ""; }
                return QDir(dir).filePath("Aircraft");
            }

            const QString &CFsCommonUtil::fs9AircraftDir()
            {
                static const QString dir(fs9AircraftDirImpl());
                return dir;
            }

            const QStringList &CFsCommonUtil::fs9AircraftObjectsExcludeDirectoryPatterns()
            {
                static const QStringList exclude;
                return exclude;
            }
        } // namespace
    } // namespace
} // namespace
