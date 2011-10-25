#include "USI_TWI_Master.h"
#include "PCA9635.h"
#include "MMA7455l.h"
#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
//#include <math.h>

/********PCA9635 Functions**********/

/********
PCA9635_init - Initializes the PCA9635 for the first time.
	PCA9535 pinout:
		LED0 - RGB1R
		LED1 - RGB1G
		LED2 - RGB1B
		LED3 - WHITE1
		LED4 - RGB2R
		LED5 - RGB2G
		LED6 - RGB2B
		LED7 - WHITE2
********/
//USI_TWI_Master_Initialise MUST be called before this function.
void PCA9635_init(void)
{
	//Writing address and register info.
	PCA_messageBuf[0]=(PCA_ALLCALL_ADDR<<TWI_ADR_BITS)|(0<<TWI_READ_BIT);
	PCA_messageBuf[1]=PCA_MODE1|0b100<<5;
	//Setting all channels to MIN to begin with.
	unsigned char i;
	for(i=4; i<20; i++)
	{
		PCA_messageBuf[i]=MIN;
	}
	//Using setmode in order to be consistent in changing MODE2, GRPFREQ, etc.
	//setMode also sends the information to the PCA.
	PCA9635_setMode(BLINK_NONE,0x80);
}

/********
PCA9635_setMode - Sets the mode of the PCA.
	@param blinkMode:
		BLINK_NONE: no blinking. disables GRPFREQ and GRPPWM, using only individual PWM resgisters.
		BLINK_24: blinks at 24 Hz by setting GRPFREQ at 24 Hz.
		BLINK_190: blinks at 190 Hz, enabled by setting bit 5 of MODE2 to 0; default blink freq.
	@param blinkPWM:
		all blink modes subject to blinking PWM. this is controlled via this param. default param in is 0x80 during init.
********/
//USI_TWI_Master_Initialise MUST be called before this function.
void PCA9635_setMode(unsigned char blinkMode, unsigned char blinkPWM)
{
	currentBlinkMode=blinkMode;
	//MODE1 register does not change depending on modes.
	PCA_messageBuf[2]=0b10000001;
	//GRPPWM and GRPFREQ are static over different modes.
	PCA_messageBuf[20]=blinkPWM;
	PCA_messageBuf[21]=0x00;
	//we never use LEDOUT2, LEDOUT3.
	PCA_messageBuf[24]=0x00;
	PCA_messageBuf[25]=0x00;
	//setting MODE2, LEDOUT0, LEDOUT1.
	switch(blinkMode)
	{
		default:
		case BLINK_NONE:
			//MODE2 does not matter, use default.
			PCA_messageBuf[3]=0b00110100;
			//Disable GRP blinking on all pins.
			PCA_messageBuf[22]=0b10101010;
			PCA_messageBuf[23]=0b10101010;
			break;
		case BLINK_24:
			//Use GRPFREQ via MODE2.
			PCA_messageBuf[3]=0b00110100;
			//Enable GRP blinking.
			PCA_messageBuf[22]=0xFF;
			PCA_messageBuf[23]=0xFF;
			break;
		case BLINK_190:
			//Set MODE2 to NOT use GRPFREQ.
			PCA_messageBuf[3]=0b00010100;
			//Enable GRP blinking.
			PCA_messageBuf[22]=0xFF;
			PCA_messageBuf[23]=0xFF;
	}
	USI_TWI_Start_Read_Write(PCA_messageBuf,26);
}

/********
PCA9635_update - Updates the individual PWMs of the PCA.
	This function sends ONLY the individual PWM registers.
********/
//USI_TWI_Master_Initialise MUST be called before this function.
void PCA9635_update(void)
{
	//Overwrite MODE1 and MODE2 data with the I2C address and start register, respectively.
	PCA_messageBuf[2]=PCA_messageBuf[0];
	PCA_messageBuf[3]=PCA_PWM0|0b101<<5;
	//Use the array starting at index 2. I.e., turn the array into a pointer, increment pointer by 2 and return it as the array.
	USI_TWI_Start_Read_Write(&PCA_messageBuf[2],18);
}

