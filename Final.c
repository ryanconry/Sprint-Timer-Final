/*  Final

 	PIC used is PIC18f4620
	Compiler is C18 

	Ryan Conry 2016/04/18
*/ 

// Include the necessary device header file
#include <p18f4620.h>
#include <usart.h>


/* configuration bits for the PIC18F4620. See section 23.1 of the 18F4620 data sheet
 and the C18 help file "hlpPIC18ConfigSet" in the MCC18\doc folder.
																				*/
#pragma config OSC = INTIO7
                
#pragma config WDT = OFF		// Watchdog off
#pragma config PWRT = OFF              //Power Up Timer: disabled
#pragma config BOREN = OFF               //Brown Out Reset: disabled
#pragma config STVREN = OFF              //Stack Overflow Reset:Disabled
#pragma config LVP = OFF               //Low Voltage ICSP:Disabled

#pragma code high_vector=0x08

//Global Variable
int count=0;
int A,C =0;
int B,D= 1;
int t2flag=1;
int h=0;
int t=0;
int s=0;
int beam=0;
int stop=0;
    
//Function prototypes
void init_io(void);					//initialise the PIC IC				
void Initialise_Timer2(void);		//initialise Timer 2
void Initialise_Timer0(void);		//initialise Timer 0
void init_usart(void);				//initialise usart
void tx_data(void);
void rx_data(void);
unsigned char convert(int);			//convert int to char
void Timer0_delay(void);			//Timer0 delay
void timer2_ISR(void);




//code required to service the interrupt. Timer0_ISR is the interrupt service 
//routine for this particular interrupt
void interrupt_at_high_vector(void)
{
	 _asm goto timer2_ISR _endasm
}


void main(void)
	{

		init_io();					//initialize ports
		Initialise_Timer2();		//initialize timer2 for sampling
		LATCbits.LATC0=0;			//All LEDs initially off
		LATCbits.LATC1=0;			
		LATCbits.LATC2=0;		

	while (1)
		{	
									//Timer will interrupt here
		} 
		
	
	}								//end main function


//This function initialises the Ports on the PIC IC as either inputs or output. Also enables interrupts
void init_io(void){
	TRISA = 0b11111111;					
	TRISB = 0b11111111;					
	TRISC = 0b11111000;				//RC0=green,RC1=yellow,RC2=red
	TRISD = 0b11111111;				//RD1 reads sq wave=input
	TRISE = 0xF;
	
	OSCCONbits.IRCF2=1;				//postscalar for internal oscillator 110 => 4Mhz
	OSCCONbits.IRCF1=1;
	OSCCONbits.IRCF0=0;	
}

//this function initialises timer 2 

void Initialise_Timer2(void)
{

	T2CON=0b00000100;
	PR2=250;
	PIR1bits.TMR2IF=0;			//Clear interrupt flag
	PIE1bits.TMR2IE = 1;		//Enable interrupts for timer2
 	INTCONbits.PEIE = 1;		//Enable peripheral interrupts
	INTCONbits.GIE = 1;			//enable global interrupts
	T2CONbits.TMR2ON = 1;		//Turn on timer2
}


//this function initialises timer 0

void Initialise_Timer0(void)
{
	TMR0H = 0xFF;					// FFD9=0.01, FE79=0.1,F0BD=1
	TMR0L = 0xD9;					
	T0CON = 0b00000111;				// Timer0 Control Register. MSB turns on time0. Bit 7 selects it as 16 bit timer.
									// bit 6 set timer 0 as timer as opposed to counter
									// bit 5 source or trailing edge
									// bit 4 specify that you intend to use pre-scaler
									// timer period is clock period * pre-scaler
									// bits 3 - 1 set pre-scaler value of 256
	 								// in this case 111 equates to pre-scaler of 256
	INTCON = 0b10100100;			//Interrupt control register. Bit 8 enables interrupts. 
									//bit 6 is timer 0 enable
									//bit 3 is timer 0 overflow flag. Must be cleared after each interrupt
}

