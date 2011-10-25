#ifndef SYN_GLOBALS
#define SYN_GLOBALS

#define F_CPU	8000000UL         // Sets up the default speed for delay.h

#define	MAX	0xFF
#define	MIN	0x00

//data BUFF
#define N 1
char data_buf [N];
unsigned char data_buf_pos;
unsigned char Y_AVG;

//globalIndex - 0-255 - incremented during button presses.
unsigned char globalIndexSwitch;
unsigned char globalIndex;
unsigned char globalIndex2;

unsigned char cycleIndex;
unsigned char blinkMode;
unsigned char motionReact;
unsigned char cycle;
unsigned char modeTimer;
unsigned char trippy;

//Chill Vars
unsigned char R;
unsigned char G;
unsigned char B;
unsigned char LED;
char P_ACC;
char THETA;
char C_ACC;
char VEL;
unsigned char ACCEL_OFFSET;
unsigned char VMAP;
unsigned char color_1;
unsigned char color_2;

//globalMode - holds the current AVR state.
#define OFF 0x00
#define ON 0x01 //active with accel disabled.
#define ACCEL_OFF_24 0x02
#define ACCEL_OFF_190 0x03
#define ON_ACCEL 0x04 //active with accel enabled.
#define ON_ACCEL_24 0x05
#define ON_ACCEL_190 0x06
#define CYCLE 0x07
#define CYCLE_24 0x08
#define CYCLE_190 0x09
#define BOOYAH 10
unsigned char globalMode;

//AVR utility functions.
//init - first-time setup
void init(void);
//start - wake-up
void start(void);
//stop - sleep
void stop(void);

//BUTTON
#define PRESS_DELAY 0x01 //timer overflows before a button is being held as opposed to being pressed.
//Button states
#define BUTTON_OPEN 0x00 //button is open.
#define BUTTON_PROCESSING 0x01 //button has been activated; we're waiting on the debounce before taking action.
#define BUTTON_PRESSED 0x02 //button has been pressed quickly and released. set this to BUTTON_OPEN after taking action.
#define BUTTON_HOLDING 0x03 //button is being held.
unsigned char button; //holds state variable
unsigned char timer_ovf_cnt; //timer overflow counter.

#endif