/********
PCA9635_stop - Sleeps the PCA.
	All this function does is pull OE high, thus disabling all output on the PCA, effectively sleeping it.
	Implemented as function in case there is a better sleep mode possible. If this is not the case, we can reasonably save function overhead by inlining the command.
********/
void PCA9635_stop(void)
{
	//Clear all PORTB pins. Enable pullup on PB3 and output HIGH on PB1.
	PORTB=(1<<PB1)|(1<<PB3);
}

/********MMA7455L Functions**********/

/********
MMA7455L_init - Initializes the MMA7455L for the first time.
	TODO: Initialize and test tap-to-switch interrupt on INT1.
	Initializes the MMA7455 in interrupting 8g mode.
********/
//USI_TWI_Master_Initialise MUST be called before this function.
void MMA7455L_init(void)
{
	//Sets the MMA address. 
	MMA_messageBuf[0]=(MMA_ADDR<<TWI_ADR_BITS)|(0<<TWI_READ_BIT);
	//Sets MCTL.
	MMA_messageBuf[1]=MMA_MCTL;
	MMA_messageBuf[2]=MMA_MCTL_VALUE_ON;
	//Zero out the rest of messageBuf.
	MMA_messageBuf[3]=0x00;
	MMA_messageBuf[4]=0x00;
	USI_TWI_Start_Read_Write(MMA_messageBuf,3);
}

/********
MMA7455L_poll - Updates X_ACC, Y_ACC and Z_ACC.
	Polls the MMA7455 over I2C. Returns the 8-bit accelerations as signed chars in MMA_messageBuf[1:3].
	Accels can also be accessed with X_ACC, Y_ACC, Z_ACC.
********/
//USI_TWI_Master_Initialise MUST be called before this function.
void MMA7455L_poll(void)
{
                //If we are not using the global ON and using accelerometer mode, refuse to update the accelerations.
//            if(globalMode!=ON_ACCEL)
//            {
//                            return;
//            }
                //MMA address.
                MMA_messageBuf[0]=(MMA_ADDR<<TWI_ADR_BITS)|(1<<TWI_READ_BIT);
                //First register, auto-increments to pull YOUT8 and ZOUT8 as well.
                MMA_messageBuf[1] = MMA_XOUT8;
                //size param as 5 bytes - bytes to read+2, as per docs.
                USI_TWI_Start_Random_Read( MMA_messageBuf, 5 );

				unsigned char Y_HOLDER;
				Y_HOLDER = (char)Y_ACC;
				if(Y_HOLDER<0)
				{
					Y_HOLDER = 0;
				}
				if (Y_HOLDER > 127)
				{
					Y_HOLDER = -Y_ACC;
				}
				//if (Y_HOLDER < 21)
				//{
				//	Y_HOLDER = 20;
				//}
                //if (Y_HOLDER > 20)
				//{
				Y_AVG=(char)((Y_AVG*14.0+Y_HOLDER)/15.0);
				//}
				//if (Y_AVG<21)
				//{
				//	Y_AVG = 20;
				//}
}


/********
MMA7455L_stop - Sleeps the MMA.
	Sends I2C command to write to MCTL register switching the mode to sleep mode.
********/
//USI_TWI_Master_Initialise MUST be called before this function.
void MMA7455L_stop(void)
{
	//MMA address.
	MMA_messageBuf[0]=(MMA_ADDR<<TWI_ADR_BITS)|(0<<TWI_READ_BIT);
	//MCTL register changing.
	MMA_messageBuf[1]=MMA_MCTL;
	//MCTL OFF value - just 0x00, really.
	MMA_messageBuf[2]=MMA_MCTL_VALUE_OFF;
	USI_TWI_Start_Read_Write(MMA_messageBuf,3);
}

/********Interrupt Vectors**********/

