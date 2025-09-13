#include <stdlib.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "pico/stdlib.h"

#define	LED1_GPIO	2
#define	LED2_GPIO	3
#define	LED3_GPIO	4

#define	BTN1_GPIO	15

static bool led1_state = false;
static bool led2_state = false;
static bool led3_state = false;

void main(void)
{
	stdio_init_all();
	stdio_set_driver_enabled(&stdio_uart, true);

	printf("%s: started\n", __func__);

	board_init();
	tusb_init();

	gpio_init(LED1_GPIO);
	gpio_init(LED2_GPIO);
	gpio_init(LED3_GPIO);
	gpio_init(BTN1_GPIO);

	gpio_set_dir(LED1_GPIO, GPIO_OUT);
	gpio_set_dir(LED2_GPIO, GPIO_OUT);
	gpio_set_dir(LED3_GPIO, GPIO_OUT);
	gpio_set_dir(BTN1_GPIO, GPIO_IN);
	gpio_pull_up(BTN1_GPIO);

	while (true) {
		tud_task();
	}
}


void tud_mount_cb(void)
{
	printf("%s: called\n", __func__);
}

void tud_umount_cb(void)
{
	printf("%s: called\n", __func__);
}

void tud_suspend_cb(bool remote)
{
	printf("%s: called\n", __func__);
}

void tud_resume_cb(void)
{
	printf("%s: called\n", __func__);
}


/*
 * Called when data is written into the device host->device
 */
void tud_vendor_rx_cb(uint8_t itf, uint8_t const *buffer, uint16_t bufsize)
{
	printf("%s: called, bufsize = %d\n", __func__, bufsize);

	if (bufsize) {
		switch (buffer[0]) {
		case 0:
			char buf = gpio_get(BTN1_GPIO) ? 1 : 0;
			tud_vendor_write(&buf, 1);
			tud_vendor_write_flush();
			printf("%s: val = 0x%01x\n", __func__, buf);
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

#if CFG_TUD_VENDOR_RX_BUFSIZE > 0
	tud_vendor_read_flush();
#endif
}

/*
 * Called when sending data completes
 */
void tud_vendor_tx_cb(uint8_t itf, uint32_t sent_bytes)
{
	printf("%s: called, itf = %d, sent_bytes = %ld\n", __func__, itf, sent_bytes);
}

/*
 * Called when control transfer happens. Always return true
 * becasue I do not understand why.
 */
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
				tusb_control_request_t const *request)
{
	printf("%s: called\n", __func__);
	return true;
}
