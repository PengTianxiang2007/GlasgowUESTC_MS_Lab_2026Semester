#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "ds3231.h"
#include <cmath> // 用于三角函数计算

// --- 硬件初始化 ---
I2C i2c(D4, D5); // SDA, SCL
Adafruit_SSD1306_I2C oled(i2c, NC, 0x78, 64, 128); // OLED对象
DS3231 rtc(&i2c); // RTC对象 (注意传地址)

// --- 常量定义 ---
const float PI = 3.1415926535f;
const int CENTER_X = 64; // 表盘中心 X (128/2)
const int CENTER_Y = 32; // 表盘中心 Y (64/2)
const int RADIUS = 28;   // 表盘半径

/**
 * 辅助函数：根据角度和长度计算指针终点坐标
 * angle: 角度 (0度代表12点方向)
 * length: 指针长度
 */
void get_hand_coords(float angle, int length, int &x, int &y) {
    // 将角度转换为弧度，并调整 0 度指向 12 点
    float radians = (angle - 90.0f) * PI / 180.0f;
    x = CENTER_X + (int)(length * cos(radians));
    y = CENTER_Y + (int)(length * sin(radians));
}

int main() {
    oled.clearDisplay();
    oled.printf("Clock Initializing...");
    oled.display();
    thread_sleep_for(1000);

    ds3231_time_t currTime;
    int h_x, h_y, m_x, m_y, s_x, s_y;

    while (true) {
        // 1. 从 RTC 读取时间
        rtc.get_time(&currTime);

        // 2. 清除显存缓冲区（准备画新的一帧）
        oled.clearDisplay();

        // 3. 绘制模拟表盘 (外圈)
        oled.drawCircle(CENTER_X, CENTER_Y, RADIUS, WHITE);
        // 绘制刻度 (12, 3, 6, 9点)
        oled.drawPixel(CENTER_X, CENTER_Y - RADIUS + 2, WHITE); // 12点
        oled.drawPixel(CENTER_X + RADIUS - 2, CENTER_Y, WHITE); // 3点
        oled.drawPixel(CENTER_X, CENTER_Y + RADIUS - 2, WHITE); // 6点
        oled.drawPixel(CENTER_X - RADIUS + 2, CENTER_Y, WHITE); // 9点

        // 4. 计算并绘制指针
        // 秒针角度: 每秒 6 度
        get_hand_coords(currTime.seconds * 6.0f, RADIUS - 2, s_x, s_y);
        oled.drawLine(CENTER_X, CENTER_Y, s_x, s_y, WHITE);

        // 分针角度: 每分 6 度
        get_hand_coords(currTime.minutes * 6.0f, RADIUS - 8, m_x, m_y);
        oled.drawLine(CENTER_X, CENTER_Y, m_x, m_y, WHITE);

        // 时针角度: 每小时 30 度 + 分钟补偿
        float hour_angle = (currTime.hours % 12) * 30.0f + currTime.minutes * 0.5f;
        get_hand_coords(hour_angle, RADIUS - 15, h_x, h_y);
        oled.drawLine(CENTER_X, CENTER_Y, h_x, h_y, WHITE);

        // 5. 绘制数字时间 (在角落显示)
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.printf("%02d:%02d", currTime.hours, currTime.minutes);

        // 6. 推送显示 (刷新屏幕)
        oled.display();

        // 7. 稍作等待，防止 I2C 总线过载
        thread_sleep_for(200); 
    }
}