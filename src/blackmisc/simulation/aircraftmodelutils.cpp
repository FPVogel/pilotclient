/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/verify.h"

namespace BlackMisc
{
    namespace Simulation
    {
        bool CAircraftModelUtilities::mergeWithVPilotData(CAircraftModelList &modelToBeModified, const CAircraftModelList &vPilotModels, bool force)
        {
            if (vPilotModels.isEmpty() || modelToBeModified.isEmpty()) { return false; }
            for (CAircraftModel &simModel : modelToBeModified)
            {
                if (!force && simModel.hasValidAircraftAndAirlineDesignator()) { continue; }  // already done
                CAircraftModel vPilotModel(vPilotModels.findFirstByModelStringOrDefault(simModel.getModelString()));
                if (!vPilotModel.hasValidDbKey())
                {
                    continue; // not found
                }
                simModel.updateMissingParts(vPilotModel, false);
            }
            return true;
        }

        QString CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrix(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return ""; }
            static const QString emptyDesignator = "----";
            static const QString colorLiveryDesignator = "-C-";

            QMap<QString, QMap<QString, CAircraftModelList>> modelsByDesignator;

            // create an empty map of all airlines
            const QMap<QString, CAircraftModelList> emptyAirlineDesignatorMap;
            CAircraftModelList sortedByAircraft(models);
            sortedByAircraft.sortBy(&CAircraftModel::getAircraftIcaoCodeDesignator);

            for (const CAircraftModel &model : as_const(sortedByAircraft))
            {
                const QString aircraftIcao(model.hasAircraftDesignator() ? model.getAircraftIcaoCodeDesignator() : emptyDesignator);
                if (!modelsByDesignator.contains(aircraftIcao))
                {
                    modelsByDesignator.insert(aircraftIcao, emptyAirlineDesignatorMap);
                }
                QMap<QString, CAircraftModelList> &airlineModels = modelsByDesignator[aircraftIcao];
                const QString airlineIcao(
                    model.getLivery().isColorLivery() ? colorLiveryDesignator :
                    model.hasAirlineDesignator() ? model.getAirlineIcaoCodeDesignator() : emptyDesignator);
                if (airlineModels.contains(airlineIcao))
                {
                    airlineModels[airlineIcao].push_back(model);
                }
                else
                {
                    airlineModels.insert(airlineIcao, CAircraftModelList({model}));
                }
            }

            // to HTML
            QString html("<table>\n");
            QStringList airlineIcaos = models.getAirlineVDesignators().toList();
            qSort(airlineIcaos);
            airlineIcaos.push_front(colorLiveryDesignator);
            airlineIcaos.push_back(emptyDesignator);
            QStringList aircraftIcaos = modelsByDesignator.keys();
            qSort(aircraftIcaos);

            // header
            html += "<thead><tr>\n";
            html += "<th></th>";
            for (const QString &airline : as_const(airlineIcaos))
            {
                html += "<th>";
                html += airline;
                html += "</th>";
            }
            html += "\n</tr></thead>\n";

            // fill data
            html += "<tbody>\n";
            for (const QString &aircraftIcao : as_const(aircraftIcaos))
            {
                html += "<tr>\n";
                html += "  <th>";
                html += aircraftIcao;
                html += "</th>\n";

                const QMap<QString, CAircraftModelList> &airlineModels = modelsByDesignator[aircraftIcao];
                for (const QString &airline : as_const(airlineIcaos))
                {
                    if (airlineModels.contains(airline))
                    {
                        html += "  <td>";
                        const CAircraftModelList &models(airlineModels[airline]);
                        html += "<a>";
                        html += QString::number(models.size());
                        html += "</a><div class=\"mouseoverdisplay\">";
                        html += models.asHtmlSummary();
                        html += "</div>";
                        html += "</td>\n";
                    }
                    else
                    {
                        html += "  <td></td>\n";
                    }
                }
                html += "</tr>\n";
            }
            html += "</tbody>\n";
            html += "</table>\n";
            return html;
        }

        QString CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrixFile(const CAircraftModelList &models, const QString &tempDir)
        {
            Q_ASSERT_X(!tempDir.isEmpty(), Q_FUNC_INFO, "Need directory");
            if (models.isEmpty()) { return ""; }
            const QString html = createIcaoAirlineAircraftHtmlMatrix(models);
            if (html.isEmpty()) { return ""; }

            QDir dir(tempDir);
            BLACK_VERIFY_X(dir.exists(), Q_FUNC_INFO, "Directory does not exist");
            if (!dir.exists()) { return ""; }

            const QString htmlTemplate = CFileUtils::readFileToString(CDirectoryUtils::getHtmlTemplateFileName());
            const QString fn("airlineAircraftMatrix.html");
            const bool ok = CFileUtils::writeStringToFile(htmlTemplate.arg(html), dir.absoluteFilePath(fn));
            return ok ? dir.absoluteFilePath(fn) : "";
        }
    } // ns
} // ns
