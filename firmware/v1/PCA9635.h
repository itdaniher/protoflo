//This file was created 2010-06-06 by Ian Daniher.
//This file stores register addresses for the PCA9635 PWM chip in a useful format.
//Awk rocks.
#ifndef PCA9635
#define PCA9635

//PCA9635 Register Definitions.
#define PCA_MODE1	0x00
#define PCA_MODE2	0x01
#define PCA_PWM0	0x02
#define PCA_PWM1	0x03
#define PCA_PWM2	0x04
#define PCA_PWM3	0x05
#define PCA_PWM4	0x06
#define PCA_PWM5	0x07
#define PCA_PWM6	0x08
#define PCA_PWM7	0x09
#define PCA_PWM8	0x0A
#define PCA_PWM9	0x0B
#define PCA_PWM10	0x0C
#define PCA_PWM11	0x0D
#define PCA_PWM12	0x0E
#define PCA_PWM13	0x0F
#define PCA_PWM14	0x10
#define PCA_PWM15	0x11
#define PCA_GRPPWM	0x12
#define PCA_GRPFREQ	0x13
#define PCA_LEDOUT0	0x14
#define PCA_LEDOUT1	0x15
#define PCA_LEDOUT2	0x16
#define PCA_LEDOUT3	0x17
#define PCA_SUBADR1	0x18
#define PCA_SUBADR2	0x19
#define PCA_SUBADR3	0x1A

//PCA allcall address - using this to talk I2C.
#define PCA_ALLCALL_ADDR	0x70

/*PCA MessageBuf contains the entire I2C output bytestring to control the PCA*/
/*ALL VALUES IN PCA_messageBuf SHOULD BE CONSIDERED VOLATILE EXCEPT FOR PCA_messageBuf[4:11], THE LED PWM VALUES*/
#define PCA_MESSAGEBUF_SIZE 26
unsigned char PCA_messageBuf [PCA_MESSAGEBUF_SIZE];

//Functional LED locations.
#define RGB1R PCA_messageBuf[4]
#define RGB1G PCA_messageBuf[5]
#define RGB1B PCA_messageBuf[6]
#define WHITE1 PCA_messageBuf[7]
#define RGB2R PCA_messageBuf[8]
#define RGB2G PCA_messageBuf[9]
#define RGB2B PCA_messageBuf[10]
#define WHITE2 PCA_messageBuf[11]

//First time PCA boot - taks I2C, sets all registers.
void PCA9635_init(void);

//Define PCA generation mode.
#define BLINK_NONE 0x00 //No blink
#define BLINK_24   0x01 //24 Hz
#define BLINK_190  0x02 //190 Hz
unsigned char currentBlinkMode;
void PCA9635_setMode(unsigned char blinkMode, unsigned char blinkPWM);

//Sends a smaller I2C control string to update LED values.
void PCA9635_update(void);
//Sleeps the PCA.
void PCA9635_stop(void);
#endif
