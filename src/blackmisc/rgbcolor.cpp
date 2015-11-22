/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/rgbcolor.h"
#include "stringutils.h"
#include "comparefunctions.h"
#include <QPainter>

using namespace BlackMisc;

namespace BlackMisc
{
    CRgbColor::CRgbColor(const QString &color, bool isName)
    {
        this->setByString(color, isName);
    }

    CRgbColor::CRgbColor(int r, int g, int b) : m_r(r), m_g(g), m_b(b)
    { }

    CRgbColor::CRgbColor(const QColor &color)
    {
        this->setQColor(color);
    }

    CIcon CRgbColor::toIcon() const
    {
        if (this->isValid())
        {
            QPixmap pixmap(QSize(16, 16));
            QPainter p(&pixmap);
            QBrush brush(this->toQColor());
            p.setBackground(brush);
            p.setBrush(this->toQColor());
            p.drawRect(0, 0, 16, 16);
            CIcon icon(pixmap, hex());
            return icon;
        }
        else
        {
            return CIcon::iconByIndex(CIcons::StandardIconError16);
        }
    }

    QColor CRgbColor::toQColor() const
    {
        return QColor(red(), green(), blue());
    }

    bool CRgbColor::setQColor(const QColor &color)
    {
        if (color.isValid())
        {
            m_r = color.red();
            m_g = color.green();
            m_b = color.blue();
            return true;
        }
        else
        {
            this->setInvalid();
            return false;
        }
    }

    int CRgbColor::red() const
    {
        return m_r;
    }

    double CRgbColor::normalizedRed() const
    {
        double c = red() * 1.0;
        return c / colorRange();
    }

    QString CRgbColor::redHex(int digits) const
    {
        return intToHex(m_r, digits);
    }

    int CRgbColor::green() const
    {
        return m_g;
    }

    double CRgbColor::normalizedGreen() const
    {
        double c = green() * 1.0;
        return c / colorRange();
    }

    QString CRgbColor::greenHex(int digits) const
    {
        return intToHex(m_g, digits);
    }

    int CRgbColor::blue() const
    {
        return m_b;
    }

    double CRgbColor::normalizedBlue() const
    {
        double c = blue() * 1.0;
        return c / colorRange();
    }

    QString CRgbColor::blueHex(int digits) const
    {
        return intToHex(m_b, digits);
    }

    QString CRgbColor::hex(bool withHash) const
    {
        if (!isValid()) { return ""; }
        QString h(redHex() + greenHex() + blueHex());
        return withHash ? "#" + h : h;
    }

    void CRgbColor::setByString(const QString &color, bool isName)
    {
        if (color.isEmpty()) { return; }
        else if (isName)
        {
            QColor q(color);
            m_r = q.red();
            m_g = q.green();
            m_b = q.blue();
        }
        else
        {
            QString c(color.trimmed());
            QColor q(c);
            if (setQColor(q)) { return; }
            if (c.startsWith("#")) { this->setInvalid(); return; }
            q.setNamedColor("#" + c);
            this->setQColor(q);
        }
    }

    bool CRgbColor::isValid() const
    {
        return m_r >= 0 && m_g >= 0 && m_b >= 0;
    }

    double CRgbColor::colorDistance(const CRgbColor &color) const
    {
        if (!this->isValid() && !color.isValid()) { return 0; }
        if (!this->isValid() || !color.isValid()) { return 1; }
        if (*this == color) { return 0.0; } // avoid rounding

        // all values 0-1
        double rd = (normalizedRed() - color.normalizedRed());
        double bd = (normalizedBlue() - color.normalizedBlue());
        double gd = (normalizedGreen() - color.normalizedGreen());
        return (rd * rd + bd * bd + gd * gd) / 3.0;
    }

    void CRgbColor::setInvalid()
    {
        m_r = -1;
        m_g = -1;
        m_b = -1;
    }

    QString CRgbColor::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return hex();
    }

    CVariant CRgbColor::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexBlue:
            return CVariant::fromValue(blue());
        case IndexRed:
            return CVariant::fromValue(red());
        case IndexGreen:
            return CVariant::fromValue(green());
        case IndexWebHex:
            return CVariant::fromValue(hex());
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CRgbColor::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself()) { (*this) = variant.to<CRgbColor>(); return; }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexBlue:
            this->m_b = variant.toInt();
            break;
        case IndexRed:
            this->m_r = variant.toInt();
            break;
        case IndexGreen:
            this->m_g = variant.toInt();
            break;
        case IndexWebHex:
            this->setByString(variant.toQString());
            break;
        default:
            CValueObject::setPropertyByIndex(variant, index);
            break;
        }
    }

    int CRgbColor::comparePropertyByIndex(const CRgbColor &compareValue, const CPropertyIndex &index) const
    {
        if (index.isMyself()) { return this->hex().compare(compareValue.hex(), Qt::CaseInsensitive); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexBlue:
            return Compare::compare(this->m_b, compareValue.m_b);
        case IndexRed:
            return Compare::compare(this->m_r, compareValue.m_r);
        case IndexGreen:
            return Compare::compare(this->m_g, compareValue.m_g);
        case IndexWebHex:
            this->hex().compare(compareValue.hex(), Qt::CaseInsensitive);
            break;
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Missing compare");
            break;
        }
        return 0;
    }

    double CRgbColor::colorRange() const
    {
        if (!this->isValid()) { return 255; }
        if (m_b < 256 && m_g < 256 && m_r < 256) { return 255; }
        if (m_b < 4096 && m_g < 4096 && m_r < 4096) { return 4095; }
        return 65535;
    }

    QString CRgbColor::intToHex(int h, int digits)
    {
        return BlackMisc::intToHex(h, digits);
    }

} // namespace
