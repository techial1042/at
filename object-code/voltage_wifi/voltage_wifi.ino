#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_ADS1015.h"
#include "ESP8266.h"

Adafruit_ADS1015 ads1015;
Adafruit_ADS1115 ads;

SoftwareSerial mySerial(13, 12);
ESP8266 wifi(mySerial, 115200);

const uint16_t time_interval = 1000 * 60 * 1;

const char url[] = "at.aiamv.cn";
const char port = 80;

const char ssid[] = "NETGEAR";
const char pwd[] = "12345678900";

const char ID = 3;

const char str_head[] = "GET /send.php?";
const char str_body[] = "\
 HTTP/1.1\n\
Host: at.aiamv.cn\n\
Connection : close\n\
Accept : text/html\n\n\n\n";


void print_id()
{
	Serial.print(">>>> ID = ");
	Serial.println((int)ID);
	Serial.println(">>>> setup begin");	
}

void hardware_init()
{
	ads1015.begin();
	ads1015.setGain(GAIN_ONE);
	Serial.begin(115200);
	while (!Serial);

	wifi_init();
}

void wifi_init()
{
	wifi.restart();
	delay(10000);
	wifi.leaveAP();
	delay(10000);
}

void setup()
{
	hardware_init();

	print_id();	
	wifi_connect();
	send_data();
	receive_data();
	wifi_close();
}

float get_voltage()
{
	return ((uint16_t)(ads1015.readADC_SingleEnded(0)) * 2.00) / 1000.0;
}

void wifi_connect()
{
	// 连接 WiFi
	while (!wifi.joinAP(ssid, pwd)) {
		Serial.print(">>>> wifi connection error\n");
		wifi.leaveAP();
		delay(10000);		
	}
	Serial.print(">>>> wifi connection success\n");
}

void wifi_close()
{
	Serial.print(">>>> leave wifi ");
	wifi.releaseTCP();
	delay(10000);
	wifi.leaveAP();
	delay(10000);	
}

void TCP_connect()
{
	// 与服务器进行 TCP 连接
	while (!wifi.createTCP(url, (int)port)) {
		Serial.print(">>>> creat tcp error\n");
		delay(1000);
	}
	Serial.print(">>>> creat tcp success\n");
}

void print_get_string(String &get)
{
	Serial.println(">>> get_string ------------");
	Serial.println(get.c_str());
	Serial.println("------------ <<<\n");	
}

void send_data()
{	
	String get = str_head;
	get = get + "voltage=" + get_voltage() + "&id=" + ID + str_body;
	
	Serial.println(wifi.getIPStatus());
	TCP_connect();
	Serial.println(wifi.getIPStatus());

	if (wifi.send(get.c_str(), get.length()))
		Serial.println(">>>> send success");
	else
		Serial.println(">>>> send error");
	
	print_get_string(get);
}

char *receive_data()
{
	static char result[200] = {0};
	wifi.recv(result, 200);
	Serial.println(">>>> result >>> ------------");
	Serial.println(result);
	Serial.println("------------ <<<\n");
	return result;
}

void loop(void)
{
	uint16_t t = millis() % time_interval;
	if (t > 500)
		return;
	Serial.println(millis());
	Serial.println(">>>> loop begin");
	wifi_connect();
	send_data();
	receive_data();
	wifi_close();
	Serial.println(">>>> loop end");
	
	delay(1000);
}