//****************************************
//ROBOTTIAUTON OHJAUS MSP430G2553
//TOIMIVA VERSIO ISOLLA RADALLA 3.4.2012
//
//TEEMU KONTIO       14.03.2013
//Jyvaskylan ammattikorkeakoulu
//IIO12s1
//
//P1.2 - MOOTTORI OIKEA (BIT2)
//P2.2 - MOOTTORI VASEN (BIT2)
//P1.5 - ANTURI VASEN   (BIT5)
//P2.0 - ANTURI OIKEA   (BIT0)
//P2.1 - ANTURI KESKI   (BIT1)
//****************************************

#include "msp430G2553.h"
unsigned short temp = 0;

void main(void)
{  
	char last = 0;
	WDTCTL = WDTPW + WDTHOLD;      //WATCHDOG POIS
	P1DIR |= BIT2+BIT0+BIT6;       //VASEN MOOTTORI (1.2 OUT)
	P2DIR |= BIT2;                 //OIKEA MOOTTORI (2.2 OUT)
	
	P1OUT = 0x00;                  //PYSÄYTÄ VASEN MOOTTORI
	P2OUT = 0x00;                  //PYSÄYTÄ OIKEA MOOTTORI
	
	TA0CTL = TASSEL_2 + MC_1;      //TIMER - VASEN MOOTTORI
	TA0CCR0 = 0xFF;                //TIMER YLARAJA
	TA0CCTL1 = OUTMOD_6;           //PWM TOGGLE/SET
	P1SEL |= BIT2;                 //TIMER OHJAA PINNIÄ 1.2
	
	TA1CTL = TASSEL_2 + MC_1;      //TIMER - OIKEA MOOTTORI
	TA1CCR0 = 0xFF;                //TIMER YLARAJA
	TA1CCTL1 = OUTMOD_6;           //PWM TOGGLE/SET
	P2SEL |= BIT2;                 //TIMER OHJAA PINNIÄ 2.2
	
	ADC10CTL1 = INCH_10 + ADC10DIV_3;                      // Temp Sensor ADC10CLK/4
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
	ADC10CTL0 |= ENC + ADC10SC;                            // Sampling and conversion start
	
	while ( ADC10CTL1 & ADC10BUSY ) 		       // Wait for ADC to complete
	{                       
	}
	
	temp = ADC10MEM; 


	while(temp+2 >= ADC10MEM)
	{  
		ADC10CTL0 |= ENC + ADC10SC;                     // Sampling and conversion start
   		for(short i=0;i<15000;i++)			// Delayed start
   		{			
  		}
  		P1OUT ^= BIT6;
  	}
  	P1OUT = BIT0;

	while(1)
	{
    		//TARKISTETAAN ONKO YKSIKAAN ANTURI MUSTALLA
  		if((P1IN & BIT5) || (P2IN & BIT0) || (P2IN & BIT1)){
    
			  //VASEN -ANTURI
			  if((P1IN & BIT5))
			  {
	                        last = 1;
				TA1CCR1 = 0xFF;    //OIKEA MOOTTORI TÄYSILLE (säätö:00-FF)
				TA0CCR1 = 0x7F;    //5F VASEN MOOTTORI POIS (säätö: 00-FF)
			  }
			  
			  //OIKEA -ANTURI
			  if((P2IN & BIT0))
			  {
	                        last = 2;
				TA0CCR1 = 0xFF;    //VASEN MOOTTORI TÄYSILLE (säätö: 00-FF)
				TA1CCR1 = 0x70;    //5F OIKEA MOOTTORI POIS (säätö: 00-FF)
			  }
			  
			  //KESKI -ANTURI
			  if((P2IN & BIT1))
			  {
	                  	last = 0;
			  	TA0CCR1 = 0xFF;      //0x90 VASEN MOOTTORI PAALLA
			  	TA1CCR1 = 0xFF;      //0x90 OIKEA MOOTTORI KÄY
			  }
    		}
     		else{
		       do{
		         	if(last == 1)
		         	{
		        	 	TA1CCR1 = 0xFF;    //OIKEA MOOTTORI TÄYSILLE (säätö:00-FF)
			 		TA0CCR1 = 0x50;    //5F VASEN MOOTTORI POIS (säätö: 00-FF)
		        	 }
		         
		         	if(last == 2)
		         	{
		        		TA0CCR1 = 0xFF;    //VASEN MOOTTORI TÄYSILLE (säätö: 00-FF)
			 		TA1CCR1 = 0x50;    //5F OIKEA MOOTTORI POIS (säätö: 00-FF)
		         	}
		       }while(!(P2IN & BIT1));
		 }
	}
}
