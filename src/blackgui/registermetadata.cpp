/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/registermetadata.h"
#include "blackgui/settings/dockwidgetsettings.h"
#include "blackgui/settings/navigatorsettings.h"
#include "blackgui/components/registermetadatacomponents.h"

namespace BlackGui
{
    void registerMetadata()
    {
        BlackGui::Settings::CDockWidgetSettings::registerMetadata();
        BlackGui::Settings::CNavigatorSettings::registerMetadata();
        BlackGui::Components::registerMetadata();
    }
}
