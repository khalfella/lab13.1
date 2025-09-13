#include "pico/stdio.h"
#include "tusb.h"
#include "device/usbd.h"


#define MAX_ENDPOINT0_SIZE	64
#define ENDPOINT_BULK_SIZE	64

#define	BULK_IN_ENDPOINT_DIR	0x81
#define	BULK_OUT_ENDPOINT_DIR	0x02

enum {
	ITF_NUM_VENDOR		= 0,
	ITF_NUM_TOTAL,
};

enum {
	STRID_LANGID		= 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
};

#define USBLED_VENDOR_ID	0xcafe  /* TinyUSB */
#define USBLED_PRODUCT_ID	0x4004

tusb_desc_device_t const desc_device = {
	.bLength		= sizeof(tusb_desc_device_t),
	.bDescriptorType	= TUSB_DESC_DEVICE,
	.bcdUSB			= 0x0110, /* USB 1.1 */
	.bDeviceClass		= TUSB_CLASS_VENDOR_SPECIFIC,
	.bDeviceSubClass	= 0x00,
	.bDeviceProtocol	= 0x00,
	.bMaxPacketSize0	= MAX_ENDPOINT0_SIZE,
	.idVendor		= USBLED_VENDOR_ID,
	.idProduct		= USBLED_PRODUCT_ID,
	.bcdDevice		= 0x0001,
	.iManufacturer		= STRID_MANUFACTURER,
	.iProduct		= STRID_PRODUCT,
	.iSerialNumber		= STRID_SERIAL,
	.bNumConfigurations	= 0x01,
};

uint8_t const * tud_descriptor_device_cb(void)
{
	return (uint8_t const *)&desc_device;
}

// Interface number, string index, EP Out & IN address, EP size
#define TUD_VENDOR_DESCRIPTOR_MK(_itfnum, _stridx, _epout, _epin, _epsize) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0x00, 0x00, _stridx,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_epsize), 0


#define	CONFIG_TOTAL_LEN	(TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)
uint8_t static desc_configuration[] = {
	TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x80, 100),
	TUD_VENDOR_DESCRIPTOR_MK(ITF_NUM_VENDOR, 0, BULK_OUT_ENDPOINT_DIR, BULK_IN_ENDPOINT_DIR, ENDPOINT_BULK_SIZE),
};

char const * string_desc_arr[] = {
	[STRID_LANGID]		= (const char[]) { 0x09, 0x04 },	/* Supported language ID - English (US) */
	[STRID_MANUFACTURER]	= "tinyusb",
	[STRID_PRODUCT]		= "tinyusb vendor product",
	[STRID_SERIAL]		= "12345",
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
	(void) index;

	static_assert(sizeof(desc_configuration) == CONFIG_TOTAL_LEN, "Configuration descriptor size mismatch");

	return desc_configuration;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	static uint16_t _desc_str[32 + 1];
	(void) langid;
	size_t chr_count, max_count;
	const char *str;

	switch (index) {
	case STRID_LANGID:
		memcpy(&_desc_str[1], string_desc_arr[0], 2);
		chr_count = 1;
		break;
	case STRID_SERIAL:
	case STRID_MANUFACTURER:
	case STRID_PRODUCT:
		str = string_desc_arr[index];
		chr_count = strlen(str);

		max_count = (sizeof(_desc_str) / sizeof(_desc_str[0])) - 1;
		if (chr_count > max_count)
			chr_count = max_count;

		for (size_t ii = 0; ii < chr_count; ii++)
			_desc_str[1 + ii] = str[ii];
		break;
	default:
		return NULL;
	}

	_desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
	return _desc_str;
}
