/*
 *  hog.h  -- HOG keyboard interface
 *
 *  Copyright (c) 2023   Callender-Consulting
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#ifndef HOG_H
#define HOG_H

#include "ascii2hid.h"
#include <zephyr/usb/class/hid.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct {
    char * string;
    int    length;
    int    index;
} string_desc_t;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void hog_init(void);
void hog_send_string(string_desc_t * string_desc);

#endif /* HOG_H */
