#include <msp430.h>
#include <stdio.h>

// 1) NO INTERRUPTS
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	P1DIR |= BIT0; // P1.0 (LED1) as output
	P8DIR |= BIT1; // P8.1 (LED2) as output

	P1DIR &= ~BIT7; // P1.7 (S1) as input
	P1REN = BIT7;
  P1OUT |= BIT7;
	P2DIR &= ~BIT2; // P2.2 (S2) as input
	P2REN = BIT2;
	P2OUT |= BIT2;

	int s1_state = 0, s2_state = 0,
	    s1_pressed = 0, s2_pressed = 0;

	while(1)
	{
	    s1_state = (P1IN & BIT7) == 0; // pressed = GND
	    s2_state = (P2IN & BIT2) == 0; // pressed = GND

	    // LED1 (A)
	    if(s2_state && !s2_pressed) {
		    s2_pressed = 1;
		    if(!s1_state) {
		        P1OUT ^= BIT0;
		    }
		}
		else if(!s2_state && s2_pressed) {
		    s2_pressed = 0;
		}

	    // LED2 (B)
	    if(s1_state && !s1_pressed) {
	        s1_pressed = 1;
            P8OUT &= ~BIT1; // LED2 if off
	    }
	    else if(!s1_state && s1_pressed) {
            s1_pressed = 0;
            if(s2_state) {
                P8OUT |= BIT1; // LED2 is on
            }
        }

        __delay_cycles(40000);
	}
}

4.3.	Программа по управлению цифровым вводом-выводом с использования прерываний

#include <msp430.h>

int s1_pressed = 0, s2_pressed = 0;

#pragma vector = PORT1_VECTOR
__interrupt void s1_press() {
    // LED2 (B)
    if(P1IES & BIT7) { // int on falling edge (S1 is pressed)
        s1_pressed = 1;
        P8OUT &= ~BIT1;
    }
    else { // int on rising edge (S1 is unpressed)
        s1_pressed = 0;
        if(s2_pressed) {
            P8OUT |= BIT1;
        }
    }

    volatile int i = 0;
    for(i=0; i<1500; i++) {
        i++;
        i--;
    }
    P1IES ^= BIT7; // int on rising edge
    P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void s2_press() {
    // LED1 (A)
    if(P2IES & BIT2) {
        s2_pressed = 1;
        if(!s1_pressed) {
            P1OUT ^= BIT0;
        }
    }
    else {
        s2_pressed = 0;
    }

    volatile int i = 0;
    for(i=0; i<1500; i++) {
        i++;
        i--;
    }
    P2IES ^= BIT2; // change edge of int
    P2IFG &= ~BIT2;
}


int main() {
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    P1DIR |= BIT0; // P1.0 (LED1) as output
    P8DIR |= BIT1; // P8.1 (LED2) as output

    P1DIR &= ~BIT7; // P1.7 (S1) as input
    P1REN = BIT7;
    P1OUT |= BIT7;
    P2DIR &= ~BIT2; // P2.2 (S2) as input
    P2REN = BIT2;
    P2OUT |= BIT2;

   __bis_SR_register(0x0008); // enable user interrupts
   P1IE |= BIT7; // S1 en int
   P2IE |= BIT2; // S2 en int
   P1IES |= BIT7; // S1 int on falling edge
   P2IES |= BIT2; // S2 int on falling edge

   __no_operation();
}
