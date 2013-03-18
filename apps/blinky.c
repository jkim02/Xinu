#include "../system/platforms/raspberry-pi/gpio.h"
#include <thread.h>
void run_blinky(){
    blinky_on();
}

void blinky_on(){
    GPIOMODE(LED_PIN, FSEL_OUTPUT);
    GPIOCLR(LED_PIN);
}




