/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CRYPTO_CRYPTODTOMODE_H
#define BLACKCORE_AFV_CRYPTO_CRYPTODTOMODE_H

#include "msgpack.hpp"

//! DTO mode
enum class CryptoDtoMode
{
    Undefined = 0,
    None = 1,
    AEAD_ChaCha20Poly1305 = 2
};

MSGPACK_ADD_ENUM(CryptoDtoMode);

#endif // guard