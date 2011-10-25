//This file was created 2010-06-06 by Ian Daniher.
//This file stores register addresses for the MMA7455l Accelerometer in a useful format.

#ifndef MMA7455L_H
#define MMA7455L_H
//MMA REGISTER ADDRESSES.
#define MMA_ADDR	0x1D
#define MMA_XOUTL	0x00
#define MMA_XOUTH	0x01
#define MMA_YOUTL	0x02
#define MMA_YOUTH	0x03
#define MMA_ZOUTL	0x04
#define MMA_ZOUTH	0x05
#define MMA_XOUT8	0x06
#define MMA_YOUT8	0x07
#define MMA_ZOUT8	0x08
#define MMA_STATUS	0x09
#define MMA_DETSRC	0x0A
#define MMA_TOUT	0x0B
#define MMA_I2CAD	0x0D
#define MMA_USRINF	0x0E
#define MMA_WHOAMI	0x0F
#define MMA_XOFFL	0x10
#define MMA_XOFFH	0x11
#define MMA_YOFFL	0x12
#define MMA_YOFFH	0x13
#define MMA_ZOFFL	0x14
#define MMA_ZOFFH	0x15
#define MMA_MCTL	0x16
#define MMA_INTRST	0x17
#define MMA_CTL1	0x18
#define MMA_CTL2	0x19
#define MMA_LDTH	0x1A
#define MMA_PDTH	0x1B
#define MMA_PW	0x1C
#define MMA_LT	0x1D
#define MMA_TW	0x1E

//MODE set for Pulse Detection, allows use of all modes
//GLVL set to 8g, 16LSB/g
#define MMA_MCTL_VALUE_ON	0b0001
//MODE is standby.
#define MMA_MCTL_VALUE_OFF	0b0000

//INT1 Register is detecting Level while INT2 is detecting Pulse.
//TODO: Test correct values for CTL1 and PW.
#define MMA_CTL1_VALUE	0b010
//XPDTH(bit7) should be '0'
//1LSB  .5ms
#define MMA_PW_VALUE	0b01

#define MMA_MESSAGEBUF_SIZE 5

//Useable acceleration locations.
#define X_ACC ((char)MMA_messageBuf[1])
#define Y_ACC ((char)MMA_messageBuf[2])
#define Z_ACC ((char)MMA_messageBuf[3])

unsigned char MMA_messageBuf[MMA_MESSAGEBUF_SIZE];

//First time boot.
void MMA7455L_init(void);
//Polls for acceleration values.
void MMA7455L_poll(void);
//Sleeps the accel.
void MMA7455L_stop(void);
#endif
