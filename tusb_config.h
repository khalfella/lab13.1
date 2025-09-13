/*
 * This header file will be included by tinyusb code. Put stuff here that is
 * expected to influence how we want tinyusb to work for us.
 */

#ifndef	TUSB_CONFIG_H
#define	TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


/* Someone must define tinyusb uc */
#ifndef	CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

/* Root Hub Port */
#ifndef	BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT	0
#endif

#ifndef	BOARD_TUD_MAX_SPEED
#define	BOARD_TUD_MAX_SPEED	OPT_MODE_FULL_SPEED
#endif

#define	CFG_TUD_MAX_SPEED	BOARD_TUD_MAX_SPEED
#define	CFG_TUSB_RHPORT0_MODE	(OPT_MODE_DEVICE | BOARD_TUD_MAX_SPEED)

#ifndef	CFG_TUSB_OS
#define	CFG_TUSB_OS		OPT_OS_NONE
#endif

#ifdef CFG_TUSB_DEBUG
#undef CFG_TUSB_DEBUG
#endif

#define	CFG_TUSB_DEBUG		0	/* Debug level */


#define	CFG_TUD_DEVICE			1	/* device support */
#define	CFG_TUD_ENDPOINT0_SIZE		64
#define	CFG_TUD_VENDOR			1
#define	CFG_TUD_VENDOR_EP_BUFSIZE	64
#define CFG_TUD_VENDOR_RX_BUFSIZE	64
#define CFG_TUD_VENDOR_TX_BUFSIZE	64

#define	CFG_TUD_DFU_RT			0
#define CFG_TUD_CDC			0
#define CFG_TUD_MSC			0
#define CFG_TUD_HID			0
#define CFG_TUD_MIDI			0
#define CFG_TUD_AUDIO			0
#define CFG_TUD_VIDEO			0
#define cFG_TUD_BTH			0

#ifdef __cplusplus
}
#endif

#endif /* TUSB_CONFIG_H */
