#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "ds3231.h"
#include <cmath> 

// =================================================================
// --- 1. I2C 总线初始化 (专门伺候 DS3231 时钟芯片) ---
// Mbed OS 构造顺序为 (SDA, SCL)。对照 Pinout 图：D4=I2C1_SDA, D5=I2C1_SCL
I2C i2c(D4, D5); 
DS3231 rtc(&i2c); 

// --- 2. SPI 总线初始化 (专门全速驱动 OLED 屏幕) ---
// 使用 SPI3 硬件通道：MOSI=D11, MISO=NC, SCLK=D13
SPI spi(D11, NC, D13); 
// 控制引脚配置：DC=D9, RST=D3, CS分配给闲置的D2进行防御性占位
Adafruit_SSD1306_Spi oled(spi, D9, D3, D2, 64, 128); 
// =================================================================

// --- 常量定义 ---
const float PI = 3.1415926535f;
const int CENTER_X = 64; 
const int CENTER_Y = 32; 
const int RADIUS = 28;   

// 辅助函数：根据角度和长度计算指针终点坐标
void get_hand_coords(float angle, int length, int &x, int &y) {
    float radians = (angle - 90.0f) * PI / 180.0f;
    x = CENTER_X + (int)(length * std::cos(radians));
    y = CENTER_Y + (int)(length * std::sin(radians));
}

int main() {
    // 安全延时，等待双硬件外设电平稳定
    thread_sleep_for(200); 

    // 初始化屏幕显示
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0, 0); // 修正：必须显式指定初始光标位置，防止打印错位
    oled.printf("Clock Initializing...");
    oled.display();
    thread_sleep_for(1000);

    // =================================================================
    // 【重要提示】首次使用或时钟芯片没电时，需要取消下方代码的注释以设定初始时间。
    // 设定成功运行一次后，请重新将它们注释掉并再次烧录，否则每次重启单片机时间都会被重置。
    /*
    ds3231_time_t rtc_setup_time;
    rtc_setup_time.hours   = 14;  // 设置当前小时 (24小时制)
    rtc_setup_time.minutes = 30;  // 设置当前分钟
    rtc_setup_time.seconds = 0;   // 设置当前秒数
    rtc.set_time(rtc_setup_time); // 写入 DS3231 芯片内部寄存器
    */
    // =================================================================

    ds3231_time_t currTime;
    int h_x, h_y, m_x, m_y, s_x, s_y;

    while (true) {
        // 1. 从 RTC 硬件芯片中读取当前实时时间
        // 注：如果编译提示结构体成员错误，请根据你下载的底层库尝试修改为 currTime.sec / min / hour
        rtc.get_time(&currTime);

        // 2. 清除屏幕显存缓冲区
        oled.clearDisplay();

        // 3. 绘制模拟表盘 (外圈)
        oled.drawCircle(CENTER_X, CENTER_Y, RADIUS, WHITE);
        // 绘制 12、3、6、9 点钟的物理刻度点
        oled.drawPixel(CENTER_X, CENTER_Y - RADIUS + 2, WHITE); 
        oled.drawPixel(CENTER_X + RADIUS - 2, CENTER_Y, WHITE); 
        oled.drawPixel(CENTER_X, CENTER_Y + RADIUS - 2, WHITE); 
        oled.drawPixel(CENTER_X - RADIUS + 2, CENTER_Y, WHITE); 

        // 4. 计算并绘制时钟的三根指针
        // 秒针：每秒走 6 度 (360度 / 60秒)
        get_hand_coords(currTime.seconds * 6.0f, RADIUS - 2, s_x, s_y);
        oled.drawLine(CENTER_X, CENTER_Y, s_x, s_y, WHITE);

        // 分针：每分钟走 6 度
        get_hand_coords(currTime.minutes * 6.0f, RADIUS - 8, m_x, m_y);
        oled.drawLine(CENTER_X, CENTER_Y, m_x, m_y, WHITE);

        // 时针：每小时走 30 度 (360度 / 12小时) + 分钟带来的平滑补偿
        float hour_angle = (currTime.hours % 12) * 30.0f + currTime.minutes * 0.5f;
        get_hand_coords(hour_angle, RADIUS - 15, h_x, h_y);
        oled.drawLine(CENTER_X, CENTER_Y, h_x, h_y, WHITE);

        // 5. 绘制数字时间 (在左上角提供数字对齐)
        oled.setTextSize(1);
        oled.setCursor(0, 0); // 每一帧刷新前，字体的光标必须无条件归零
        oled.printf("%02d:%02d:%02d", currTime.hours, currTime.minutes, currTime.seconds);

        // 6. 将绘制完毕的完整帧画面，全速推送到 OLED 屏幕上显示
        oled.display();

        // 7. 适当延时。由于不需要手动模拟时间累加，这里的延时纯粹是为了降低总线读写功耗
        thread_sleep_for(150); 
    }
}