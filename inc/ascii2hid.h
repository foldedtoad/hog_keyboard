/*
 * Copyright (c) 2023   Callender-Consulting
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>

#include "hid.h"    // clone of USB hid.h

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

int ascii_to_hid(uint8_t ascii);
bool needs_shift(uint8_t ascii);
