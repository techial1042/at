#include <Wire.h>
#include "SoftwareSerial.h"
#include <avr/boot.h>
#include "Adafruit_ADS1015.h"
#include "data.h"

Adafruit_ADS1015 ads;
String device_id;

void setup()
{
	// 串口等初始化
	ads.begin();
	Serial.begin(115200);
	while (!Serial);
	serial.begin(115200);
	//debug();

	// 模拟烧录固件，使用 at 指令
	firmware();

	// 获得并打印 arduino uno 唯一标识号
	Serial.println("\n\n ---------- begin");
	device_id = get_deivce_id_string();
	Serial.println("device id = " + device_id + ";");
}

// 复位，重启设备
void (*resetFunc)(void) = 0;

void loop(void)
{
	String voltage = get_voltage_diff_string(3);
	Serial.println("ADC differential 0_1, voltage = "
	               + voltage + ";\n");
	delay(200);
	char url_data[40] = {0};
	sprintf("/send.php?voltage=%s&id=%s",
	        voltage.c_str(), device_id.c_str());
	String message = create_http_request_message("GET", url_data,
	                 "HTTP/1.1",
	                 "Host: at.aiamv.cn\n"
	                 "Connection : close\n"
	                 "Accept : text/html\n");

	String buffer = get_at_result(message.c_str());
	Serial.println("buffer >------------------\n");
	Serial.println(buffer);
	Serial.println("----------------<\n");

	if (!find_result(buffer, "200 OK")) {
		Serial.print("send error, count = ");
		Serial.println(++CONNECT_STATUS_FAIL);
	}

	if (CONNECT_STATUS_FAIL > CONNECT_STATUS_FAIL_COUNT) {
		resetFunc();
		delay(2000);
		digitalWrite(RESET, LOW);
		delay(2000);
		restart();
		delay(2000);
	}

	delay(99000);
}

// 获得 arduino uno 唯一标识号
String get_deivce_id_string()
{
	String str;
	for (int i = 14; i < 14 + 10; i++) {
		str += boot_signature_byte_get(i);
		//Serial.println((int)boot_signature_byte_get(i));
	}
	return str;
}

// ADC 转换，并获取 「电压」 的 「数值」
double get_voltage_diff_num()
{
	int16_t results = ads.readADC_Differential_0_1();
	double diff = (double)results * 3.0F / 1000.0;
	return fabs(diff);
}

// 将电压的 「数值」 转换成 「字符串」
String get_voltage_diff_string(unsigned char accuracy)
{
	double voltage = get_voltage_diff_num();
	return String(voltage, accuracy);
}