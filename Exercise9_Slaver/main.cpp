// 将 MCU 设置为从机，并与主机交换数据，发送自己的开关位置，并显示主机的开关位置。
#include "mbed.h" 

// 硬件绑定：mosi, miso, sclk, ssel(片选输入)
SPISlave ser_port(D11, D12, D13, A3);   

DigitalOut red_led(D2);       // 从机的红灯同样接自己的 D2
DigitalOut green_led(D3);     // 从机的绿灯同样接自己的 D3

// 从机的开关同样接自己的 D4 和 D5，并开启内置下拉
DigitalIn switch_ip1(D4, PullDown); 
DigitalIn switch_ip2(D5, PullDown); 

char switch_word;   // 我们将要发送的字
char recd_val;      // 来自主机的值
 
int main() {
    while (1) {
        // 通过测试开关输入，设置要发送的字
        switch_word = 0xA0; // 设置基础暗号 1010 0000

        if (switch_ip1 == 1) 
            switch_word = switch_word | 0x01; // 开关1按下，最低位置1 
        if (switch_ip2 == 1)
            switch_word = switch_word | 0x02; // 开关2按下，次低位置1 
 
        // 检查主机有没有拉低 A3 并发送时钟
        if (ser_port.receive()) { 
            recd_val = ser_port.read();   // 读取主机刚送过来的开关状态
            ser_port.reply(switch_word);  // 把从机当前的开关状态装弹上膛，等待主机下一次读取
        } 
 
        // 根据主机传过来的暗号设置本地 LED
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