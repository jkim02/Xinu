#include "../system/platforms/raspberry-pi/gpio.h"
#include <thread.h>
void run_blinky(){
resume(create(blinky_on(),1024,20,"OVERHOLT",0));
}

void blinky_on(){
GPIOMODE(16,FSEL_OUTPUT);
GPIOCLR(16);
}




