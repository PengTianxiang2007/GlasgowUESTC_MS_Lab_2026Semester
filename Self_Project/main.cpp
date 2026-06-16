#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "ds3231.h"
#include <cmath>

// =================================================================
// --- 1. I2C for DS3231 RTC ---
I2C i2c(D4, D5);
DS3231 rtc(&i2c);

// --- 2. SPI for OLED (SSD1306, 128x64) ---
SPI spi(D11, NC, D13);
Adafruit_SSD1306_Spi oled(spi, D9, D3, D2, 64, 128);
// =================================================================

const float PI = 3.1415926535f;
const int CENTER_X = 52;
const int CENTER_Y = 31;
const int RADIUS   = 25;

const char* DAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

// ---- helper: polar → cartesian for hand endpoints ----
void hand_endpoint(float angle_deg, int length, int &x, int &y) {
    float rad = (angle_deg - 90.0f) * PI / 180.0f;
    x = CENTER_X + (int)(length * std::cos(rad));     //调用余弦函数，得到投影的x、y坐标
    y = CENTER_Y + (int)(length * std::sin(rad));
}

// ---- helper: draw a numbered tick at a given hour ----

//绘制表盘的刻度数字
void draw_hour_number(int hour, int r) {
    // hour=12 at top (0°), hour=3 right (90°), hour=6 bottom (180°), hour=9 left (270°)
    float angle = hour * 30.0f;    //将小时转换为角度
    int x, y;
    hand_endpoint(angle, r, x, y);
    oled.setCursor(x - 3, y - 3);      //视觉偏移纠正
    if (hour < 10)                   //占位输出
        oled.printf("%1d", hour);
    else
        oled.printf("%2d", hour);
}

// ---- helper: thicker line for hour/minute hands ----


void draw_thick_line(int x0, int y0, int x1, int y1, int w, uint16_t color) {
    //先画基准线
    oled.drawLine(x0, y0, x1, y1, color);
    if (w >= 2) {
        //要求宽度
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        //往各个方向平移再画一条
        if (dx > dy) {
            oled.drawLine(x0, y0 - 1, x1, y1 - 1, color);
            oled.drawLine(x0, y0 + 1, x1, y1 + 1, color);
        } else {
            oled.drawLine(x0 - 1, y0, x1 - 1, y1, color);
            oled.drawLine(x0 + 1, y0, x1 + 1, y1, color);
        }
    }
}

