### Voltage

基于 Arduino，测量微生物电池电压，利用 ESP8266 WiFi模块发送数据。

#### 硬件

- Arduino
- ESP 8266
- ADS 1015

#### 软件

- Arduino IDE
- vim/vscode
- 串口调试工具
- 服务器

#### 相关链接保存

- ADS1X15
	- [https://github.com/adafruit/Adafruit_ADS1X15](https://github.com/adafruit/Adafruit_ADS1X15)

#### 引脚连线

| Arduino | ESP8266 | ADS1015 |
| :-----: | :-----: | :-----: |
|  5.0v   |    /    |   VCC   |
|  3.3v   |   VCC   |    /    |
|  3.3v   |  CH_PD  |    /    |
|   GND   |   GND   |   GND   |
| PORT10  |  UTXD   |    /    |
| PORT11  |  URXD   |    /    |
| PORT09  |   RST   |    /    |
|   SCL   |    /    |   SCL   |
|   SDA   |    /    |   SDA   |

---

![arduino-uno](assets/arduino-uno.jpg)

*Arduino UNO*

---


![ads1015](assets/ads1015.png)

*ADS1015*

---

![esp8266](assets/esp8266.jpg)

*ESP8266*
