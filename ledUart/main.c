/*
 * Includes
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"

/*
 * Function Prototypes
 */
void setup_gpio();
void setup_uart0(void);
void send_char(char c);
char get_char(void);
void send_string(char* string);

/*
 * Variables
 */
char c;

/*
 * Main Function
 */
int main(void)
{
    // Set the MCU clock to 80MHz
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN|SYSCTL_SYSDIV_2_5);

    setup_gpio(); // Config the GPIO pins
    setup_uart0(); // Config the uart0 module

    while(1){

        send_string("Enter the LED you want to turn on:\r\n");
        c = get_char();
        send_char(c);
        send_string("\r\n");

        // Turn on the LEDs depending on which char was received
        switch (c){
        case 'r':
            GPIO_PORTF_DATA_R |= (1<<1);
        break;
        case 'g':
            GPIO_PORTF_DATA_R |= (1<<3);
        break;
        case 'b':
            GPIO_PORTF_DATA_R |= (1<<2);
        break;
        default:
            GPIO_PORTF_DATA_R &= ~((1<<1)|(1<<2)|(1<<3));
        break;
        }

    }
}

/*
 * Interruption Handler Functions
 */

/*
 * Peripherals Setup Functions
 */
void setup_gpio(){

    // Enable and provide a clock to the GPIO port F module
    SYSCTL_RCGCGPIO_R |= (1<<5);

    // Config Pins PF1, PF2 and PF3 as output
    GPIO_PORTF_DIR_R |= (1<<1)|(1<<2)|(1<<3);
    GPIO_PORTF_DEN_R |= (1<<1)|(1<<2)|(1<<3);

}

void setup_uart0(void){

    // Datasheet Section 14.4

    // Step 1
    SYSCTL_RCGCUART_R |= (1<<0); // Enable UART0 Module

    // Step 2
    SYSCTL_RCGCGPIO_R |= (1<<0); // Enable GPIO port A clock (PA0 > Rx and PA1 > Tx)

    // Step 3
    GPIO_PORTA_AFSEL_R |= (1<<1)|(1<<0);

    // Step 4

    // Step 5
    GPIO_PORTA_PCTL_R |= (0x1<<4)|(0x1<<0); // Enable UART0 for pins PA0 and PA1
    GPIO_PORTA_DEN_R |= (1<<1)|(1<<0);      // Digital Enable for pins PA0, PA1

    // Step 1
    UART0_CTL_R &= ~(1<<0);  // Disable uart0

    // Step 2
    UART0_IBRD_R = 43; // BRD equation

    // Step 3
    UART0_FBRD_R = 26; // BRD fractional

    // Step 4
    UART0_LCRH_R |= (0x3<<5);  // 8 bits
    UART0_LCRH_R &= ~(1<<3);   // 1 stop bit
    UART0_LCRH_R &= ~(1<<1);   // no parity

    // Step 5
    UART0_CC_R = 0x0; // System clock as source to uart baud clock

    // Step 6

    // Step 7
    UART0_CTL_R |= (1<<9)|(1<<8)|(1<<0); // Enable UART, rx and tx

}

/*
 * General functions
 */
void send_char(char c){

    while ((UART0_FR_R & (1<<5)) != 0){
        // Wait until the data is transimitted
    }
    UART0_DR_R = c;

}

char get_char(void){

    char c;
    while ((UART0_FR_R & (1<<4)) != 0){
        // Wait until data can be received
    }
    c = UART0_DR_R;
    return c;

}

void send_string(char* string){

    while(*string){
        send_char(*(string++));
    }

}
