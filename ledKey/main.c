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

    setup_gpio(); // Config the GPIO pins
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

    static uint8_t flag = 0;      // control which led is on
    static bool flag_pressed = 0; // 1 when sw is pressed
    static bool flag_sw = 0;      // debounce the sw
    static uint32_t counter = 0;

    // Read the PF4 pin and debounce the SW
    if (!(GPIO_PORTF_DATA_R & 0x10)) {
        if (flag_pressed == 0) {
            counter++;
        }
        if (counter > 100) {
            flag_sw = 1;
        }
    } else {
        counter = 0;
        flag_pressed = 0;
    }

    // what happens when sw1 is pressed
    switch (flag_sw) {
    case 1:
        if (flag < 7) {
            flag++;
        } else {
            flag = 0;
        }
    break;
    default:
    break;
    }

    // flag values and actions
    switch (flag){
    case 0:
        GPIO_PORTF_DATA_R &= ~((1<<1)|(1<<2)|(1<<3));
    break;
    case 1:
        GPIO_PORTF_DATA_R |= (1<<1);
        GPIO_PORTF_DATA_R &= ~((1<<2)|(1<<3));
    break;
    case 2:
        GPIO_PORTF_DATA_R |= (1<<2);
        GPIO_PORTF_DATA_R &= ~((1<<1)|(1<<3));
    break;
    case 3:
        GPIO_PORTF_DATA_R |= (1<<3);
        GPIO_PORTF_DATA_R &= ~((1<<1)|(1<<2));
    break;
    case 4:
        GPIO_PORTF_DATA_R |= (1<<1)|(1<<2);
        GPIO_PORTF_DATA_R &= ~(1<<3);
    break;
    case 5:
        GPIO_PORTF_DATA_R |= (1<<1)|(1<<3);
        GPIO_PORTF_DATA_R &= ~(1<<2);
    break;
    case 6:
        GPIO_PORTF_DATA_R |= (1<<2)|(1<<3);
        GPIO_PORTF_DATA_R &= ~(1<<1);
    break;
    case 7:
        GPIO_PORTF_DATA_R |= (1<<1)|(1<<2)|(1<<3);
    break;
    default:
        GPIO_PORTF_DATA_R &= ~((1<<1)|(1<<2)|(1<<3));
    break;
    }

    // Additional debounce logic
    if (flag_sw == 1) {
        flag_pressed = 1;
        flag_sw = 0;
        counter = 0;
    }

}

/*
 * Peripherals Setup Functions
 */
void setup_gpio(){

    // Enable and provide a clock to the GPIO port F module
    SYSCTL_RCGCGPIO_R |= (1<<5);

    // To use SW2 (PF0) uncomment this lines (Datasheet table 10.1)
    //GPIO_PORTF_LOCK_R = 0x4c4f434b;
    //GPIO_PORTF_CR_R = 0x01;

    // Config Pin PF1 as output
    GPIO_PORTF_DIR_R |= (1<<1);
    GPIO_PORTF_DEN_R |= (1<<1);

    // Config Pin PF2 as output
    GPIO_PORTF_DIR_R |= (1<<2);
    GPIO_PORTF_DEN_R |= (1<<2);

    // Config Pin PF3 as output
    GPIO_PORTF_DIR_R |= (1<<3);
    GPIO_PORTF_DEN_R |= (1<<3);

    // Config Pin PF4 as input with pull-up resistor
    GPIO_PORTF_DIR_R &= ~(1<<4);
    GPIO_PORTF_PUR_R |= (1<<4);
    GPIO_PORTF_DEN_R |= (1<<4);

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
    TIMER0_TAILR_R = 7999; // 0.1 ms

    // Step 6
    TIMER0_IMR_R |= (1<<0); //Interrupt Enable (Time-out)
    NVIC_EN0_R |= (1<<19);  //Enable interrupt Timer0

    // Step 7
    TIMER0_CTL_R |= (1<<0); // Enable the timer to start counting

    // Step 8
    // Interruption start

}
