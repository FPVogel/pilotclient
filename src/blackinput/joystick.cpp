/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "joystick.h"

#if defined(Q_OS_WIN)
    #include "win/joystickwindows.h"
#elif defined(Q_OS_LINUX)
    #include "linux/joysticklinux.h"
#elif defined(Q_OS_OSX)
    #include "osx/joystickmac.h"
#else
    #error "Platform is not supported!"
#endif

namespace BlackInput
{

    IJoystick::IJoystick(QObject *parent) :
        QObject(parent)
    {
    }

    std::unique_ptr<IJoystick> IJoystick::create(QObject *parent)
    {
#if defined(Q_OS_WIN)
        std::unique_ptr<IJoystick> ptr(new CJoystickWindows(parent));
#elif defined(Q_OS_LINUX)
        std::unique_ptr<IJoystick> ptr(new CJoystickLinux(parent));
#elif defined(Q_OS_OSX)
        std::unique_ptr<IJoystick> ptr(new CJoystickMac(parent));
#endif

        return ptr;
    }

} // BlackInput
