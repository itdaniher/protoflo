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

void initLED0(void){
	PORTE.DIRSET = 1 << 1 | 1 << 2 | 1 << 3; // LED0B, LED0G, LED0R
	TCE0.CTRLA = TC_CLKSEL_DIV64_gc;
	TCE0.CTRLB = TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	TCE0.PER = 1024;
	TCE0.CNT = 0;
}

void initOtherLEDs(void){
	PORTB.DIRSET = 1 << 0 | 1 << 1;
	PORTC.DIRSET = 1 << 1 | 1 << 2 | 1 << 5 | 1 << 7;
}

void initADC(void){
    ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc | 1 << ADC_CONMODE_bp | 1 << ADC_IMPMODE_bp | ADC_CURRLIMIT_NO_gc | ADC_FREERUN_bm;                                             
    ADCA.REFCTRL =  ADC_REFSEL_VCC_gc;
    ADCA.PRESCALER = ADC_PRESCALER_DIV128_gc;
    ADCA.EVCTRL = ADC_SWEEP_012_gc;
    ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_1X_gc;
    ADCA.CH1.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_1X_gc;
    ADCA.CH2.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_1X_gc;
    ADCA.CH0.MUXCTRL = 0b100 | ADC_CH_MUXPOS_PIN2_gc; //  INTGND vs ACCEL-X
    ADCA.CH1.MUXCTRL = 0b100 |  ADC_CH_MUXPOS_PIN3_gc; // INTGND vs ACCEL-Y
    ADCA.CH2.MUXCTRL = 0b100 | ADC_CH_MUXPOS_PIN4_gc; // INTGND vs ACCEL-Z
    ADCA.CTRLA = ADC_ENABLE_bm;
}

void configHardware(void){
	USB_ConfigureClock();
	USB_Init();
	initLED0();
	initOtherLEDs();
	initADC();
}

/** Event handler for the library USB Control Request reception event. */
bool EVENT_USB_Device_ControlRequest(USB_Request_Header_t* req){
	if ((req->bmRequestType & CONTROL_REQTYPE_TYPE) == REQTYPE_VENDOR){
		switch(req->bRequest){
			case 0x70: // LED0
				switch(req->wIndex){
					case 0x00: // R
						TCE0.CCD = req->wValue;
						break;
					case 0x01: // G
						TCE0.CCB = req->wValue;
						break;
					case 0x02: // B
						TCE0.CCC = req->wValue;
						break;
				}	
				USB_ep0_send(0);
				break;
			case 0x73: // LEDs
				if (req->wValue&0x01){
					switch (req->wIndex){
						case 0x00:
							PORTB.OUTSET = 1 << 1;
							break;
						case 0x01:
							PORTB.OUTSET = 1 << 0;
							break;
						case 0x02:
							PORTC.OUTSET = 1 << 1;
							break;
						case 0x03:
							PORTC.OUTSET = 1 << 2;
							break;
						case 0x04:
							PORTC.OUTSET = 1 << 5;
							break;
						case 0x05:
							PORTC.OUTSET = 1 << 7;
							break;
						}
					}
				else{
					switch (req->wIndex){
						case 0x00:
							PORTB.OUTCLR = 1 << 1;
							break;
						case 0x01:
							PORTB.OUTCLR = 1 << 0;
							break;
						case 0x02:
							PORTC.OUTCLR = 1 << 1;
							break;
						case 0x03:
							PORTC.OUTCLR = 1 << 2;
							break;
						case 0x04:
							PORTC.OUTCLR = 1 << 5;
							break;
						case 0x05:
							PORTC.OUTCLR = 1 << 7;
							break;
						}
					}
					USB_ep0_send(0);
					break;
			case 0xAC: // accelerometer
				ep0_buf_in[0] = (ADCA.CH0.RES>>2)&0xFF;
				ep0_buf_in[1] = (ADCA.CH1.RES>>2)&0xFF;
				ep0_buf_in[2] = (ADCA.CH2.RES>>2)&0xFF;
				USB_ep0_send(3);
				break;
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

