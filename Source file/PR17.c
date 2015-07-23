//=========================================================================
//	Author              : Cytron Technologies
//	Project             : RFID door security
//	Project description	: Use ISD2560 chip to record and playback voice
//=========================================================================

//	include
//==========================================================================
#if defined(__XC8)
  #include <xc.h>
 #pragma config CONFIG = 0x3F32
//FOSC = EXTRC     // Oscillator Selection bits (RC oscillator)
//WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
//PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
//BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
//LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
//CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
//WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
//CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#else
#include <htc.h>                  //include the PIC microchip header file
//#include <pic.h>

//	configuration
//==========================================================================
__CONFIG (0x3F32);

#endif


//	define
//==========================================================================
#define	BUTTON1		RB0				//button 1 active low
#define	BUTTON2		RB1				//button 2 active low
#define	BUTTON3		RB2				//button 3 active low
#define	BUTTON4		RB4				//button 4 active low
#define BUTTON5		RB5				//button 5 active low
#define LED1      RA5				//led 1 active high
#define LED2      RA3				//led 2 active high
#define LED3      RA2				//led 3 active high
#define LED4      RA1				//led 4 active high
#define LED5      RA0				//led 5 active high
#define	PR  			RC3				//playback/record (low = record ; high = playback)
#define	EOM 			RC2				//end of message (pulse low at the end of each message)
#define	PD  			RC1				//power down (high to place devide in standby mode)
#define	CE        RC0				//chip enable (low to enable device)
#define	OVF       RE0				//overflow (pulse low at the end of memory array)
#define A8        RC4				//bit-8 of address pin
#define	A9        RC5				//bit-9 of address pin
#define ADD       PORTD			//bit-0 to bit-7 off address pin

//	function prototype				(every function must have a function prototype)
//==========================================================================
void delay(unsigned long data);
void record (unsigned short address);
void play (unsigned short address);
void stop (void);
void tmr_delay(unsigned short lenght);
	
//	main function					(main function of the program)
//==========================================================================
void main(void)
{
	//assign variable
	unsigned char mode;
	unsigned char message;
	unsigned char i;
	
	//set I/O input output
	TRISA = 0b11000000;					//configure PORTA I/O direction
	TRISB = 0b11111111;					//configure PORTB I/O direction
	TRISC = 0b10000100;					//configure PORTC I/O direction
	TRISD = 0b00000000;					//configure PORTD I/O direction
	TRISE = 0b00000011;					//configure PORTE I/O direction
	
	//set ADC pin to digital I/O
	ADCON1 = 0b00000110;				//set ADC pin (ANx) to I/O

	//tmr 0 configuration
	T0CS = 0;							//set timer 0 clock source to internal instruction clock cycle
	PSA = 0;							//assign prescaler to timer 0 module
	PS2 = 1;							//set timer 0 prescaler (bit 2)
	PS1 = 1;							//set timer 0 prescaler	(bit 1)
	PS0 = 1;							//set timer 0 prescaler	(bit 0)
	
	//initial condition
	LED1=0;								//off led 1
	LED2=0;								//off led 2
	LED3=0;								//off led 3
	LED4=0;								//off led 4
	LED5=0;								//off led 5
	mode='p';							//set default mode to play mode
	stop();								//stop the ISD2560
	message=0;							//set to message 0 (no active message)

	while(1)							//infinity loop
	{
		if(BUTTON1==0)					//button 1 to switch mode between play and record
		{	
			while(BUTTON1==0);			//wait button 1 to release
			if(mode=='p')           //if current mode = play mode,
			{
				mode='r';				//change to record mode
				LED1=1;					//and on led 1
			}
			else 						//else
			{		
				mode='p';				//change current mode to play mode
				LED1=0;					//and off led 1
			}
		}	
		else if(!BUTTON2)				//button 2 to select message 1
		{
			message=1;					//current active message = 1
		}	
		else if(!BUTTON3)				//button 3 to select message 2
		{
			message=2;					//current active message = 2
		}	
		else if(!BUTTON4)				//button 4 to select message 3
		{			
			message=3;					//current active message = 3
		}
		else if(!BUTTON5)				//button 5 to select message 4
		{	
			message=4;					//current active message = 4
		}			

		if(message!=0)					//if message not equal to zero, 
		{								//that means got active message
			if(mode=='p')				//if current mode is play mode,
			{
				play((message-1)*150);	//play message according to message address
			}
			else if(mode=='r')			//if current mode is record mode,
			{
				record((message-1)*150);	//record message according to message address
			}	
			for(i=30;i>0;i-=1)			//30 times loop
			{
				switch(message)			
				{
					case 1:
						LED2=!LED2;		//if message = 1, blink led 2
						break;
					case 2:
						LED3=!LED3;		//if message = 2, blink led 3
						break;
					case 3:
						LED4=!LED4;		//if message = 3, blink led 4
						break;
					case 4:
						LED5=!LED5;		//if message = 4, blink led 5
						break;
				}	
				tmr_delay(50);			//delay 500ms (blinking delay time)
			}
			message=0;					//set to no active message
			stop();						//stop the ISD2560 from play or record operation
			LED2=0;						//off led 2
			LED3=0;						//off led 3
			LED4=0;						//off led 4
			LED5=0;						//off led 5
		}
	}
}

//	functions
//==========================================================================
void delay(unsigned long data)			//short delay		
{										//delay lenght according to the given value
	for( ;data>0;data-=1);
}

void tmr_delay(unsigned short lenght)	//delay using timer 0
{										//delay lenght = given value X 10ms
	for( ;lenght>0;lenght-=1)			//loop
	{
		TMR0=0;							//clear timer 0 value
		while(TMR0<195);				//wait timer 0 value to reach 195 (10ms)
	}	
}

void record (unsigned short address)	//this function is to start record operation at the given address
{										//the address range is from 0 to 600 representing 0 to 60 second
	unsigned char temp;
	PD=0;								//activate ISD
	delay(500);							//short delay
	PR=0;								//select record mode(0=rec, 1=play)
	ADD=address;						//set the address value for bit0 to bit 7
	temp=address>>8;
	if((temp&0x01)==0) A8=0;
	else A8=1;							//set the address bit 8
	if((temp&0x02)==0) A9=0;
	else A9=1;							//set the address bit 9
	delay(100);							//short delay
	CE=0;								//start record
}

void play (unsigned short address)		//this function is to start playback operation at the given address
{										//the address range is from 0 to 600 representing 0 to 60 second
	unsigned char temp;
	PD=0;								//activate ISD
	delay(500);							//short delay
	PR=1;								//select playback mode(0=rec, 1=play)
	ADD=address;						//set the address value for bit0 to bit 7
	temp=(address>>8);
	if((temp&0x01)==0) A8=0;
	else A8=1;							//set the address bit 8
	if((temp&0x02)==0) A9=0;
	else A9=1;							//set the address bit 9
	delay(100);							//short delay
	CE=0;								//start play
}
	
void stop (void)						//this function is to stop the ISD2560 operation
{
	CE=1;								//stop operation
	delay(100);							//short delay
	PD=1;								//deactivate ISD 2560
	delay(20000);						//short delay
}

