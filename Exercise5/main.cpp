#include "mbed.h"

BusOut display(A0,D3,D4,A2,A1,D9,D10,D11);	//segments A,B,C,D,E,F,G,DP
int main()
{
    while (true) {
        for (int i=0; i<10; i++){
            switch(i){
                case 0: display = 0x3F; break;
                case 1: display = 0x06; break;
                case 2: display = 0x5B; break;
                case 3: display = 0x4F; break;
                case 4: display = 0x66; break;
                case 5: display = 0x6D; break;
                case 6: display = 0x7D; break;
                case 7: display = 0x07; break;
                case 8: display = 0x7F; break;
                case 9: display = 0x6F; break;
            }
            ThisThread::sleep_for(500ms);
        }
    }
}

