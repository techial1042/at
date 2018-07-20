/*
 * RX TX 接收和发送引脚
 * RESET 重启模块使用，拉低至低电平即可
 */
#define RX 10
#define TX 11
#define RESET 9

SoftwareSerial serial(RX, TX);

// 发送失败计数， 允许发送失败的最大值
unsigned int CONNECT_STATUS_FAIL = 0;
unsigned int CONNECT_STATUS_FAIL_COUNT = 10;


void clean_buffer()
{
	while (serial.available())
		serial.read();
}

// 返回缓冲区的内容
String serial_event()
{
	String input;
	while (serial.available())
		input += (char)serial.read();
	return input;
}

/*
 * 发送 at 指令, 不返回返回值
 * newline 表示是否在字符串后加入换行， 默认加入换行
 * delay_time 默认延迟 3000ms
 */
void send_at_command(const char *at,
                     const bool newline = true,
                     const unsigned long delay_time = 3000)
{
	clean_buffer();
	delay(delay_time);
	if (newline) {
		serial.println(at);
	} else {
		serial.print(at);
	}
}

/*
 * 发送 at 后，获得其返回值
 * newline 表示是否在字符串后加入换行， 默认加入换行
 * delay_time 默认延迟 3000ms
 */
String get_at_result(const char *at, const bool newline = true,
                     const unsigned long delay_time = 3000)
{
	clean_buffer();
	send_at_command(at, newline, delay_time);
	delay(220);
	return serial_event();
}

bool find_result(const String buffer, const char *res)
{
	return buffer.indexOf(res) == -1;
}


/*
 * 构造 http 报文
 * method 可以为 GET/POST 等等，区分大小写
 * url 请求的地址
 * version http版本号
 * head http头部
 * 无需多加换行
 */
String create_http_request_message(const char *method,
                                   const char *url, const char *version,
                                   const char *head, const char *body = "")
{
	char request[300] = {0};
	sprintf(request, "%s %s %s\n%s\n%s\n",
	        method, url, version, head, body);
	return String(request);
}

// 通过 arduino 软串口调试模块
void debug()
{
	while (1) {
		if (serial.available())
			Serial.write(serial.read());
		if (Serial.available())
			serial.write(Serial.read());
	}
}

// 退出透传模式后，重启 WiFi 模块，并将发送错误次数置零
void restart()
{
	send_at_command("+++", false, 500);
	send_at_command("\r\n", true, 1000);
	send_at_command("AT+RST", true, 10000);

	CONNECT_STATUS_FAIL = 0;
}

/*
 * 设置连接 WiFi，传进 at, ssid, pwd
 * 格式化字符串后返回 at 指令
 * at, ssid, pwd 严格区分大小写
 */
void set_wifi(char *at,
              const char *ssid, const char *pwd)
{
	sprintf(at, "AT+CWJAP_DEF=\"%s\",\"%s\"", ssid, pwd);
}

/*
 * 设置连接服务器，传进 at, ip, port, method
 * 格式化字符串后返回 at 指令
 * method TCP，UDP 二选一
 */
void set_server_link(char *at, const char *ip, const unsigned int port,
                     const char *method)
{
	sprintf(at, "AT+SAVETRANSLINK=1,\"%s\",%d,\"%s\"", ip, port, method);
}

/*
 * 将配置写入 flash, 断电不消失, at 指令后有带 _DEF 一般都是断电不丢失
 *
 * cwmode 配置为终端模式
 *
 * cwjap 连接到 WiFi AT+CWJAP_DEF=<ssid>,<pwd>[,<bssid>]
 *
 * cwautoconn 上电自动连上 WiFi
 *
 * cipmux 设置 TCP 单链接
 *
 * cipmode=1 设置为透传模式
 *
 * savetranslink 保存透传到 falsh
 * AT+SAVETRANSLINK=<mode>,<remote IP or domain name>,<remote port>[,<type>,<TC keep alive>]
 */
void save_in_flash()
{
	char at[30] = {0};

	send_at_command("AT+CWMODE_DEF=1");

	set_wifi(at, "NETGEAR", "12345678900");
	send_at_command(at);

	send_at_command("AT+CWAUTOCONN=1");
	send_at_command("AT+CIPMUX=1");

	set_server_link(at, "at.aiamv.cn", 80, "TCP");
	send_at_command(at);

	send_at_command("AT+RST");

	delay(5000);
}

/*
 * 发送 at 指令来模拟固件
 * +++ 退出透传模式
 * restart 重启设备
 * ate0 关闭回显
 * gmr 查询版本信息
 */
void firmware()
{
	send_at_command("+++", false, 3000);
	send_at_command("\r\n", true, 3000);
	send_at_command("AT+RESTORE", true, 3000);
	send_at_command("ATE0", true, 3000);
	send_at_command("AT+GMR", true, 3000);

	save_in_flash();
}