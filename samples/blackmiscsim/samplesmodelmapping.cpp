/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesmodelmapping.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/sampleutils.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/fscommon/modelmappingsprovidervpilot.h"
#include "blackmisc/simulation/aircraftmatcher.h"

#include <QDebug>
#include <memory>
#include <utility>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Aviation;

namespace BlackSample
{
    void CSamplesModelMapping::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        BlackMisc::registerMetadata();

        std::unique_ptr<IModelMappingsProvider> cvm(new CModelMappingsProviderVPilot(true));
        bool s = cvm->read();
        streamOut << "directory: " << CVPilotRulesReader::standardMappingsDirectory() << endl;
        streamOut << "loaded: " << BlackMisc::boolToYesNo(s) << " size: " << cvm->getMatchingModels().size() << endl;

        // mapper with rule set, handing over ownership
        QString fsxDir = CSampleUtils::selectDirectory(
        {
            QStringLiteral("P:/FlightSimulatorX (MSI)/SimObjects"),
            QStringLiteral("P:/Temp/SimObjects"),
            QStringLiteral("C:/Flight Simulator 9/Aircraft")
        }, streamOut, streamIn);

        CAircraftCfgParser cfgParser(CSimulatorInfo(CSimulatorInfo::FSX), fsxDir);
        if (!cfgParser.changeRootDirectory(fsxDir))
        {
            streamOut << "Wrong or empty directoy " << fsxDir << endl;
            return;
        }

        streamOut << "Start reading models" << endl;
        cfgParser.startLoading(CAircraftCfgParser::CacheSkipped | CAircraftCfgParser::LoadDirectly);
        streamOut << "Read models: " << cfgParser.getAircraftCfgEntriesList().size() << endl;
        streamOut << "Ambigious models: " << cfgParser.getAircraftCfgEntriesList().detectAmbiguousTitles().join(", ") << endl;

        // sync definitions, remove redundant ones
        CAircraftMatcher matcher(CAircraftMatcher::AllModes);
        matcher.setModelMappingProvider(std::move(cvm));

        CAircraftIcaoCode icao("C172");
        streamOut << "Searching for " << icao << endl;
        streamOut << matcher.getMatchingModels().findByIcaoDesignators(icao, CAirlineIcaoCode()) << endl;
    }
} // namespace
