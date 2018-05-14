#include <Wire.h>
#include <SoftwareSerial.h>
#include <avr/boot.h>
#include "Adafruit_ADS1015.h"

#define RX 10
#define TX 11
#define RESET 9
SoftwareSerial serial(RX, TX);
Adafruit_ADS1015 ads;
String device_id;
unsigned int CONNECT_STATUS_FAIL = 0;
unsigned int CONNECT_STATUS_FAIL_COUNT = 10;

void setup()
{
	ads.begin();
	Serial.begin(115200);
	while (!Serial);
	serial.begin(115200);
	//debug();
	Serial.println("\n\nbegin");
	firmware();
	device_id = 3;//get_deivce_id_string();
	Serial.println("device id = " + device_id + ";");
}

void (*resetFunc)(void) = 0;
unsigned long long last_send_time = 0;
unsigned long long TIMEOUT = 1000 * 60 * 5;

void loop(void)
{
	//if (1) {
	if (last_send_time == 0 || millis() - last_send_time >= TIMEOUT) {
		String voltage = get_voltage_diff_string(3);
		Serial.println("ADC differential 0_1, voltage = "
				+ voltage + ";\n");
		delay(200);
		String post = foramt_send_data(device_id, voltage);
		if (!is_send_success(post.c_str())) {
			Serial.print("send error, count = ");
			Serial.println(++CONNECT_STATUS_FAIL);
		}
		//Serial.println("post >------------------\n"
		//               + post
		//               + "----------------<\n");

		if (CONNECT_STATUS_FAIL > CONNECT_STATUS_FAIL_COUNT) {
			resetFunc();
			delay(2000);
			digitalWrite(RESET, LOW);
			delay(2000);
			restart();
			delay(2000);
		}
		last_send_time = millis();
		Serial.println("send end ---------------");
	}
	}

	String get_deivce_id_string()
	{
		String str;
		for (int i = 14; i < 24; i++)
			str.concat(boot_signature_byte_get(i));
		return str;
	}

	double get_voltage_diff_num()
	{
		int16_t results = ads.readADC_Differential_0_1();
		double diff = (double)results * 3.0F / 1000.0;
		return fabs(diff);
	}

	String get_voltage_diff_string(unsigned char accuracy)
	{
		double voltage = get_voltage_diff_num();
		return String(voltage, accuracy);
	}

	bool is_send_success(char *post)
	{
		serial.println(post);
		String event = serial_event();
		Serial.println("event >-----------------\n"
				+ event 
				+ "---------------<\n");
		return ((event.indexOf("SUCCESS") != -1) || (event.indexOf("200 OK") != -1));
	}

	String serial_event()
	{
		String input;
		static const unsigned long long timeout = 1000 * 2;
		unsigned long long time_start = millis();
		while (millis() - time_start < timeout) {
			if (serial.available()) {
				input += (char)serial.read();
			}
		}
		input.trim();
		return input;
	}

	String foramt_send_data(String device_id, String voltage)
	{
		String data = String("voltage=") + voltage
			+ String("&id=") + device_id;

		return String("GET /send.php?") + data + String(" HTTP/1.1\n")
			+ String("Host: at.aiamv.cn\n")
			+ String("Connection : close\n")
			+ String("Accept : text/html\n")
			+ String("\n\n\n");
	}

	void debug()
	{
		while (1) {
			if (serial.available())
				Serial.write(serial.read());
			if (Serial.available())
				serial.write(Serial.read());
		}
	}

	void restart()
	{
		serial.print("+++");
		delay(500);
		serial.println("\r\n");
		delay(1000);
		serial.println("AT+RST");
		delay(10000);
		CONNECT_STATUS_FAIL = 0;
	}

	/*
	 *	发送 at 指令模拟固件
	 *	+++ 退出透传模式
	 *	restart 重启设备
	 *	ate0 关闭回显
	 *	gmr 查询版本信息
	 */
	void firmware()
	{
		serial.print("+++");
		delay(500);
		serial.println("\r\n");
		delay(1000);
		serial.println("AT+RESTORE");
		delay(5000);
		serial.println("ATE0");
		delay(1000);
		serial.println("AT+GMR");
		delay(1000);

		save_in_flash();
	}

	/*
	 *	将配置写入 flash, 断电不消失
	 *
	 *	cwmode 配置为终端模式
	 *
	 *	cwjap 连接到 WiFi AT+CWJAP_DEF=<ssid>,<pwd>[,<bssid>]
	 *
	 *	cwautoconn 上电自动连上 WiFi
	 *
	 *	cipmux 设置 TCP 单链接
	 *
	 *	cipmode=1 设置为透传模式
	 *
	 *	savetranslink 保存透传到 falsh
	 *	AT+SAVETRANSLINK=<mode>,<remote IP or domain name>,<remote port>[,<type>,<TC keep alive>]
	 */
	void save_in_flash()
	{
		serial.println("AT+CWMODE_DEF=1");
		delay(3000);
		serial.println("AT+CWJAP_DEF=\"NETGEAR\",\"12345678900\"");
		delay(5000);
		serial.println("AT+CWAUTOCONN=1");
		delay(3000);
		serial.println("AT+CIPMUX=1");
		delay(3000);
		serial.println("AT+SAVETRANSLINK=1,\"at.aiamv.cn\",80,\"TCP\"");
		delay(3000);
		serial.println("AT+RST");
		delay(5000);
	}
