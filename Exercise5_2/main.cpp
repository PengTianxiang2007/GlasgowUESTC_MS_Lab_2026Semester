#include "mbed.h"

// 定义引脚，顺序对应数码管的 A, B, C, D, E, F, G, DP
BusOut display(A0,D3,D4,A2,A1,D9,D10,D11); 

int main() {
    // 任务 (iii): 重复 5 次
    for (int i = 0; i < 5; i++) {
        
        // 任务 (i): 依次显示 H, E, L, P，每个 0.2 秒
        display = 0x76; // 显示 'H'
        ThisThread::sleep_for(200ms);
        
        display = 0x79; // 显示 'E'
        ThisThread::sleep_for(200ms);
        
        display = 0x38; // 显示 'L'
        ThisThread::sleep_for(200ms);
        
        display = 0x73; // 显示 'P'
        ThisThread::sleep_for(200ms);
        
        // 任务 (ii): 然后无显示 (熄灭) 0.2 秒
        display = 0x00; 
        ThisThread::sleep_for(200ms);
    }

    // 任务 (iii): 程序停止。
    // 在嵌入式系统中，我们使用死循环让单片机保持在这个最终状态
    display = 0x00; // 确保最后是熄灭状态
    while (true) {
        ThisThread::sleep_for(1s); // 挂起程序，不做任何事情
    }
}