#include "mbed.h"
#define TIME 1s
// main() runs in its own thread in the OS
int main()
{   
    DigitalOut red(D5);
    DigitalOut green(D6);
    DigitalOut D9_output(D9);
    D9_output = 1;
    while (true) {
        red = !red;  //the inital value is 0
        green = !red;
        ThisThread::sleep_for(TIME);  //keep the current situation and rest for 1s, then the next procissing
    }                                   //ThisThread:in this process    "::":
}

