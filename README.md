### ESP8266 模块基本使用

#### 准备材料

- 硬件
  - ESP8266 模块

  - USB_TLL 串口线

  - 3.3V 电源（可以使用 Arduino 的 3.3V 接口进行供电，如果没有 3.3V 的电压接口，可以使用 3V 的进行供电，5V 的接口电压太高会烧坏模块。


- 软件
    - sscom4.2（也可以使用 Arduino IDE 自带的串口调试工具，现阶段不建议使用 Arduino IDE 自带的串口调试工具。



#### 连接示意图

![http://image.geek-workshop.com/forum/201601/06/234002of3b55cavjfg23et.jpg](http://image.geek-workshop.com/forum/201601/06/234002of3b55cavjfg23et.jpg)

<u>*针脚分布图*</u>



![img](http://image.geek-workshop.com/forum/201601/06/233944wg1aqrxrznx07xbj.jpg)

*<u>USB_TLL 与 ESP8266 连接图</u>*



#### 调试 ESP8266 模块

- 用 AT 指令调试 ESP8266 教程 [http://www.geek-workshop.com/thread-25582-1-1.html](http://www.geek-workshop.com/thread-25582-1-1.html)
- 注意事项
  - AT 指令要按给定的格式，不能多一个符号。如「空格」等。
  - 记得勾选串口调试工具记得勾上「发送新行」该选项。
  - 注意串口「占用」问题，要是出现被占用的问题，说明有另一个程序正在使用这个串口，把另一个程序关了即可。
  - 连接处接触「不良」的情况。接触不良可能会导致后面一系列的问题，如无法进行调试，调试过程中无法发送，无法接收等等。




#### ESP8266 AT 指令

这里只列出项目过程中使用较为频繁的指令，更多指令参考 ESP8266 帮助文档。

```c
// 模块重启
AT+RST 

/*
工作模式设置
AT+CWMODE=1  ->  Station 客户端模式
AT+CWMODE=2  ->  AP 接入点模式
AT+CWMODE=3  ->  Station + AP 两种模式共存
*/
AT+CWMODE=3


// 连接到路由器，路由器名称 + 密码，路由器的名称不要使用中文
AT+CWJAP="123","0123456789"

// 连接到 TCP 服务器，地址 + 端口号
AT+CIPSTART="TCP","192.168.1.100",8080

// 发送数据，带上发送的字节长度，不要使用不带参数的发送指令
AT+CIPSEND=4
```



#### 最后

如果以上没问题，也就说明 ESP8266 模块没问题了，可以接下来使用 Arduino UNO 板子连模块了。

线的问题会导致一直无法调试成功。

注意电脑兼容性问题，windows 10 连接调试模块表示躺枪，更新下驱动或者找兼容 windows 10 版本的驱动即可解决。



### 通过 Arduino 调试 ESP8266

#### 针脚连接

```
连接时，按照 WIFI 模块的工作模式来接线。

即 GND 接 Arduino 的 GND ，

VCC 和 CH_PD 接 Arduino 的 3.3V 输出。

UTXD 和 URXD分别接 Arduino 的 13和12引脚

（即程序中由 SoftwareSerial 定义的 RX 和 TX）。

作者：rollingstarky
链接：https://www.jianshu.com/p/64024d8c3773
來源：简书
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
```



参考这份资料和 ESP8266 针脚的示意图，稍微修改了 Arduino 上连接的软串口，其实软串口选哪个没啥影响的。就是烧录程序的时候配置一下就可以了。

面包板是绝配，可以让线看起来不会那么乱，不然后期检查是否连错很麻烦……

下面是我的自己的连接方式

```
ESP8266 <---------> Arduino

GND <---------> GND

VCC, CH_PD <---------> 3.3V // 可以用 3.0V 电压

UTXD, URXD <---------> 11 和 10 引脚 // RX, TX 配置10、11为软串口

// Arduino UNO 上的 RX, TX  接口理论上可行，但我并未进行测试，自行可进行测试。
```



#### 烧录

打开 Arduino IDE，板子型号和端口选好，然后上传即可。代码如下

```C++
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX 配置10、11为软串口
void setup()
{
        Serial.begin(9600);
        while (!Serial);
        mySerial.begin(9600);
}

void loop()
{
        if (mySerial.available())
                Serial.write(mySerial.read());
        if (Serial.available())
                mySerial.write(Serial.read());
}
```



- 烧录完成后，打开串口工具，和之前 AT 指令一样，测试是否连接有问题。
- 这里推荐使用 Arduino IDE 自带的串口调试工具，要注意波特率的和换行方式。
  - 波特率看烧录时的规定。
  - 换行方式「 NL 和 CR」。



#### 最后

如果以上都没问题，也就说明传输方面没问题了，剩下的可以和其他模块进行集成。如测电压模块，利用测电压的模块获得电压后，然后构造发送的数据报文，即可发送数据到服务器。

- 该阶段的参考文档
  - [https://kaspars.net/blog/electronics/esp8266-teensy-arduino?utm_source=tuicool&utm_medium=referral](https://kaspars.net/blog/electronics/esp8266-teensy-arduino?utm_source=tuicool&utm_medium=referral)
  - [http://blog.sina.com.cn/s/blog_76752dcc0102vtyn.html](http://blog.sina.com.cn/s/blog_76752dcc0102vtyn.html)
  - [https://www.jianshu.com/p/64024d8c3773](https://www.jianshu.com/p/64024d8c3773)




### 发送数据到服务器

#### ESP8266 AT 指令集成库

- 有了这个库，你就不用自己手动构造一个 AT 字符串了，都帮你封装好了，然后调用函数接口即可。
- [https://github.com/itead/ITEADLIB_Arduino_WeeESP8266](https://github.com/itead/ITEADLIB_Arduino_WeeESP8266) 


- 发送 HTTP 请求
  - 了解 HTTP 报文头的格式。
  - 构造报文头字符串。加入发送的参数等。
  - 使用封装好的 AT 指令发送。



#### 发送电压到服务器参考代码

```c
#include <Wire.h>
#include "Adafruit_ADS1015.h"
#include <SoftwareSerial.h>
#include "ESP8266.h"

Adafruit_ADS1015 ads1015;
Adafruit_ADS1115 ads;

SoftwareSerial mySerial(13, 12);
ESP8266 wifi(mySerial, 115200);

const unsigned long time_interval = 1000 * 60 * 30;
unsigned long time_start = 0;

const char url[] = "at.aiamv.cn";
const uint32_t port = 80;

const char ssid[] = "NETGEAR";
const char pwd[] = "12345678900";

const int ID = 3;

const char str_head[] = "GET /send.php?";
const char str_body[] = "\
 HTTP/1.1\n\
Host: at.aiamv.cn\n\
Connection : close\n\
Accept : text/html\n\n\n\n";

void setup()
{
	ads1015.begin();
	ads1015.setGain(GAIN_ONE);
	Serial.begin(115200);
	while (!Serial) {;}

	wifi.restart();
	delay(2000);
	time_start = millis();
	Serial.println(">>>> get start time");
}

float get_voltage()
{
	uint16_t adc1 = ads1015.readADC_SingleEnded(0);
	return (adc1 * 2.00) / 1000.0;
}

void loop(void)
{
	if (abs(millis() - time_start) < time_interval)
		return;
	Serial.println(">>>> begin");
	// 连接 WiFi
	while (!wifi.joinAP(ssid, pwd)) {
		Serial.print(">>>> wifi connection error\n");
		wifi.restart();
		delay(1000);
	}
	Serial.print(">>>> wifi connection success\n");
	
	String get = str_head;
	get = get + "voltage=" + get_voltage() + "&id=" + ID;
	get += str_body;

	Serial.println(wifi.getIPStatus());
	// 与服务器进行 TCP 连接
	while (!wifi.createTCP(url, port)) {
		Serial.print(">>>> creat tcp error\n");
		delay(1000);
	}
	Serial.print(">>>> creat tcp success\n");
	Serial.println(wifi.getIPStatus());

	if (wifi.send(get.c_str(), get.length()))
		Serial.println(">>>> send success");
	else
		Serial.println(">>>> send error");
	
	Serial.println(">>> get_string ------------");
	Serial.println(get.c_str());
	Serial.println("------------ <<<\n");
	
	char result[200] = {0};
	wifi.recv(result, 200);
	Serial.println(">>>> result >>> ------------");
	Serial.println(result);
	Serial.println("------------ <<<\n");
	
	// 断开 WiFi 连接
	Serial.print(">>>> leave wifi ");
	wifi.releaseTCP();
	time_start = millis();
	Serial.println(">>>> again get start time"); 
	delay(1000);
}
```