/********
Timer 0 Overflow Interrupt.
	Called when Timer0 overflows (~ ever 0.03 sec).
	Interrupt is enabled when button pin goes from HIGH to LOW.
	Serves two purposes:
		Debounces button press signal by adding a delay before declaring it as a BUTTON_PRESSED event.
		Trigger the BUTTON_HOLDING signal if button is still LOW.
********/
ISR(TIMER0_OVF_vect)
{
	//If we are beyond the time limit for enabling hold selection, and the pin is still LOW.
	//	time limit - PRESS_DELAY*(~ 0.03) sec.
	if((timer_ovf_cnt>PRESS_DELAY)&&((~PINB)&(1<<PINB3)))
	{
		//declare the button as being held.
		button=BUTTON_HOLDING;
		//disable this interrupt signal, we will hold until the button is opened with a pin change.
		TIMSK=0x00;
		return;
	}
	//If the button has been released to HIGH.
	if(PINB&(1<<PINB3))
	{
		//declar the button as pressed.
		button=BUTTON_PRESSED;
		//disable the interrupt signal, the main loop will clear the status after processing.
		TIMSK=0x00;
		return;
	}
	//If it's not time to declare either state, wait some more.
	timer_ovf_cnt++;
}

/********
Pin Change Interrupt.
	Called when  or PB3 change pin states (either HIGH to LOW or LOW to HIGH)
	Serves four purposes:
		Wakes micro from sleep mode.
		Changes blinking mode when PB4 becomes HIGH.
		Triggers debouncing delay when the button changes from HIGH to LOW.
		Clears the button when it is released after a HOLDING state.
********/
ISR(PCINT0_vect)
{
	//If the sleep enabled pin is set, we need to wake up.
	if(MCUCR&(1<<SE))
	{
		//Wake up function.
		start();
		//Do not return so that BUTTON_PRESSED can be set to enable the MMA and update globalMode.
	}
	//If PB4 has changed from LOW to HIGH.
	if(PINB&(1<<PINB4))
	{
		//Increment blink mode. Only three modes, loop if need be.
//		PCA9635_setMode((currentBlinkMode+1)%0x05,0x80);
	}
	//If PB3 has changed from HIGH to LOW.
	if((~PINB)&(1<<PINB3))
	{
		//AND if the button has been open.
		if(button==BUTTON_OPEN)
		{
			//Then we're starting closing procedures.
			//Zero out timer counter.
			timer_ovf_cnt=0x00;
			//Set the button to block extra opening pin change interrupt actions.
			button=BUTTON_PROCESSING;
			//Clear Timer0.
			TCNT0=0x00;
			//Enable the overflow interrupt.
			TIMSK=(1<<TOIE0);
		}
	}
	//If PB3 has change from LOW to HIGH.
	if(PINB&(1<<PINB3))
	{
		//And we are not doing debouncing.
		if(button!=BUTTON_PROCESSING)
		{
			//Then make sure the button is marked as open; set the mode.
			if (button == BUTTON_HOLDING)
			{
				globalIndexSwitch = (globalIndexSwitch+1)%2;
			} 
			button=BUTTON_OPEN;


		}
	}
}

/********Main Control Functions**********/

/********
init - initializes the AVR for the first time.
	Sets:
		Default values
		Pin config
		Interrupt config
		I2C init
		Peripherals init
********/
void init(void)
{
	//Default Values.
	globalIndex=180;
	globalIndex2=225;
	globalIndexSwitch = 0;
	globalMode=ON;
	cycleIndex=0;
	Y_AVG = 0;
	motionReact = 0;
	cycle = 0;
	modeTimer=0;
	button=BUTTON_OPEN;
	R = 0x00;
	G = 0x00;
	B = 0x00;
	LED = 0x00;
	P_ACC = 0x00;
	THETA = 0x00;
	C_ACC = 0x00;
	VEL = 0x00;
	VMAP = 0x00;
	color_1 = 0x00;
	color_2 = 0x00;

	//Data Buffer Init
	//data_buf_pos = 0;
	//unsigned char idx;
	//for (idx=0;idx<N;idx++)
	//	{data_buf[idx]=0;}

	//Pin Config.
	//PB1 is output, all else input.
        DDRB=(1<<DDB1);
	//Clears PORTB, ensures PB1 is LOW, and there is a pull-up on PB3.
        PORTB=(0<<PB1)|(1<<PB3);//|(1<<PB4);
	//Interrupt config.
	//Enables the pin change interrupt.
        GIMSK|=(1<<PCIE);
	//Enables pin change interrupt for PB4 and PB3.
	PCMSK=/*(1<<PCINT4)|*/(1<<PCINT3);
	//Timer prescaler value of 1024, Timer freq = 8MHz/1024.
	TCCR0B=(1<<CS02)|(1<<CS00);
	//Enables interrupts.
	sei();
	//I2C init.
	USI_TWI_Master_Initialise();
	//Peripherals init.
	PCA9635_init();
	MMA7455L_init();

}

