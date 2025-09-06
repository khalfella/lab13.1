/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "pico/stdlib.h"

/* This example demonstrate HID Generic raw Input & Output.
 * It will receive data from Host (In endpoint) and echo back (Out endpoint).
 * HID Report descriptor use vendor for usage page (using template TUD_HID_REPORT_DESC_GENERIC_INOUT)
 *
 * There are 2 ways to test the sketch
 * 1. Using nodejs
 * - Install nodejs and npm to your PC
 *
 * - Install excellent node-hid (https://github.com/node-hid/node-hid) by
 *   $ npm install node-hid
 *
 * - Run provided hid test script
 *   $ node hid_test.js
 *
 * 2. Using python
 * - Install `hid` package (https://pypi.org/project/hid/) by
 *   $ pip install hid
 *
 * - hid package replies on hidapi (https://github.com/libusb/hidapi) for backend,
 *   which already available in Linux. However on windows, you may need to download its dlls from their release page and
 *   copy it over to folder where python is installed.
 *
 * - Run provided hid test script to send and receive data to this device.
 *   $ python3 hid_test.py
 */

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

#define	LED1_GPIO	2
#define	LED2_GPIO	3
#define	LED3_GPIO	4

#define	BTN1_GPIO	15

static bool led1_state = false;
static bool led2_state = false;
static bool led3_state = false;

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  // init device stack on configured roothub port
  tusb_rhport_init_t dev_init = {
    .role = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_AUTO
  };
  tusb_init(BOARD_TUD_RHPORT, &dev_init);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  gpio_init(LED1_GPIO);
  gpio_init(LED2_GPIO);
  gpio_init(LED3_GPIO);
  gpio_init(BTN1_GPIO);

  gpio_set_dir(LED1_GPIO, GPIO_OUT);
  gpio_set_dir(LED2_GPIO, GPIO_OUT);
  gpio_set_dir(LED3_GPIO, GPIO_OUT);

  gpio_set_dir(BTN1_GPIO, GPIO_IN);
  gpio_pull_up(BTN1_GPIO);

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  (void) itf;
  (void) report_id;
  (void) report_type;

	char buff;
	int i;

	if (bufsize) {
		switch (buffer[0]) {
		case 0:
			buff = gpio_get(BTN1_GPIO) ? 1 : 0;
  			tud_hid_report(0, &buff, 1);
			break;
		case 1:
			led1_state = !led1_state;
  			gpio_put(LED1_GPIO, led1_state);
			break;
		case 2:
			led2_state = !led2_state;
  			gpio_put(LED2_GPIO, led2_state);
			break;
		case 3:
			led3_state = !led3_state;
  			gpio_put(LED3_GPIO, led3_state);
			break;
		}
	}
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
