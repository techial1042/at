#include <Wire.h>
#include "SoftwareSerial.h"
#include <avr/boot.h>
#include "Adafruit_ADS1015.h"
#include "data.h"
#include "FlexiTimer2.h"

Adafruit_ADS1015 ads;
String device_id;

#define _DEBUG_ESP8266 0
#define _DEBUG_ADS1015 0


int first_flag = 0;

void setup()
{
	// 串口等初始化
	ads.begin();
	Serial.begin(115200);
	while (!Serial);
	serial.begin(115200);


#if _DEBUG_ADS1015

	while (1) {
		String voltage = get_voltage_diff_string(3);
		Serial.println("ADS differential 0_1, voltage = " +
				voltage + ";\n");
		delay(200);
	}
	
#endif

#if _DEBUG_ESP8266

	debug();

#endif

	// 模拟烧录固件，使用 at 指令
	firmware();

	// 获得并打印 arduino uno 唯一标识号
	Serial.println("\n\n---------- begin");
	device_id = "7"; //get_deivce_id_string();

	Serial.println("device id = " + device_id + ";");

	FlexiTimer2::set(1000, 1.0 / 1000, count_time);
	FlexiTimer2::start();
}

// 复位，重启设备
void (*resetFunc)(void) = 0;


int timeout = 0;
void count_time()
{
	timeout++;
}

const int SEND_TIMEOUT = 60 * 30;
void loop(void)
{
	if (first_flag != 1 || timeout <= SEND_TIMEOUT) {
		return;
	}

	first_flag = 0;
	timeout = 0;
	
	// 获得此时测量的电压
	String voltage = get_voltage_diff_string(3);
	Serial.println("ADC differential 0_1, voltage = " +
			voltage + ";\n");
	delay(200);

	// 构造 url
	char url_data[60] = {0};
	sprintf(url_data, "/send.php?voltage=%s&id=%s",
			voltage.c_str(), device_id.c_str());

	String message = create_http_request_message("GET", url_data,
			"HTTP/1.1",
			"Host: at.aiamv.cn\n"
			"Connection: close\n"
			"Accept: text/html\n");
	Serial.println(message);

	// 发送 http 请求，并打印其返回值
	String buffer = get_at_result(message.c_str());
	Serial.println("buffer >------------------\n");
	Serial.println(buffer);
	Serial.println("----------------<\n");

	// 如果 find 200 OK，说明发送成功
	if (find_result(buffer, "200 OK")) {
		Serial.print("send error, count = ");
		Serial.println(++CONNECT_STATUS_FAIL);
	}

	// 发送次数过多，先重启 WiFi模块， 再重启 arduino
	if (CONNECT_STATUS_FAIL > CONNECT_STATUS_FAIL_COUNT) {
		digitalWrite(RESET, LOW);
		delay(2000);
		resetFunc();
		delay(2000);
		restart();
		delay(2000);
	}

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
	delay(100);
	double diff = (double)results * 3.0F / 1000.0;
	return fabs(diff);
}

// 将电压的 「数值」 转换成 「字符串」
String get_voltage_diff_string(unsigned char accuracy)
{
	double voltage = get_voltage_diff_num();
	return String(voltage, accuracy);
}