/********
start - wakes the AVR from sleep.
	Enables the PCA via enabling OE. DOES NOT wake up MMA; that is in mainloop.
********/
void start(void)
{
	//Disable the sleep flag.
	sleep_disable();
	//Clears PORTB, ensures PB1 is LOW, and there is a pull-up on PB3.
        PORTB=(0<<PB1)|(1<<PB3);//|(1<<PB4);
}

/********
start - puts the AVR to sleep.
	Calls peripheral stop functions. Powers down AVR.
********/
void stop(void)
{
	//Peripheral stops.
	PCA9635_stop();
	MMA7455L_stop();
	//Power down mode; everything off.
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	//Ensure interrupts are enables, or we can't wake.
	sei();
	//ZzZ.
	sleep_cpu();
}

/********
main - AVR mainloop
	Boots, then repeats:
		Poll MMA.
		Act on button.
		Determine colors.
		Display colors.
********/

//Start Chill Shit

//Map Function.  Rewrite, since it's copy-pasta ~chill 7.8.2010
int map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Color Write Function ~chill 7.8.2010
void color_write(char LED, char R, char G, char B)
{
    PCA_messageBuf[LED * 4 + 0] = R;      //Red
    PCA_messageBuf[LED * 4 + 1] = G;      //Green
    PCA_messageBuf[LED * 4 + 2] = B;      //Blue
}

//Color Map Function.  This shit is fucking important and shit. ~chill 7.8.2010
int color_map(int color_in,int LED)
{
    if (color_in < 85 ) {
        R = 255-(3*color_in);
        G = 3*color_in;
        B = 0; }

    if (color_in > 84 && color_in < 170 ) {
        R = 0;
        G = 255- (3 * (color_in-85));
        B = 3 * (color_in-85); }

    if (color_in > 169 && color_in < 256 ) {
        R = 3 * (color_in-170);
        G = 0;
        B = 255-(3 * (color_in-170)); }
        
    color_write(LED,R,G,B);
	return 0;
}

//End Chill Shit


