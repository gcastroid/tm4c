/*
 * Includes
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"

/*
 * Main Function
 */
int main(void)
{
    // Set the MCU clock to 80MHz
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN|SYSCTL_SYSDIV_2_5);

    // Enable and provide a clock to the GPIO port F module
    SYSCTL_RCGCGPIO_R |= (1<<5);

    // Config Pin PF2 as output
    GPIO_PORTF_DIR_R |= (1<<2);
    GPIO_PORTF_DEN_R |= (1<<2);

    uint32_t i;
    while(1){

        // Delay
        for(i = 0; i < 1000000; i++){}
        // Set the PF2 pin
        GPIO_PORTF_DATA_R |= (1<<2);
        // Delay
        for(i = 0; i < 1000000; i++){}
        // Reset the PF2 pin
        GPIO_PORTF_DATA_R &= ~(1<<2);

    }
}
