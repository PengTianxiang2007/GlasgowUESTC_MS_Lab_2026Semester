#include "mbed.h"

// 定义输入和输出
AnalogIn pot(A6);
DigitalOut led1(D2), led2(D3), led3(D4), led4(D5);

int main() {
    while(1) {
        float val = pot.read(); // 读取 0.0 到 1.0 之间的值
        float voltage = val * 3.3f; // 还原成实际电压进行判断

        // 根据电压值控制 LED
        if (voltage < 0.6f) {
            led1 = 0; led2 = 0; led3 = 0; led4 = 0;
        } else if (voltage < 1.2f) {
            led1 = 1; led2 = 0; led3 = 0; led4 = 0;
        } else if (voltage < 1.8f) {
            led1 = 1; led2 = 1; led3 = 0; led4 = 0;
        } else if (voltage < 2.4f) {
            led1 = 1; led2 = 1; led3 = 1; led4 = 0;
        } else {
            led1 = 1; led2 = 1; led3 = 1; led4 = 1;
        }
        
        ThisThread::sleep_for(50ms); // 稍微延迟，防止过度刷新
    }
}