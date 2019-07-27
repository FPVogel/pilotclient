/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_XSWIFTBUSSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_XSWIFTBUSSETTINGS_H

#include "xswiftbussettingsqtfree.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            //! XSwiftBus settings
            class BLACKMISC_EXPORT CXSwiftBusSettings :
                public CValueObject<CXSwiftBusSettings>,
                public CXSwiftBusSettingsQtFree,
                public ITimestampBased
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexDBusServerAddress = CPropertyIndex::GlobalIndexCXSwiftBusSettings,
                    IndexMaxPlanes,
                    IndexDrawingLabels,
                    IndexMaxDrawingDistance,
                    IndexFollowAircraftDistance
                };

                //! Default constructor
                CXSwiftBusSettings();

                //! From JSON constructor
                CXSwiftBusSettings(const QString &json);

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc CXSwiftBusSettingsQtFree::getDBusServerAddress
                QString getDBusServerAddressQt() const { return QString::fromStdString(this->getDBusServerAddress()); }

                //! \copydoc CXSwiftBusSettingsQtFree::setDBusServerAddress
                void setDBusServerAddressQt(const QString &dBusAddress) { this->setDBusServerAddress(dBusAddress.toStdString()); }

                //! \copydoc CXSwiftBusSettingsQtFree::toXSwiftBusJsonString
                QString toXSwiftBusJsonStringQt() const { return QString::fromStdString(this->toXSwiftBusJsonString()); }

                //! \copydoc CXSwiftBusSettingsQtFree::parseXSwiftBusString
                void parseXSwiftBusStringQt(const QString &json) { this->parseXSwiftBusString(json.toStdString()); }

                //! Sets both timestamps
                virtual void setCurrentUtcTime() override;

                //! Valid settings?
                CStatusMessageList validate() const;

                //! Default value for settings
                static const CXSwiftBusSettings &defaultValue();

            private:
                BLACK_METACLASS(
                    CXSwiftBusSettings,
                    BLACK_METAMEMBER(dBusServerAddress),
                    BLACK_METAMEMBER(maxPlanes),
                    BLACK_METAMEMBER(drawingLabels),
                    BLACK_METAMEMBER(maxDrawDistanceNM),
                    BLACK_METAMEMBER(followAircraftDistanceM),
                    BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForComparison | DisabledForHashing)
                );
            };

            /*!
             * Setting for XSwiftBus.
             */
            struct TXSwiftBusSettings : public TSettingTrait<CXSwiftBusSettings>
            {
                //! \copydoc BlackMisc::TSettingTrait::key
                static const char *key() { return "xswiftbus/settings"; }

                //! \copydoc BlackMisc::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("XSwiftBus"); return name; }

                //! \copydoc BlackMisc::TSettingTrait::defaultValue
                static CXSwiftBusSettings defaultValue() { return CXSwiftBusSettings::defaultValue(); }

                //! \copydoc BlackMisc::TSettingTrait::isValid
                static bool isValid(const CXSwiftBusSettings &settings, QString &reason)
                {
                    const CStatusMessageList msgs = settings.validate();
                    reason = msgs.toQString(true);
                    return msgs.isSuccess();
                }
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CXSwiftBusSettings)

#endif // guard
