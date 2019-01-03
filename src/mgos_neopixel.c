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

#define NUM_CHANNELS 3 /* r, g, b */

struct mgos_neopixel {
  int pin;
  int num_pixels;
  enum mgos_neopixel_order order;
  uint8_t *data;
  uint8_t T0H;
  uint8_t T1H;
  uint8_t T0L;
  uint8_t T1L;
  uint8_t RES;
};

struct mgos_neopixel *mgos_neopixel_create(int pin, int num_pixels,
                                           enum mgos_neopixel_order order) {
  mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
  /* Keep in reset */
  mgos_gpio_write(pin, 0);

  int T0H = mgos_sys_config_get_neopixel_timing_T0H();
  int T1H = mgos_sys_config_get_neopixel_timing_T1H();
  int T0L = mgos_sys_config_get_neopixel_timing_T0L();
  int T1L = mgos_sys_config_get_neopixel_timing_T1L();
  int RES = mgos_sys_config_get_neopixel_timing_RES();

  struct mgos_neopixel *np = calloc(1, sizeof(*np));
  np->pin = pin;
  np->num_pixels = num_pixels;
  np->order = order;
  np->data = malloc(num_pixels * NUM_CHANNELS);
  np->T0H = T0H > 0 ? T0H : 3;
  np->T0L = T0L > 0 ? T0L : 8;
  np->T1H = T1H > 0 ? T1H : 8;
  np->T1L = T1L > 0 ? T1L : 6;
  np->RES = RES > 0 ? RES : 60;

  mgos_neopixel_clear(np);
  return np;
}

void mgos_neopixel_set(struct mgos_neopixel *np, int i, int r, int g, int b) {
  uint8_t *p = np->data + i * NUM_CHANNELS;
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

void mgos_neopixel_clear(struct mgos_neopixel *np) {
  memset(np->data, 0, np->num_pixels * NUM_CHANNELS);
}

void mgos_neopixel_show(struct mgos_neopixel *np) {
  mgos_gpio_write(np->pin, 0);
  mgos_usleep(60);
  mgos_bitbang_write_bits(np->pin, MGOS_DELAY_100NSEC, np->T0H, np->T0L, np->T1H, np->T1L, np->data,
                          np->num_pixels * NUM_CHANNELS);
  mgos_gpio_write(np->pin, 0);
  mgos_usleep(60);
  mgos_gpio_write(np->pin, 1);
}

void mgos_neopixel_free(struct mgos_neopixel *np) {
  free(np->data);
  free(np);
}

bool mgos_neopixel_init(void) {
  return true;
}