void main(void)
{
        PORTB=(1<<PB1);
	//First time boot, call init.
	init();
	//Utility index for stuff that shifts over time. incremented every loop.
	unsigned char idx;
        for (idx=0x00;;idx++)
	{
		//Poll MMA.
		MMA7455L_poll();

		//Act on button.
		switch(button)
		
		{
			default:
			case BUTTON_OPEN:
				break;
			case BUTTON_PROCESSING:
				break;
			case BUTTON_PRESSED:
				//Button pressed quickly, increment our globalMode and do actions.
				//Increment globalMode
				globalMode=(globalMode+1)%11;
				//Button has been released - make sure we don't start looping globalModes quickly.
				button=BUTTON_OPEN;
				//Act on the new globalMode
				unsigned char blinkMode = BLINK_NONE;
				switch(globalMode)
				{
					case ON:
						blinkMode = BLINK_NONE;
						motionReact = 0;
						cycle = 0;
						trippy = 0;
						modeTimer=0;
						break;
					case ACCEL_OFF_24:
						blinkMode = BLINK_24;
						motionReact = 0;
						cycle = 0;
						trippy = 0;
						modeTimer=0;
						break;
					case ACCEL_OFF_190:
						blinkMode = BLINK_190;
						motionReact = 0;
						cycle = 0;
						trippy = 0;
						modeTimer=0;
						break;
					case ON_ACCEL:
						//initialize the MMA after sleeping.
						MMA7455L_init();
						blinkMode = BLINK_NONE;
						motionReact = 1;
						cycle = 0;
						trippy = 0;
						modeTimer=0;
						break;
					case ON_ACCEL_24:
						MMA7455L_init();
						blinkMode = BLINK_24;
						motionReact = 1;
						cycle = 0;
						trippy = 0;
						modeTimer=0;
						break;
					case ON_ACCEL_190:
						MMA7455L_init();
						blinkMode = BLINK_190;
						motionReact = 1;
						cycle = 0;
						trippy = 0;
						modeTimer=0;
						break;
					case CYCLE:
						MMA7455L_stop();
						blinkMode = BLINK_NONE;
						motionReact = 0;
						cycle = 1;
						trippy = 0;
						modeTimer=0;
						break;
					case CYCLE_24:
						blinkMode = BLINK_24;
						motionReact = 0;
						cycle = 1;
						trippy = 0;
						modeTimer=0;
						break;
					case CYCLE_190:
						blinkMode = BLINK_190;
						motionReact = 0;
						cycle = 1;
						trippy = 0;
						modeTimer=0;
						break;
					case BOOYAH:
						motionReact = 0;
						cycle = 0;
						trippy = 1;
						modeTimer=0;
						break;
					default:
					case OFF:
						//stop the accel and put it to sleep, as we're not using it.
						MMA7455L_stop();
						motionReact = 0;
						cycle = 0;
						trippy = 0;
						//sleep the AVR.
						stop();
						break;
				}

				PCA9635_setMode(blinkMode,0x80);
				break;
			case BUTTON_HOLDING:
				//Button is being pressed for a long time - begin incrementing the globalIndex.
				if (globalIndexSwitch == 0) {globalIndex=(globalIndex+1); }
				else 						{globalIndex2=(globalIndex2+1); }
				break;
		}
		
		char ACCEL_AVG;
		if (motionReact == 1) {
			//Stage 1: Sketchy Physics
	    	P_ACC =0;
	        THETA = 0;
	        C_ACC = 0;
	        VEL = 0;
	        VMAP = 0;
			ACCEL_AVG = Y_AVG;
			}
			else { 
				ACCEL_AVG = 0; }
				


		//Write to Data Buffer Array
		//data_buf[data_buf_pos]=VMAP;
		//data_buf_pos=(data_buf_pos+1)%N;

		//Read the array, average the array, and divide by 'N'
		//unsigned char idx;
		//short sum = 0;
		//for (idx=0;idx<N;idx++) {
		//sum = sum + data_buf[idx];
		//}
		//short ACCEL_AVG;
		//ACCEL_AVG = 1*((char)sum / N);

		//Maping Peice-Wise Function
		if (ACCEL_AVG < 21) {
			ACCEL_OFFSET = 0;
		}
		if (ACCEL_AVG > 20 && ACCEL_AVG < 101) {
			ACCEL_OFFSET = (char)(.5 * (ACCEL_AVG-20));
		}
		if (ACCEL_AVG > 100) {
			ACCEL_OFFSET = (char)(7 * (ACCEL_AVG-100) + 40);
		}

		//Color Cycle
		if (cycle == 1) {
			cycleIndex+=1;
		}
		else {
			cycleIndex =0;
		}

        //Stage 2: Color Values (integrate sub controls)
            color_1 = (ACCEL_OFFSET + cycleIndex + globalIndex) % 0xFF;
            color_2 = (ACCEL_OFFSET + cycleIndex + globalIndex2) % 0xFF;


		if (trippy == 1) {
			switch (globalMode)
				default:
				case BOOYAH:
					if (modeTimer < 20) {
						blinkMode = BLINK_24;
						WHITE1=0xFF;
						WHITE2=0xFF;
						color_write(2,0,0,0);
						color_map(color_1,1);
					}
					if (modeTimer > 19) {
						blinkMode = BLINK_NONE;
						WHITE1=0x00;
						WHITE2=0x00;
						color_write(1,0,0,0);
						color_map(color_2,2);
					}
					modeTimer+=1%30;
					PCA9635_setMode(blinkMode,0x80);

		} else {
			WHITE1=0xFF;
			WHITE2=0xFF;
			 //Map Indexs to Color Spectrum Function.  Map Motion to fucking technicolor
            color_map(color_1,1);
            color_map(color_2,2);
		}

  

		//Display colors.
		PCA9635_update();
        }
}