void init_usart(void){
	
	SPBRG=25;					//get a baud rate of 9600 with 4MHz clock
	TXSTA=0b00100100;			//initialize Tx 
	RCSTA=0b10010000;			//initialize Rx
	TXREG=0x0;
}


//Timer0 delay function
void Timer0_delay(void)
{
	TMR0H = 0xFF;					// high and low byte of 16bit timer register
	TMR0L = 0xD9;
	T0CONbits.TMR0ON = 1; 			// Turn ON Timer 0

	while(!INTCONbits.TMR0IF);		//wait for Timer0 interrupt

		h=h+1;
		if(h==10){
			t=t+1;
			h=0;}
		if(t==10){
			s=s+1;
			t=0;
		}	
		if(s==10){
			s=0;}

		if(PORTCbits.RC3==0)		//runner broke beam
		{
			beam=beam+1;
			if(beam==10)
			{
				stop=1;

				if(t>0)
					t=t-1;
				else
					t=9;

				T0CONbits.TMR0ON = 0; // Turn OFF Timer 0
			}
		}

		if(PORTCbits.RC3==1)
			stop=0;

	INTCONbits.TMR0IF=0;			//clear Timer0 interrupt flag	
}

void tx_data(void)
{

	unsigned char h1;
	unsigned char t1;
	unsigned char s1;

	h1=convert(h);
	t1=convert(t);
	s1=convert(s);


    TXREG=s1;                                     // Store seconds data in Transmit register
	while(!PIR1bits.TXIF);                       // Wait until TXIF goes low
	TXREG='.'; 									
	while(!PIR1bits.TXIF);
	TXREG=t1;                                     // Store tenths data in Transmit register
    while(!PIR1bits.TXIF);
	TXREG=h1;                                     // Store hundredths data in Transmit register
    while(!PIR1bits.TXIF);

}


void rx_data(void)
{
    while(!PIR1bits.RCIF);                            // Wait until RCIF gets low
}


unsigned char convert(int val){

	unsigned char conv;

	if(val==1){
		conv='1';
	}
	if(val==2){
		conv='2';
	}
	if(val==3){
		conv='3';
	}
	if(val==4){
		conv='4';
	}
	if(val==5){
		conv='5';
	}
	if(val==6){
		conv='6';
	}
	if(val==7){
		conv='7';
	}
	if(val==8){
		conv='8';
	}
	if(val==9){
		conv='9';
	}
	if(val==0){
		conv='0';
	}


	return conv;
}


#pragma interrupt timer2_ISR
void timer2_ISR(void)
{
if(T2CONbits.TMR2ON)
{
		if(count==0)
		{
			A=PORTDbits.RD1;		//take first 250us sample
		}
		if(count==1)
		{	
			B=PORTDbits.RD1;		//take second 250us sample
		}
		if(count==2)
		{
			C=PORTDbits.RD1;		//take third 250us sample
		}
		if(count==3)
		{
			D=PORTDbits.RD1;		//take fourth 250us sample
		}
	
		count++;
	
		if(count==4)			//if 1ms has passed...
		{count=0;				//restart sampling
		}
	
}

if(A!=B || A!=C || A!=D || B!=C || B!=D || C!=D)
{			
	LATCbits.LATC0=1;			//Green LED on, runner at block 
	
}

if(A==B && A==C && A==D && B==C && B==D && C==D)
{
	if(t2flag==1)
	{
		LATCbits.LATC0=0;			//green led off, runner left
		LATCbits.LATC1=1;			//yellow LED on
		t2flag=0;					//flag needed so timer2 ISR doesnt restart
		PIE1bits.TMR2IE = 0;		//Disable interrupts for timer2
		T2CONbits.TMR2ON = 0;		//Turn off timer2
		Initialise_Timer0();		//Timer0 initialisation
	}


	while(!stop)
	{

		Timer0_delay();				//the timer being started

	}

	if(stop)
		{
		LATCbits.LATC1=0;			//yellow LED off
		LATCbits.LATC2=1;			//Red LED on, runner stopped
		init_usart();
		rx_data();
		tx_data();
		while(1);
		
		}		
				
}

PIR1bits.TMR2IF=0;			//Clear interrupt flag
}