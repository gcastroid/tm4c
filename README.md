# TM4C-CCS

This repo contains some projects for the TivaC Launchpad. 

# Description

All the functions were developed at low level in the C language except the clock frequency setting which was made using the TivaWare library. The IDE used is the Code Composer Studio. The TivaC Launchpad (EK-TM4C123GXL) has a ARM Cortex M4F MCU that can run at 80MHz. 

[blink:](https://github.com/gcastroid/TM4C-CCS/tree/main/blink) 

That's the hello world of embedded systems, a simple blink code made with for loops to generate the delays.

[blink w/ timer:](https://github.com/gcastroid/TM4C-CCS/tree/main/blinkTimer) 

This project simply blink the LED at a determined frequency using a Timer Interruption.

[led brightness control:](https://github.com/gcastroid/TM4C-CCS/tree/main/ledAdcPwm) 

Here the brightness of a LED is controlled with a potentiometer. The pot value is read by the ADC and the value is converted to duty cycle to control the LED with the PWM. Pin PE3 is the ADC input pin and pin PB6 is the PWM output pin.

[led selection:](https://github.com/gcastroid/TM4C-CCS/tree/main/ledKey) 

This project uses the board key to control which led is on. The timer interrupt is used to debounce the switch, and each time that it's pressed selects from eight options (R on, G on, B on, RG on, RB on, GB on, RGB on and all leds off).

[uart com w/ PC:](https://github.com/gcastroid/TM4C-CCS/tree/main/ledUart) 

The UART module is configured so the MCU can communicate with a PC. If the user sends 'r' the red led turns on, 'g' the green led turns on and 'b' the blue led turns on, in any other case each led turns off.
