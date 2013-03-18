#include <thread.h>         
#include "../system/platforms/raspberry-pi/gpio.h"
void stop_blinky(){
resume(create(blinky_off(),1024,20,"OLD",0));
                
}
void blinky_off(){
GPIOMODE(16,FSEL_OUTPUT);
GPIOSET(16);

}