int main() {
    thread_sleep_for(200);

    // ---------- splash ----------
    oled.clearDisplay();         //清空
    oled.setTextSize(1);          //设置文字大小
    oled.setCursor(14, 8);        //打印标题
    oled.printf("DS3231  Clock");   
    oled.drawLine(10, 28, 118, 28, WHITE);   //绘制装饰线
    oled.setCursor(30, 34);  
    oled.printf("Initializing");
    oled.display();
    thread_sleep_for(1200);

    // ---------- check RTC health ----------
    ds3231_cntl_stat_t cs;
    rtc.get_cntl_stat_reg(&cs);
    bool osc_lost = (cs.status & 0x80) != 0;   // OSF bit: 1 = oscillator was stopped

    // =================================================================
    // One-time time setting (uncomment, set your time, flash once,
    // then re-comment and flash again):
    /*
    ds3231_time_t setup_t;
    setup_t.hours   = 14;
    setup_t.minutes = 30;
    setup_t.seconds = 0;
    setup_t.mode    = 0;   // 24-hour mode
    rtc.set_time(setup_t);

    ds3231_calendar_t setup_c;
    setup_c.day   = 5;     // 1=Sun … 7=Sat
    setup_c.date  = 22;
    setup_c.month = 5;
    setup_c.year  = 26;    // 2026
    rtc.set_calendar(setup_c);

    // Clear the Oscillator Stop Flag after setting time
    ds3231_cntl_stat_t clear_osf;
    rtc.get_cntl_stat_reg(&clear_osf);
    clear_osf.status &= ~0x80;       // clear OSF bit
    clear_osf.control &= ~(1 << 7);  // clear EOSC, make sure oscillator is on
    rtc.set_cntl_stat_reg(clear_osf);
    */
    // =================================================================

    ds3231_time_t     tm;
    ds3231_calendar_t cal;

    int hx, hy, mx, my, sx, sy, s_tail_x, s_tail_y;     //存放坐标
    int tick_x0, tick_y0, tick_x1, tick_y1;     //存放刻度线的起点和终点
    float angle;
 
    while (true) {
        // ---- read RTC ----
        rtc.get_time(&tm);
        rtc.get_calendar(&cal);
        uint16_t raw_temp = rtc.get_temperature();

        oled.clearDisplay();

        // =============================================================
        //  ANALOG CLOCK FACE
        // =============================================================

        // -- outer double bezel --
        oled.drawCircle(CENTER_X, CENTER_Y, RADIUS,     WHITE);
        oled.drawCircle(CENTER_X, CENTER_Y, RADIUS + 1, WHITE);

        // -- 60 tick marks (short=minute, long=5-minute) --



        //循环，覆盖60个位置

        //处理表盘刻度线
        for (int i = 0; i < 60; i++) {
            angle = i * 6.0f;                
            bool five = (i % 5 == 0);

            //是否被5整除，如果是，则刻度线长度延长
            int outer = five ? RADIUS - 1 : RADIUS;  
            int inner = five ? RADIUS - 5 : RADIUS - 2;
            hand_endpoint(angle, outer, tick_x1, tick_y1);
            hand_endpoint(angle, inner, tick_x0, tick_y0);
            oled.drawLine(tick_x0, tick_y0, tick_x1, tick_y1, WHITE);
        }

        // -- hour numbers 1-12 --
        //在表盘内圈，打印出1-12数字
        oled.setTextSize(1);
        for (int h = 1; h <= 12; h++) {
            draw_hour_number(h, RADIUS - 8);
        }

        //绘制秒针
        // -- second hand (thin, with counter-tail) --
        hand_endpoint(tm.seconds * 6.0f,          RADIUS,     sx, sy);
        hand_endpoint(tm.seconds * 6.0f + 180.0f, 7,          s_tail_x, s_tail_y);
        oled.drawLine(s_tail_x, s_tail_y, sx, sy, WHITE);

        // -- minute hand (medium thickness, smooth sweep) --
        float min_angle = tm.minutes * 6.0f + tm.seconds * 0.1f;
        hand_endpoint(min_angle, RADIUS - 6, mx, my);
        draw_thick_line(CENTER_X, CENTER_Y, mx, my, 2, WHITE);

        // -- hour hand (thickest, smooth sweep) --
        float hr_angle = (tm.hours % 12) * 30.0f + tm.minutes * 0.5f
                         + tm.seconds * (1.0f / 120.0f);
        hand_endpoint(hr_angle, RADIUS - 12, hx, hy);
        draw_thick_line(CENTER_X, CENTER_Y, hx, hy, 3, WHITE);

        // -- centre cap --
        //绘制时钟中心的圆
        oled.fillCircle(CENTER_X, CENTER_Y, 2, WHITE);
        oled.drawCircle(CENTER_X, CENTER_Y, 3, WHITE);

        // =============================================================
        //  RIGHT-SIDE DIGITAL PANEL
        // =============================================================
        // separator line





        //右侧绘制时钟面板


        oled.drawFastVLine(82, 2, 60, WHITE);  //绘制分割线

        oled.setTextSize(2);                   //字号放大
        oled.setTextColor(WHITE);
        oled.setCursor(88, 3);                  //定位一个相对位置中心
        oled.printf("%02d", tm.hours);        
        oled.setCursor(88, 21);
        oled.printf("%02d", tm.minutes);

        oled.setTextSize(1);
        oled.setCursor(113, 10);
        oled.printf(":%02d", tm.seconds);

        // tiny am/pm label (24h mode so always blank, but decorative)
        oled.setCursor(107, 38);
        oled.printf("24H");

        // =============================================================
        //  TOP STATUS BAR
        // =============================================================
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        if (osc_lost) {
            oled.printf("BAT FAIL");
        } else {
            oled.printf("DS3231 OK");
        }

        // =============================================================
        //  BOTTOM BAR — date + temperature
        // =============================================================
        oled.drawFastHLine(0, 54, 82, WHITE);

        oled.setTextSize(1);
        oled.setCursor(0, 56);
        oled.printf("%s %02d/%02d/20%02d",
                    DAY_NAMES[(cal.day > 0 && cal.day <= 7) ? cal.day - 1 : 0],
                    cal.month, cal.date, cal.year);


        //解析温度数据
        int temp_int  = (int8_t)(raw_temp >> 8);            // signed integer part
        int temp_frac = ((raw_temp >> 6) & 0x03) * 25;     // fractional × 0.25°C → hundredths
        oled.setCursor(0, 46);
        oled.printf("T:%d.%02dC", temp_int, temp_frac);

        // =============================================================
        //  PUSH FRAME
        // =============================================================
        oled.display();

        thread_sleep_for(200);   // ~5 fps
    }
}
