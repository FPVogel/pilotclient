/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_ACTIONHOTKEYDEFS_H
#define BLACKMISC_INPUT_ACTIONHOTKEYDEFS_H

#include "blackmisc/icon.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc
{
    namespace Input
    {
        //! PTT key
        BLACKMISC_EXPORT const QString &pttHotkeyAction();

        //! PTT key COM1 only
        BLACKMISC_EXPORT const QString &pttCom1HotkeyAction();

        //! PTT key COM2 only
        BLACKMISC_EXPORT const QString &pttCom2HotkeyAction();

        //! PTT key
        BLACKMISC_EXPORT const QPixmap &pttHotkeyIcon();

        //! Audio volume + key
        BLACKMISC_EXPORT const QString &audioVolumeIncreaseHotkeyAction();

        //! Audio volume - key
        BLACKMISC_EXPORT const QString &audioVolumeDecreaseHotkeyAction();

        //! Audio icon volume +
        BLACKMISC_EXPORT const QPixmap &audioVolumeIncreaseHotkeyIcon();

        //! Audio icon volume -
        BLACKMISC_EXPORT const QPixmap &audioVolumeDecreaseHotkeyIcon();

        //! XPDR state
        BLACKMISC_EXPORT const QString &toggleXPDRStateHotkeyAction();

        //! XPDR state
        BLACKMISC_EXPORT const QPixmap &toggleXPDRStateHotkeyIcon();

        //! XPDR state
        BLACKMISC_EXPORT const QString &toggleXPDRIdentHotkeyAction();

        //! XPDR ident
        BLACKMISC_EXPORT const QPixmap &toggleXPDRIdentHotkeyIcon();

    } // ns
} // ns

#endif // guard
