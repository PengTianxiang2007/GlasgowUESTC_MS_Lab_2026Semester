#include "mbed.h"

AnalogOut Aout(A4); 
AnalogIn Ain(A6);

int main() {
    while (1) {
        Aout = Ain; 
    }
}