
#include "Pilo.h"


int main()
{


    GPIO<"/dev/gpiochip0", "PiloGPIO"> gpio{};

    gpio.add_lines<17>();

    gpio.write<17>(true);

    return 0;
}