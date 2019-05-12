/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdint.h>

#include "mgos_neopixel.h"

#include "common/cs_dbg.h"
#include "mgos_bitbang.h"
#include "mgos_gpio.h"
#include "mgos_system.h"
#include "mgos_config.h"
#include "mgos.h"

struct mgos_neopixel* mgos_neopixel_create(int pin, int num_pixels,
    enum mgos_neopixel_order order, int num_channels)
{
    mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
    /* Keep in reset */
    mgos_gpio_write(pin, 0);

    struct mgos_neopixel* np = calloc(1, sizeof(struct mgos_neopixel));
    np->pin = pin;
    np->num_channels = num_channels > 0 ? num_channels : 3;
    np->num_pixels = num_pixels;
    np->order = order;
    np->timing = &global_timing;
    np->data = calloc(1, (size_t) (np->num_pixels * np->num_channels));
    mgos_neopixel_clear(np);
    return np;
}

void mgos_neopixel_set(struct mgos_neopixel* np, int i, int r, int g, int b)
{
    uint8_t* p = np->data + i * np->num_channels;
    switch (np->order) {
    case MGOS_NEOPIXEL_ORDER_RGB:
        p[0] = r;
        p[1] = g;
        p[2] = b;
        break;

    case MGOS_NEOPIXEL_ORDER_GRB:
        p[0] = g;
        p[1] = r;
        p[2] = b;
        break;

    case MGOS_NEOPIXEL_ORDER_BGR:
        p[0] = b;
        p[1] = g;
        p[2] = r;
        break;

    default:
        LOG(LL_ERROR, ("Wrong order: %d", np->order));
        break;
    }
}

void mgos_neopixel_clear(struct mgos_neopixel* np)
{
    memset(np->data, 0, np->num_pixels * np->num_channels);
}

void mgos_neopixel_show(struct mgos_neopixel* np)
{
    mgos_gpio_write(np->pin, 0);
    mgos_usleep(np->timing->RES);
    mgos_bitbang_write_bits(np->pin, MGOS_DELAY_100NSEC,
        np->timing->T0H, np->timing->T0L, np->timing->T1H, np->timing->T1L,
        np->data, np->num_pixels * np->num_channels);
    mgos_gpio_write(np->pin, 0);
    mgos_usleep(np->timing->RES);
    mgos_gpio_write(np->pin, 1);
}

void mgos_neopixel_free(struct mgos_neopixel* np)
{
    free(np->data);
    free(np);
}

bool mgos_neopixel_init(void)
{

    global_timing.T0H = mgos_sys_config_get_neopixel_T0H();
    global_timing.T1H = mgos_sys_config_get_neopixel_T1H();
    global_timing.T0L = mgos_sys_config_get_neopixel_T0L();
    global_timing.T1L = mgos_sys_config_get_neopixel_T1L();
    global_timing.RES = mgos_sys_config_get_neopixel_RES();

    global_timing.T0H = global_timing.T0H > 0 ? global_timing.T0H : 4;
    global_timing.T1H = global_timing.T1H > 0 ? global_timing.T1H : 14;
    global_timing.T0L = global_timing.T0L > 0 ? global_timing.T0L : 14;
    global_timing.T1L = global_timing.T1L > 0 ? global_timing.T1L : 4;
    global_timing.RES = global_timing.RES > 0 ? global_timing.RES : 60;

    return true;
}
