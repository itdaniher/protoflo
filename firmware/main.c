// (C) 2011 Ian Daniher (Nonolith Labs) <ian@nonolithlabs.com>

#define F_CPU 32000000UL
#include <avr/io.h>
#include <avr/delay.h>

#include "Descriptors.h"

#include "usb.h"

/* Function Prototypes: */
void configHardware(void);
bool EVENT_USB_Device_ControlRequest(USB_Request_Header_t* req);

int main(void){
	configHardware();
	sei();	

	while (1){
			USB_Task(); // Lower-priority USB polling, like control requests
	}
}

void configHardware(void){
	USB_ConfigureClock();
	USB_Init();
}

/** Event handler for the library USB Control Request reception event. */
bool EVENT_USB_Device_ControlRequest(USB_Request_Header_t* req){
	if ((req->bmRequestType & CONTROL_REQTYPE_TYPE) == REQTYPE_VENDOR){
		switch(req->bRequest){
			case 0xBB: // bootload
				USB_ep0_send(0);
				USB_ep0_wait_for_complete();
				_delay_us(10000);
				USB_Detach();
				_delay_us(100000);
				void (*enter_bootloader)(void) = (void *) 0x47fc /*0x8ff8/2*/;
				enter_bootloader();
				break;
		}
		return true;
	}
	return false;
}

