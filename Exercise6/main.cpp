/* * 实验 6c: 循环输出四个恒定电压值
 * 目标电压: 0.5V, 1.0V, 2.0V, 2.5V
 * 硬件: L432KC 开发板
 * 测量: 请使用万用表测量 A4 引脚与 GND 之间的电压
 */

#include "mbed.h"

// 在模拟引脚 4 (A4) 上初始化模拟输出对象 Aout
AnalogOut Aout(A4);

int main() {
    // 映射原理: 
    // 代码值 (0.0 to 1.0) = 目标电压 / 系统参考电压 (3.3V)
    
    while (true) {
        // 1. 输出 0.5V
        // 计算: 0.5 / 3.3 ≈ 0.1515
        Aout = 0.5f / 3.3f;
        ThisThread::sleep_for(2s);

        // 2. 输出 1.0V
        // 计算: 1.0 / 3.3 ≈ 0.3030
        Aout = 1.0f / 3.3f;
        ThisThread::sleep_for(2s);

        // 3. 输出 2.0V
        // 计算: 2.0 / 3.3 ≈ 0.6061
        Aout = 2.0f / 3.3f;
        ThisThread::sleep_for(2s);

        // 4. 输出 2.5V
        // 计算: 2.5 / 3.3 ≈ 0.7576
        Aout = 2.5f / 3.3f;
        ThisThread::sleep_for(2s);
    }
}