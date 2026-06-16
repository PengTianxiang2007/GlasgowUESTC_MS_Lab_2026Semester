#include "mbed.h"
#define TIME 500
// main() runs in its own thread in the OS
int main()
{
    DigitalOut wave(D3);
    while (true) {
        wave = !wave;
        wait_us(TIME);
    }
}

