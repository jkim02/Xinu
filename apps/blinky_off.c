#include <thread.h>         
#include "../system/platforms/raspberry-pi/gpio.h"
void stop_blinky(){
    blinky_off();
}

void blinky_off(){
    GPIOMODE(LED_PIN, FSEL_OUTPUT);
    GPIOSET(LED_PIN);
}
