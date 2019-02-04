/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/aircraftcategorytreemodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/compare.h"
#include "blackmisc/icon.h"
#include "blackmisc/variant.h"
#include "blackmisc/propertyindex.h"

#include <QList>
#include <QMap>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        CAircraftCategoryTreeModel::CAircraftCategoryTreeModel(QObject *parent) : QStandardItemModel(parent)
        {
            m_columns.addColumn(CColumn::standardString("description", CAircraftCategory::IndexDescription));
            m_columns.addColumn(CColumn::standardString("changed", CAircraftCategory::IndexUtcTimestampFormattedYmdhms));
        }

        void CAircraftCategoryTreeModel::updateContainer(const CAircraftCategoryList &categories)
        {
            if (categories.isEmpty())
            {
                CAircraftCategoryTreeModel::clear();
                return;
            }

            m_categories = categories;
            m_categories.sortByLevel();
            QStandardItemModel::clear();
            QMap<int, QStandardItem *> items;
            this->setColumnCount(m_columns.size() + 1);

            for (const CAircraftCategory &category : m_categories)
            {
                QList<QStandardItem *> categoryRow;

                // ownership of QStandardItem is taken by model
                QStandardItem *si = new QStandardItem(
                    category.isAssignable() ? CIcons::paperPlane16() : CIcons::folder16(),
                    category.getLevelAndName()
                );
                si->setEditable(false);
                categoryRow.push_back(si);

                // add all clumns
                for (const CColumn &column : m_columns.columns())
                {
                    const CPropertyIndex i(column.getPropertyIndex());
                    const CVariant v(category.propertyByIndex(i));

                    if (column.getFormatter()->supportsRole(Qt::DecorationRole))
                    {
                        const QIcon icon = column.getFormatter()->decorationRole(v).toPixmap();
                        si = new QStandardItem(icon, QString());
                    }
                    else if (column.getFormatter()->supportsRole(Qt::DisplayRole))
                    {
                        const CVariant f = column.getFormatter()->displayRole(v);
                        si = new QStandardItem(f.toQString(true));
                    }
                    if (!si) { continue; }
                    si->setEditable(false); // make not editable
                    categoryRow.push_back(si);
                } // columns

                // add all items
                if (categoryRow.isEmpty()) { continue; }
                if (category.isFirstLevel())
                {
                    this->invisibleRootItem()->appendRow(categoryRow);
                }
                else
                {
                    const int p = category.getDepth() - 1;
                    items[p]->appendRow(categoryRow);
                }
                items.insert(category.getDepth(), categoryRow.front());
            }
        }

        void CAircraftCategoryTreeModel::clear()
        {
            m_categories.clear();
            QStandardItemModel::clear();
        }
    }  // namespace
} // namespace