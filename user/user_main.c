/*
  Copyright (c) 2016, Marko Viitanen (Fador)

  Permission to use, copy, modify, and/or distribute this software for any purpose 
  with or without fee is hereby granted, provided that the above copyright notice 
  and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
  REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
  AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, 
  INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
  PERFORMANCE OF THIS SOFTWARE.

*/

#define USE_US_TIMER
#include "ets_sys.h"
#include "driver/uart.h"
#include <c_types.h>
#include <user_interface.h>
#include <osapi.h>
#include <espconn.h>
#include "connection.h"
#include "io.h"
#include "user_network.h"
#include "configure.h"
#include "ws2812_lib.h"


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void user_init(void)
{
  int i;

  system_timer_reinit();
  uart_init(BIT_RATE_115200, BIT_RATE_115200);  
  
  // Use first credentials from the list
  network_update_credential(WLAN_SSID_LIST[0], WLAN_PASSWD_LIST[0]);
  
  // Init the lib here, so we can use it anywhere
  ws2812_init();
  
  // Indicate that we have successfully booted, light all the leds
  ets_intr_lock();
  ws2812_reset();
  for(i = 0; i < 16; i++) {
    ws2812_send_pixel(10, 10, 10);
  }
  ets_intr_unlock();
    
  os_printf("\nStart network init SDK version:%s\n", system_get_sdk_version());
  
  network_init();
  
  os_printf("\nReady\n");
}
