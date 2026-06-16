#include "mbed.h"

// 假设 D3 连接到 74HC4066 的 1E (引脚 13)
DigitalOut switch_control(D3); 
// 假设 D5 连接到一个物理开关，用来切换频率
DigitalIn freq_selector(D5);

int main() {
    while(1) {
        if(freq_selector == 1) {     
            // 200Hz -> 周期 5ms (5000us) -> 高低电平各 2500us
            switch_control = 1;
            wait_us(2500);
            switch_control = 0;
            wait_us(2500);
        }
        else {                  
            // 500Hz -> 周期 2ms (2000us) -> 高低电平各 1000us
            switch_control = 1;
            wait_us(1000);
            switch_control = 0;
            wait_us(1000);
        }
    }
}