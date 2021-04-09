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
void setup_timer0();

/*
 * Main Function
 */
int main(void)
{
    // Set the MCU clock to 80MHz
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN|SYSCTL_SYSDIV_2_5);

    setup_gpio(); // Config the GPIO ports
    setup_timer0(); // Config the timer0 module

    while(1){

    }
}

/*
 * Interruption Handler Functions
 */
void timer0_handler(void){

    // Step 8
    TIMER0_ICR_R |= (1<<0); //Clear Bits TATORIS and TATOMIS

    // Code to be executed at each interruption

    static bool flag_led = 0;

    // Blink the led
    if(flag_led == 0){
        GPIO_PORTF_DATA_R |= (1<<2);
        flag_led = 1;
    }
    else{
        GPIO_PORTF_DATA_R &= ~(1<<2);
        flag_led = 0;
    }

}

/*
 * Peripherals Setup Functions
 */
void setup_gpio(){

    // Enable and provide a clock to the GPIO port F module
    SYSCTL_RCGCGPIO_R |= (1<<5);

    // Config Pin PF2 as output
    GPIO_PORTF_DIR_R |= (1<<2);
    GPIO_PORTF_DEN_R |= (1<<2);
}

void setup_timer0(){

    // Datasheet Section 11.4.1 Periodic Timer Mode

    // Enable timer 0 clock
    SYSCTL_RCGCTIMER_R |= (1<<0);

    // Step 1
    TIMER0_CTL_R &= ~(1<<0);

    // Step 2
    TIMER0_CFG_R = 0x0;

    // Step 3
    TIMER0_TAMR_R &= ~(0x3<<0); // Clear the 0:1 bit field
    TIMER0_TAMR_R |= (0x2<<0); // Periodic

    // Step 4
    TIMER0_TAMR_R &= ~(1<<4); // Counts down
    TIMER0_TAMR_R |= (1<<5); // Enable interrupt

    // Step 5
    // T(timer period) = N / F(MCU Freq.)
    // N - 1 = Value to load in the register
    TIMER0_TAILR_R = 79999999; // 1 s

    // Step 6
    TIMER0_IMR_R |= (1<<0); //Interrupt Enable (Time-out)
    NVIC_EN0_R |= (1<<19);  //Enable interrupt Timer0

    // Step 7
    TIMER0_CTL_R |= (1<<0); // Enable the timer to start counting

    // Step 8
    // Interruption start

}
