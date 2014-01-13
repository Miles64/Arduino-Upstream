/*
  USBAPI.h
  Copyright (c) 2005-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __USBAPI__
#define __USBAPI__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#include "Arduino.h"

#if defined(USBCON)

#include "USBDesc.h"
#include "USBCore.h"

//================================================================================
//================================================================================
//	USB

class USBDevice_
{
public:
	USBDevice_();
	bool configured();

	void attach();
	void detach();	// Serial port goes down too...
	void poll();
	bool wakeupHost(); // returns false, when wakeup cannot be processed
};
extern USBDevice_ USBDevice;

//================================================================================
//================================================================================
//	Serial over CDC (Serial1 is the physical port)

struct ring_buffer;

#if (RAMEND < 1000)
#define SERIAL_BUFFER_SIZE 16
#else
#define SERIAL_BUFFER_SIZE 64
#endif

class Serial_ : public Stream
{
private:
	int peek_buffer;
public:
	Serial_() { peek_buffer = -1; };
	void begin(unsigned long);
	void begin(unsigned long, uint8_t);
	void end(void);

	virtual int available(void);
	virtual int peek(void);
	virtual int read(void);
	virtual void flush(void);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t*, size_t);
	using Print::write; // pull in write(str) and write(buf, size) from Print
	operator bool();

	volatile uint8_t _rx_buffer_head;
	volatile uint8_t _rx_buffer_tail;
	unsigned char _rx_buffer[SERIAL_BUFFER_SIZE];
};
extern Serial_ Serial;

#define HAVE_CDCSERIAL

//================================================================================
//================================================================================
//	Mouse

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

class Mouse_
{
private:
	uint8_t _buttons;
	void buttons(uint8_t b);
public:
	Mouse_(void);
	void begin(void);
	void end(void);
	void click(uint8_t b = MOUSE_LEFT);
	void move(signed char x, signed char y, signed char wheel = 0);	
	void moveAbs(int16_t x, int16_t y, int16_t wheel); // all parameters have the range of -32768 to 32767 and must be scaled to screen pixels
	void press(uint8_t b = MOUSE_LEFT);		// press LEFT by default
	void release(uint8_t b = MOUSE_LEFT);	// release LEFT by default
	bool isPressed(uint8_t b = MOUSE_LEFT);	// check LEFT by default
};
extern Mouse_ Mouse;

//================================================================================
//================================================================================
//	Keyboard

// Legacy keycodes. These are what you use with 'press()' and 'release()'
#define KEY_LEFT_CTRL		0x80
#define KEY_LEFT_SHIFT		0x81
#define KEY_LEFT_ALT		0x82
#define KEY_LEFT_GUI		0x83
#define KEY_RIGHT_CTRL		0x84
#define KEY_RIGHT_SHIFT		0x85
#define KEY_RIGHT_ALT		0x86
#define KEY_RIGHT_GUI		0x87

#define KEY_UP_ARROW		0xDA
#define KEY_DOWN_ARROW		0xD9
#define KEY_LEFT_ARROW		0xD8
#define KEY_RIGHT_ARROW		0xD7
#define KEY_BACKSPACE		0xB2
#define KEY_TAB				0xB3
#define KEY_RETURN			0xB0
#define KEY_ESC				0xB1
#define KEY_INSERT			0xD1
#define KEY_DELETE			0xD4
#define KEY_PAGE_UP			0xD3
#define KEY_PAGE_DOWN		0xD6
#define KEY_HOME			0xD2
#define KEY_END				0xD5
#define KEY_CAPS_LOCK		0xC1
#define KEY_F1				0xC2
#define KEY_F2				0xC3
#define KEY_F3				0xC4
#define KEY_F4				0xC5
#define KEY_F5				0xC6
#define KEY_F6				0xC7
#define KEY_F7				0xC8
#define KEY_F8				0xC9
#define KEY_F9				0xCA
#define KEY_F10				0xCB
#define KEY_F11				0xCC
#define KEY_F12				0xCD


// System Control values for Keyboard_::systemControl()
//  these defines come from the HID usage table "Generic Desktop Page (0x01)"
//  in the USB standard document "HID Usage Tables" (HUT1_12v2.pdf)
//  Currently this list contains only OSC (one shot control) values,
//  the implementation of systemControl will have to be changed when
//  adding OOC or RTC values.
#define SYSTEM_CONTROL_POWER_DOWN              1
#define SYSTEM_CONTROL_SLEEP                   2
#define SYSTEM_CONTROL_WAKEUP                  3
#define SYSTEM_CONTROL_COLD_RESTART            4
#define SYSTEM_CONTROL_WARM_RESTART            5
#define SYSTEM_CONTROL_DOCK                    6
#define SYSTEM_CONTROL_UNDOCK                  7
#define SYSTEM_CONTROL_SPEAKER_MUTE            8
#define SYSTEM_CONTROL_HIBERNATE               9
#define SYSTEM_CONTROL_DISPLAY_INVERT         10
#define SYSTEM_CONTROL_DISPLAY_INTERNAL       11
#define SYSTEM_CONTROL_DISPLAY_EXTERNAL       12
#define SYSTEM_CONTROL_DISPLAY_BOTH           13
#define SYSTEM_CONTROL_DISPLAY_DUAL           14
#define SYSTEM_CONTROL_DISPLAY_TOGGLE_INT_EXT 15
#define SYSTEM_CONTROL_DISPLAY_SWAP           16



// Consumer Control values for Keyboard_::systemControl()
//  these defines come from the HID usage table "Consumer Device Page (0x0c)"
//  in the USB standard document "HID Usage Tables" (HUT1_12v2.pdf)
//  Currently this list contains only OSC (one shot control) values,
//  the implementation of systemControl will have to be changed when
//  adding OOC or RTC values.

#define CONSUMER_CONTROL_VOLUME_MUTE        1
#define CONSUMER_CONTROL_VOLUME_UP          2
#define CONSUMER_CONTROL_VOLUME_DOWN        3
#define CONSUMER_CONTROL_PLAY_PAUSE         4
#define CONSUMER_CONTROL_STOP               5
#define CONSUMER_CONTROL_PREV_TRACK         6
#define CONSUMER_CONTROL_NEXT_TRACK         7
#define CONSUMER_CONTROL_EJECT              8

//	Low level key report: up to 6 keys and shift, ctrl etc at once
#define KEYREPORT_KEYCOUNT	0x06
typedef struct
{
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keys[KEYREPORT_KEYCOUNT];
} KeyReport;

class Keyboard_ : public Print
{
private:
	KeyReport _keyReport;
	void sendReport(KeyReport* keys);
public:
	Keyboard_(void);
	void begin(void);
	void end(void);
	virtual size_t write(uint8_t k);
	virtual size_t press(uint8_t k);
	virtual size_t release(uint8_t k);
	virtual size_t writeKeycode(uint8_t k);
	virtual size_t pressKeycode(uint8_t k);
	virtual size_t releaseKeycode(uint8_t k);
	virtual size_t addKeycodeToReport(uint8_t k);
	virtual size_t removeKeycodeFromReport(uint8_t k);
	virtual void releaseAll(void);
	virtual size_t systemControl(uint8_t k);
    virtual size_t consumerControl(uint8_t k);
};
extern Keyboard_ Keyboard;

//================================================================================
//================================================================================
//	Low level API

typedef struct
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t wValueL;
	uint8_t wValueH;
	uint16_t wIndex;
	uint16_t wLength;
} Setup;

//================================================================================
//================================================================================
//	HID 'Driver'

int		HID_GetInterface(uint8_t* interfaceNum);
int		HID_GetDescriptor(int i);
bool	HID_Setup(Setup& setup);
void	HID_SendReport(uint8_t id, const void* data, int len);

//================================================================================
//================================================================================
//	MSC 'Driver'

int		MSC_GetInterface(uint8_t* interfaceNum);
int		MSC_GetDescriptor(int i);
bool	MSC_Setup(Setup& setup);
bool	MSC_Data(uint8_t rx,uint8_t tx);

//================================================================================
//================================================================================
//	CSC 'Driver'

int		CDC_GetInterface(uint8_t* interfaceNum);
int		CDC_GetDescriptor(int i);
bool	CDC_Setup(Setup& setup);

//================================================================================
//================================================================================

#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

int USB_SendControl(uint8_t flags, const void* d, int len);
int USB_RecvControl(void* d, int len);

uint8_t	USB_Available(uint8_t ep);
int USB_Send(uint8_t ep, const void* data, int len);	// blocking
int USB_Recv(uint8_t ep, void* data, int len);		// non-blocking
int USB_Recv(uint8_t ep);							// non-blocking
void USB_Flush(uint8_t ep);


#endif /* if defined(USBCON) */

#endif
