#include <Wire.h>
#include "Adafruit_ADS1015.h"
#include <SoftwareSerial.h>
#include "ESP8266.h"

Adafruit_ADS1015 ads1015;
Adafruit_ADS1115 ads;

SoftwareSerial mySerial(13, 12);
ESP8266 wifi(mySerial, 115200);

const uint16_t time_interval = 1000 * 60 * 25;
uint16_t time_start = 0;

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
	time_start = millis();
	Serial.print(">>>> ");		
	Serial.println(time_start);
	delay(2000);
	
	Serial.println(">>>> get start time");
	send_data();
}

float get_voltage()
{
	uint16_t adc1 = ads1015.readADC_SingleEnded(0);
	return (adc1 * 2.00) / 1000.0;
}

void send_data()
{
	Serial.println(">>>> begin");
	// 连接 WiFi
	while (!wifi.joinAP(ssid, pwd)) {
		Serial.print(">>>> wifi connection error\n");
		wifi.restart();
		delay(10000);
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
	Serial.println(">>>> get start time again"); 
	delay(1000);	
}
void loop(void)
{
	if (millis() < time_start)
		time_start -= millis();
	if ((uint16_t)millis() - time_start < time_interval)
		return;
	send_data();
}