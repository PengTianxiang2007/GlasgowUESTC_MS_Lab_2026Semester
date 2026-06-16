#include "mbed.h"

// L432KC 的 DAC1_OUT1 位于 PA_4 引脚
AnalogOut dac_out(A4);

int main() {
    // --- 实验参数配置 ---
    const int STEPS = 500;            // 定义一个周期内包含 500 个阶梯
    const float V_REF = 3.3;    // 硬件参考电压
    const float V_TARGET = 3.0; // 我们想要达到的最大目标电压
    const float MAX_V_RATIO = V_TARGET / V_REF; // 对应 3.0V (3.0/3.3)
    const int STEP_DELAY_US = 20;     // 每步延时 20us，总计 10ms (100Hz)
    const float increment = MAX_V_RATIO / STEPS;

    while (true) {
        // 使用 for 循环生成锯齿波的“爬升”部分
        for (int i = 0; i < STEPS; i++) {
            // 写入当前电压比例
            dac_out.write(increment * i);
            
            // 维持当前电压的时间，控制波形频率
            wait_us(STEP_DELAY_US);
        }
        // 当循环结束，i 归零，电压瞬间从 3.0V 跳回 0V，形成锯齿形状
    }
}