// 将 MCU 设置为主机，并与从机交换数据，发送自己的开关位置，并显示从机的开关位置。
#include "mbed.h" 

SPI ser_port(D11, D12, D13); // 硬件绑定：mosi, miso, sclk
DigitalOut red_led(D2);       // 红灯接 D2
DigitalOut green_led(D3);     // 绿灯接 D3
DigitalOut cs(A3);           // 总开关（片选）接 A3

// 使用 PullDown 模式，防止引脚悬空噪声导致灯闪烁
DigitalIn switch_ip1(D4); 
DigitalIn switch_ip2(D5); 

char switch_word;  // 我们将要发送的字
char recd_val;     // 从从机返回的值
 
int main() {
    while (1) {
        // 选择 SPI 主机默认设置，无需进一步配置
        // 通过测试开关输入，设置要发送的字 
        switch_word = 0xA0; // 设置基础暗号 1010 0000

        if (switch_ip1 == 1) 
            switch_word = switch_word | 0x01; // 开关1按下，最低位置1
        if (switch_ip2 == 1)
            switch_word = switch_word | 0x02; // 开关2按下，次低位置1 

        cs = 0; // 闭合总开关：选中从机 
        recd_val = ser_port.write(switch_word); // 发送主机的状态，同时挤回从机的状态 
        cs = 1; // 断开总开关：结束通信 

        ThisThread::sleep_for(10ms);

        // 根据从机传回的暗号设置本地 LED
        red_led = 0;     // 预设两者都为 0
        green_led = 0; 
        recd_val = recd_val & 0x03; // 强行滤掉高位，只保留低两位

        if (recd_val == 1) 
            red_led = 1;
        if (recd_val == 2) 
            green_led = 1;
        if (recd_val == 3) { 
            red_led = 1; 
            green_led = 1; 
        } 
    } 
}