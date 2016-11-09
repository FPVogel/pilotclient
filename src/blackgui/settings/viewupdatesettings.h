/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_VIEWUPDATESETTINGS_H
#define BLACKGUI_SETTINGS_VIEWUPDATESETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

#include <QMap>
#include <QString>
#include <QMetaType>

namespace BlackGui
{
    namespace Settings
    {
        //! Settings about view update rates
        class BLACKGUI_EXPORT CViewUpdateSettings :
            public BlackMisc::CValueObject<CViewUpdateSettings>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAtc = BlackMisc::CPropertyIndex::GlobalIndexCViewUpdateSettings,
                IndexAircraft,
                IndexRendering,
                IndexUser
            };

            //! Default constructor
            CViewUpdateSettings();

            //! Reset to defaults
            void reset();

            //! Get time
            const BlackMisc::PhysicalQuantities::CTime &getAtcUpdateTime() const { return m_updateAtc; }

            //! Set time
            void setAtcUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateAtc = time; }

            //! Get time
            const BlackMisc::PhysicalQuantities::CTime &getAircraftUpdateTime() const { return m_updateAircraft; }

            //! Set time
            void setAircraftUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateAircraft = time; }

            //! Get time
            const BlackMisc::PhysicalQuantities::CTime &getUserUpdateTime() const { return m_updateUser; }

            //! Set time
            void setUserUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateUser = time; }

            //! Get time
            const BlackMisc::PhysicalQuantities::CTime &getRenderingUpdateTime() const { return m_updateRendering; }

            //! Set time
            void setRenderingUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateRendering = time; }

            //! Valid?
            bool isValid() const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            BlackMisc::PhysicalQuantities::CTime m_updateAtc;
            BlackMisc::PhysicalQuantities::CTime m_updateAircraft;
            BlackMisc::PhysicalQuantities::CTime m_updateRendering;
            BlackMisc::PhysicalQuantities::CTime m_updateUser;

            BLACK_METACLASS(
                CViewUpdateSettings,
                BLACK_METAMEMBER(updateAtc),
                BLACK_METAMEMBER(updateAircraft),
                BLACK_METAMEMBER(updateRendering),
                BLACK_METAMEMBER(updateUser)
            );
        };

        //! Trait for settings about update rates
        struct TViewUpdateSettings : public BlackMisc::TSettingTrait<CViewUpdateSettings>
        {
            //! Key in data cache
            static const char *key() { return "guiviewupdatesettings"; }

            //! Validator function.
            static bool isValid(const CViewUpdateSettings &settings) { return settings.isValid(); }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CViewUpdateSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CViewUpdateSettings>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackGui::Settings::CViewUpdateSettings>)

#endif // guard
