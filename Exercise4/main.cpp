#include "mbed.h"
#define TIME 1s

int main()
{
    DigitalOut S1(D5);
    DigitalOut S0(D6);
    while (true) {
        S1 = 0;
        S0 = 0;
        ThisThread::sleep_for(TIME);
        S1 = 0;
        S0 = 1;
        ThisThread::sleep_for(TIME);
        S1 = 1;
        S0 = 0;
        ThisThread::sleep_for(TIME);
        S1 = 1;
        S0 = 1;
        ThisThread::sleep_for(TIME);
    }
}

