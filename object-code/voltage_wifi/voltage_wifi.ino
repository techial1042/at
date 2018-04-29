#include <Wire.h>
#include <SoftwareSerial.h>
#include <avr/boot.h>
#include "Adafruit_ADS1015.h"

/*
   const char url[] = "at.aiamv.cn";
   const char port = 80;

   const char ssid[] = "NETGEAR";
   const char pwd[] = "12345678900";


   AT+CWMODE_DEF=1

   AT+CWJAP_DEF="NETGEAR","12345678900"

   AT+CWAUTOCONN=1

   AT+CIPSTART="TCP","183.230.40.33","80"

   AT+CIPMODE=1

   AT+CIPSEND

GET /send.php?voltage=0.5&id=88555550535125571933 HTTP/1.1
	Host: at.aiamv.cn
	Connection : close
	Accept : text/html



*/

Adafruit_ADS1015 ads;
SoftwareSerial mySerial(10, 11); // RX, TX 配置10、11为软串口
String device_id;

String get_device_id()
{
	for (int i = 14; i < 24; i++)
		device_id.concat(boot_signature_byte_get(i));
}

void hardware_init()
{
	ads.begin();
	Serial.begin(115200);
	while (!Serial);
	mySerial.begin(115200);
}

void get_voltage(char *voltage)
{
	int16_t results = ads.readADC_Differential_0_1();
	double v = results * 3.0F / 1000.0;
	dtostrf(fabs(v), 0, 3, voltage);
}

void format_string(char *voltage, String device_id, char *str)
{
	sprintf(str, 
"GET /send.php?voltage=%s&id=4 HTTP/1.1\n\
Host: at.aiamv.cn\n\
Connection : close\n\
Accept : text/html\r\n\r\n\r\n\r\n",
			voltage);
}

void setup()
{
	get_device_id();
	hardware_init();
	Serial.println("device id = " + device_id + ";");
	set_link();
}

const uint16_t time_interval = 60000 * 10;

void send_data()
{
	char vo[10] = {0};
	get_voltage(vo);
	Serial.println(vo);
	char str[200] ={0};
	format_string(vo, device_id, str);	
	Serial.println(str);
	mySerial.println(str);
	delay(1000);
}

void set_link()
{
	mySerial.println("AT+CIPSTART=\"TCP\",\"at.aiamv.cn\",80");
	delay(1000);
	mySerial.println("AT+CIPMODE=1");
	delay(1000);
	mySerial.println("AT+CIPSEND");
	delay(2000);
}

void debug()
{
	while (1) {
		if (mySerial.available())
			Serial.write(mySerial.read());
		if (Serial.available())
			mySerial.write(Serial.read());
	}	
}

void loop(void)
{
//	debug();
	send_data();
	delay(100000 * 5);
}
