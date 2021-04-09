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
void setup_adc0();
void setup_pwm();

float conv_to_percent(int value);
void set_duty(float duty);

/*
 * Variables
 */
int adc_read = 0;
int adc_percent = 0;

/*
 * Main Function
 */
int main(void)
{
    // Set the MCU clock to 80MHz
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN|SYSCTL_SYSDIV_2_5);

    setup_gpio(); // Config the GPIO ports
    setup_timer0(); // Config the timer0 module
    setup_adc0(); // Config the adc0 module
    setup_pwm(); // Config the pwm module

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

void adc0ss3_handler(void){

    // Read the ADC value
    adc_read = ADC0_SSFIFO3_R;

    // Output to PWM
    adc_percent = conv_to_percent(adc_read);
    set_duty(adc_percent);

    // Interrupt event
    ADC0_ISC_R |= (1<<3); // Clear bit IN3

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
    TIMER0_TAILR_R = 79999; // 0.1 ms

    // Step 6
    TIMER0_IMR_R |= (1<<0); //Interrupt Enable (Time-out)
    NVIC_EN0_R |= (1<<19);  //Enable interrupt Timer0

    // Step 7
    TIMER0_CTL_R |= (1<<0); // Enable the timer to start counting

    // Step 8
    // Interruption start

}

void setup_adc0(){

    // Datasheet section 13.4.1

    // Step 1
    SYSCTL_RCGCADC_R |= (1<<0); // Enable clock to ADC0 module

    // Step 2
    SYSCTL_RCGCGPIO_R |= (1<<4); // Enable clock GPIO E
    GPIO_PORTE_DIR_R &= ~(1<<3); // PE3 Input

    // Step 3
    GPIO_PORTE_AFSEL_R |= (1<<3); // Using PE3

    // Step 4
    GPIO_PORTE_DEN_R &= ~(1<<3); // Clear PE3 DEN

    // Step 5
    GPIO_PORTE_AMSEL_R |= (1<<3); // Enable analog functions PE3

    // Step 6
    ADC0_ACTSS_R &= ~(1<<3);

    // Datasheet section 13.4.2

    // Step 1

    // Step 2
    ADC0_EMUX_R |= (0x5<<12); // Timer is the trigger
    TIMER0_CTL_R |= (1<<5);   // Timer0A trigger

    // Step 3

    // Step 4
    ADC0_SSMUX3_R = 0; // Analog input 0 (Se usasse AI2 escreve 2 no registrador)

    // Step 5
    ADC0_SSCTL3_R |= (1<<2)|(1<<1);

    // Step 6
    ADC0_IM_R |= (1<<3);

    // Step 7
    ADC0_ACTSS_R |= (1<<3); // Enable the SS3

    // Interrupt
    ADC0_ISC_R |= (1<<3);   // Interruption SS3
    NVIC_EN0_R |= (1<<17);  // Enable interrupt adc0ss3

}

void setup_pwm(){

    // Datasheet section 20.4

    // Step 1
    SYSCTL_RCGCPWM_R |= (1<<0); // Enable PWM0 clock

    // Step 2
    SYSCTL_RCGCGPIO_R |= (1<<1); // Enable GPIOB clock

    // Step 3
    GPIO_PORTB_DEN_R |= (1<<6);   // PB6 digital enable
    GPIO_PORTB_AFSEL_R |= (1<<6); // PB6 M0PWM0

    // Step 4
    GPIO_PORTB_PCTL_R &= ~(0x4<<24); // Clear
    GPIO_PORTB_PCTL_R |= (0x4<<24);  // PMC6 - PB6

    // Step 5
    SYSCTL_RCC_R &= ~(1<<20); // System clock as PWM clock source
    //SYSCTL_RCC_R &= ~((1<<17)|(1<<18)|(1<<19)); // Clear the bits
    //SYSCTL_RCC_R |= (0x0<<17);  // SysClock divided by 2
    //SYSCTL_RCC_R |= (0x2<<17);  // SysClock divided by 8

    // Step 6
    PWM0_0_CTL_R = 0x00000000;
    PWM0_0_GENA_R |= (1<<7)|(1<<3)|(1<<2); // 0x8C
    //PWM0_0_GENB_R = 0x0000080C;

    // Step 7
    // 80mhz >>>> 50k >>> 1600
    PWM0_0_LOAD_R = 1599; // 50kHz PWM frenquency

    // Step 8
    PWM0_0_CMPA_R = 799; // duty 50%

    // Step 9
    //PWM0_0_CMPB_R = 99;

    // Step 10
    PWM0_0_CTL_R |= (1<<0);

    // Step 11
    PWM0_ENABLE_R = (1<<0);

}

/*
 * General functions
 */
float conv_to_percent(int value){

    float a;
    a = (value/4095.)*100;
    return a;

}

void set_duty(float duty){

    int value;
    value = ((100-duty)/100.)*1598;
    PWM0_0_CMPA_R = value; // set the pwm duty cycle

}
