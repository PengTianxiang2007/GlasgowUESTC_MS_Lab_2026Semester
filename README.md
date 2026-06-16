# GlasgowUESTC MS Lab 2026 Semester

电子科技大学格拉斯哥海南学院 微电子系统（Microelectronic Systems）课程实验代码

**Platform**: ARM Mbed OS (NUCLEO-L432KC)  
**Semester**: 2025-2026 第二学期  

---

## 目录结构

```
├── Exercise1/              LED闪烁实验
├── Exercise2/              方波生成实验
├── Exercise3/              开关控制频率选择
├── Exercise4/              2-bit 模拟开关控制
├── Exercise5/              七段数码管显示 0-9
├── Exercise5_2/            七段数码管显示 HELP
├── Exercise6/              DAC 输出恒定电压
├── Exercise7/              DAC 生成锯齿波
├── Exercise8/              ADC 电位器控制 LED
├── Exercise8_1/            ADC-DAC 直通
├── Exercise9_Master/       SPI 主机通信
├── Exercise9_Slaver/       SPI 从机通信
├── Self_Project/           DS3231 模拟+数字时钟 (OLED)
└── README.md
```

---

## Exercise 1: LED 闪烁

交替闪烁红色(D5)和绿色(D6) LED，使用 `DigitalOut` 和 `ThisThread::sleep_for` 实现 1 秒间隔。

**涉及知识点**: GPIO 数字输出, 线程延时

```cpp
DigitalOut red(D5);
DigitalOut green(D6);
while (true) {
    red = !red;
    green = !red;
    ThisThread::sleep_for(1s);
}
```

---

## Exercise 2: 方波生成

在 D3 引脚生成方波，使用 `wait_us(500)` 控制半周期，频率约 1kHz。

**涉及知识点**: 微秒级延时, 数字输出切换

---

## Exercise 3: 开关控制双频输出

通过 D5 物理开关选择输出频率：
- 开关按下 (HIGH): 200Hz
- 开关释放 (LOW): 500Hz

使用 74HC4066 模拟开关实现频率切换。

**涉及知识点**: `DigitalIn`, 条件分支, 精确 us 延时

---

## Exercise 4: 2-bit 模拟开关控制

使用 D5/D6 两个引脚控制 74HC4066 的四通道选择 (00 → 01 → 10 → 11)，实现 2-bit 二进制计数。

**涉及知识点**: `BusOut` (替代), 多引脚协调控制

---

## Exercise 5: 七段数码管 0-9 计数器

使用 `BusOut` 同时控制 A-G+DP 八个段，循环显示数字 0-9，每个数字维持 500ms。

```cpp
BusOut display(A0, D3, D4, A2, A1, D9, D10, D11);
```

**涉及知识点**: `BusOut` 并行输出, 七段码编码表, switch-case

---

## Exercise 5_2: 七段数码管显示 "HELP"

在七段数码管上依次显示 H-E-L-P，重复 5 次后熄灭。使用自定义段码编码显示字母。

**涉及知识点**: 自定义七段码, 循环控制

---

## Exercise 6: DAC 输出恒定电压

使用 L432KC 的 DAC (DAC1_OUT1, 引脚 A4) 循环输出 4 个目标电压：0.5V, 1.0V, 2.0V, 2.5V。

```cpp
AnalogOut Aout(A4);
Aout = target_voltage / 3.3f;  // 映射到 0.0-1.0
```

**涉及知识点**: `AnalogOut`, DAC, 电压映射 (Vout = code × Vref)

---

## Exercise 7: DAC 锯齿波生成

生成 100Hz 锯齿波 (阶梯近似，500 步/周期，步长 20μs)，峰值 3.0V。

**涉及知识点**: 波形合成, DAC 阶梯逼近, 时序控制

---

## Exercise 8: ADC 电位器控制 LED 灯条

读取 A6 引脚的模拟电压 (电位器)，根据电压值依次点亮 D2-D5 四个 LED：

| 电压范围 | LED 状态 |
|---------|----------|
| < 0.6V  | 全灭 |
| 0.6-1.2V | LED1 |
| 1.2-1.8V | LED1+2 |
| 1.8-2.4V | LED1+2+3 |
| > 2.4V | 全亮 |

**涉及知识点**: `AnalogIn`, ADC 采样, 模拟电压到数字逻辑的转换

---

## Exercise 8_1: ADC-DAC 直通

最简单的 ADC→DAC 信号直通：读取 A6 输入，直接写入 A4 输出。

```cpp
AnalogOut Aout(A4);
AnalogIn  Ain(A6);
while (1) {
    Aout = Ain;
}
```

**涉及知识点**: ADC + DAC 联合使用

---

## Exercise 9: SPI 双机通信

使用两块 L432KC 进行 SPI 全双工通信，互相交换开关状态并显示在 LED 上。

- **Master** (D11/D12/D13 + A3 CS): 发起通信
- **Slaver** (同样引脚): 响应主机

通信协议: 主机拉低 A3 (片选), 发送开关状态字节, 同时接收从机状态。双方均在本地 LED 上显示对方状态。

**涉及知识点**: `SPI` / `SPISlave`, 全双工通信, 片选信号, 位掩码

---

## Self Project: DS3231 模拟+数字时钟

最终项目——基于 DS3231 RTC 和 SSD1306 OLED 的桌面时钟，兼具模拟表盘和数字显示。

### 功能特性

- **模拟表盘**: 圆形表盘 + 60 个刻度 + 12 个数字，含时针(粗)/分针(中)/秒针(细)
- **平滑扫掠**: 分针和时针根据秒/分进行平滑连续移动
- **数字面板**: 右侧显示 HH:MM:SS 格式，24 小时制
- **状态显示**: 顶部显示 RTC 状态 (DS3231 OK / BAT FAIL)
- **日期温度**: 底部显示日期 (Day MM/DD/YYYY) 和 DS3231 内置温度传感器读数
- **秒针尾迹**: 秒针带反向短尾，更像真实时钟

### 硬件连接

| 外设 | 接口 | 引脚 |
|------|------|------|
| DS3231 RTC | I2C | SDA=D4, SCL=D5 |
| SSD1306 OLED | SPI | MOSI=D11, SCLK=D13, CS=D9, DC=D3, RST=D2 |

### 使用库

本项目依赖以下第三方 Mbed 库（均已包含在 `Self_Project/` 中）：

- **Adafruit-GFX-Library-mbed-compatible**: Adafruit GFX 底层图形库 (绘图、字体)
- **Adafruit_GFX-Mbed-master**: SSD1306 OLED 驱动 (基于 Adafruit GFX)
- **mbed-ds3231-main**: DS3231 高精度 RTC 驱动库

### 首次使用

1. 取消 `main.cpp` 中的注释块（约第 87-108 行），设置当前时间
2. 编译烧录一次以初始化 RTC
3. 重新注释掉时间设置代码，再次烧录
4. 之后 RTC 由备用电池维持时间，无需重复设置

---

## 开发环境

- **IDE**: Keil Studio Cloud / Mbed Studio / Mbed CLI
- **目标板**: NUCLEO-L432KC (STM32L432KC)
- **Mbed OS**: 6.x (通过 mbed-os.lib 管理)

## License

MIT License — 仅供参考和学习使用。第三方库遵循各自的许可证。
