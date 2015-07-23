//=========================================================================
//	Author				: Cytron Technologies		
//	Project				: RFID door security
//	Project description	: Use ISD2560 chip to record and playback voice
//=========================================================================

//	include
//==========================================================================
#include <pic.h> 					//include pic library

//	configuration
//==========================================================================
__CONFIG ( 0x3F32 );				//microcontroller configuration	

//	define
//==========================================================================
#define	button1		RB0				//button 1 active low
#define	button2		RB1				//button 2 active low				
#define	button3		RB2				//button 3 active low				
#define	button4		RB4				//button 4 active low
#define button5		RB5				//button 5 active low
#define led1		RA5				//led 1 active high
#define led2		RA3				//led 2 active high
#define led3		RA2				//led 3 active high
#define led4		RA1				//led 4 active high
#define led5		RA0				//led 5 active high
#define	pr			RC3				//playback/record (low = record ; high = playback)
#define	eom			RC2				//end of message (pulse low at the end of each message)
#define	pd			RC1				//power down (high to place devide in standby mode)
#define	ce			RC0				//chip enable (low to enable device)
#define	ovf			RE0				//overflow (pulse low at the end of memory array)
#define a8			RC4				//bit-8 of address pin
#define	a9			RC5				//bit-9 of address pin
#define add			PORTD			//bit-0 to bit-7 off address pin

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
	led1=0;								//off led 1
	led2=0;								//off led 2
	led3=0;								//off led 3
	led4=0;								//off led 4
	led5=0;								//off led 5
	mode='p';							//set default mode to play mode
	stop();								//stop the ISD2560
	message=0;							//set to message 0 (no active message)

	while(1)							//infinity loop
	{
		if(button1==0)					//button 1 to switch mode between play and record
		{	
			while(button1==0);			//wait button 1 to release
			if(mode=='p')				//if current mode = play mode,
			{
				mode='r';				//change to record mode
				led1=1;					//and on led 1
			}
			else 						//else
			{		
				mode='p';				//change current mode to play mode
				led1=0;					//and off led 1
			}
		}	
		else if(!button2)				//button 2 to select message 1
		{
			message=1;					//current active message = 1
		}	
		else if(!button3)				//button 3 to select message 2
		{
			message=2;					//current active message = 2
		}	
		else if(!button4)				//button 4 to select message 3	
		{			
			message=3;					//current active message = 3
		}
		else if(!button5)				//button 5 to select message 4
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
						led2=!led2;		//if message = 1, blink led 2
						break;
					case 2:
						led3=!led3;		//if message = 2, blink led 3
						break;
					case 3:
						led4=!led4;		//if message = 3, blink led 4
						break;
					case 4:
						led5=!led5;		//if message = 4, blink led 5
						break;
				}	
				tmr_delay(50);			//delay 500ms (blinking delay time)
			}
			message=0;					//set to no active message
			stop();						//stop the ISD2560 from play or record operation
			led2=0;						//off led 2
			led3=0;						//off led 3
			led4=0;						//off led 4
			led5=0;						//off led 5
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
	pd=0;								//activate ISD
	delay(500);							//short delay
	pr=0;								//select record mode(0=rec, 1=play)
	add=address;						//set the address value for bit0 to bit 7
	temp=address>>8;
	if((temp&0x01)==0) a8=0;		
	else a8=1;							//set the address bit 8
	if((temp&0x02)==0) a9=0;
	else a9=1;							//set the address bit 9
	delay(100);							//short delay
	ce=0;								//start record	
}

void play (unsigned short address)		//this function is to start playback operation at the given address
{										//the address range is from 0 to 600 representing 0 to 60 second
	unsigned char temp;
	pd=0;								//activate ISD
	delay(500);							//short delay
	pr=1;								//select playback mode(0=rec, 1=play)
	add=address;						//set the address value for bit0 to bit 7
	temp=(address>>8);
	if((temp&0x01)==0) a8=0;
	else a8=1;							//set the address bit 8
	if((temp&0x02)==0) a9=0;
	else a9=1;							//set the address bit 9
	delay(100);							//short delay
	ce=0;								//start play
}
	
void stop (void)						//this function is to stop the ISD2560 operation
{
	ce=1;								//stop operation
	delay(100);							//short delay
	pd=1;								//deactivate ISD 2560
	delay(20000);						//short delay
}

