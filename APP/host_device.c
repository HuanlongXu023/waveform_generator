#include "host_device.h"
#include "usbd_cdc_if.h"
#include "lwrb.h"
//#include "test_ui.h"
#include "waveform_generator.h"
#include "save_load.h"

// #define USB_BUFFER_SIZE (MAX_WAVEFORM_POINTS * 2 + 1)
// #define USB_BUFFER_SIZE (256)

uint32_t USB_tick;

uint8_t USB_DataBuff[USB_BUFFER_SIZE];

lwrb_t USB_RxBuffer;

void host_device_task_init(void)
{
	char myStr[128] = {0};							  // 定义一个数组，用于存放要输出的字符串
	sprintf(myStr, "<<<<<<APP\r\r");				  // 格式化字符串
	CDC_Transmit_FS((uint8_t *)myStr, strlen(myStr)); // 发送字符串

	sprintf(myStr, "req max = 100000, freq min = 1\r\r");
	CDC_Transmit_FS((uint8_t *)myStr, strlen(myStr)); // 发送字符串

	lwrb_init(&USB_RxBuffer, USB_DataBuff, USB_BUFFER_SIZE);
}

// ================= 辅助函数：USB 发送字符串 =================
void usb_send_str(char *str)
{
	CDC_Transmit_FS((uint8_t *)str, strlen(str));
}

// ================= 辅助函数：USB 发送单字节 =================
void usb_send_byte(uint8_t byte)
{
	CDC_Transmit_FS(&byte, 1);
}

void hexdump_16(void *data, uint16_t size)
{
	const uint16_t max = 16;
	for (uint16_t i = 0; i < size; i++)
	{
		if ((i % max == 0) && (i != 0))
		{
			printf("\n");
		}

		printf("%d ", ((uint16_t *)data)[i]);
	}

	printf("\r\n");
}

// ================= 核心：命令解析与执行 =================

uint8_t ARB_num;
uint8_t ARB_flag;

uint8_t Recv_data[USB_BUFFER_SIZE];

uint16_t Recv_data_dac[USB_BUFFER_SIZE / 2];
// uint16_t Recv_data_dac[MAX_WAVEFORM_POINTS];

void divide_dac_data_by_4(uint16_t *data, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		data[i] = data[i] >> 2; // 使用右移操作除以4，提高效率
	}
}

void process_protocol_cmd(uint8_t *data, uint16_t len)
{
	char reply[64] = {0};

	// 1. 安全检查：长度至少 3 字节（指令码）
	if (len < 3)
		return;

	// 2. 去除末尾的 0x0a (\n) 和 0x0d (\r)，方便处理
	while (len > 0 && (data[len - 1] == '\n' || data[len - 1] == '\r'))
	{
		data[len - 1] = '\0';
		len--;
	}
	if (ARB_flag)
	{
		ARB_flag = 0;
		printf("ARB_num len = %d\r\n", len);
		// hexdump(data, len);
		memcpy(Recv_data_dac, data, len);
		divide_dac_data_by_4(Recv_data_dac, len / 2);
		hexdump_16(Recv_data_dac, len / 2);
		save_arb(ARB_num);
	}
	// ==========================================
	// 3. 指令匹配与处理 (简单的 if-else，直观易懂)
	// ==========================================

	// ---------------------------------------------------------
	// 指令 1: UMO (读取本机型号)
	// ---------------------------------------------------------
	if (strncmp((char *)data, "UMO", 3) == 0)
	{
		// 协议要求：返回型号字符串 + 0x0a
		// 这里模拟返回 "FY6000"
		usb_send_str("FY3900-05M");
		// usb_send_str("SG-28");
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "UVE", 3) == 0)
	{
		usb_send_str("V1.3");
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}
	// ---------------------------------------------------------
	// 指令 2: WMF (设置 CH1 频率)
	// 格式: WMFxxxxxxxx.xxxxxx
	// ---------------------------------------------------------
	else if (strncmp((char *)data, "WMF", 3) == 0)
	{
		// 提取数据部分 (从第 3 个字符开始)
		char *freq_str = (char *)data + 3;

		// 字符串转浮点数/整数
		double freq_val = atof(freq_str);

		// 更新全局参数 (取整，单位 Hz)
		// ch1_params.freq = (uint32_t)freq_val;

		// 调试打印（可选）
		char dbg[64];
		// sprintf(dbg, "\r\n[Set] Freq=%d Hz\r\n", (int)ch1_params.freq);
		usb_send_str(dbg);

		// 协议要求：写入成功只回复 0x0a
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	// ---------------------------------------------------------
	// 指令 3: RMF (读取 CH1 频率)
	// ---------------------------------------------------------
	// else if (strncmp((char *)data, "RMF", 3) == 0)
	// {
	// 	// 协议要求格式: xxxxxxxx.xxxxxx + 0x0a
	// 	// 例如 1000Hz 变成 "00001000.000000\n"

	// 	// uint32_t integer_part = ch1_params.freq; // 整数部分
	// 	// uint32_t decimal_part = 0;				 // 小数部分 (简化处理)

	// 	// sprintf(reply, "%08lu.%06lu", integer_part, decimal_part);
	// 	// usb_send_str(reply);
	// 	usb_send_byte(0x0a);
	// 	usb_send_byte(0x0a);
	// 	return;
	// }

	else if (strncmp((char *)data, "RSA", 3) == 0)
	{
		// 1. 检查长度：必须至少有 "RSAx" 4个字符
		if (strlen((char *)data) < 4)
		{
			usb_send_byte(0x0a); // 长度不对，直接回复结束符
			return;
		}

		// 2. 【核心】提取第4个字符 (下标为3)，并转为数字
		// '0' -> 0, '1' -> 1 ...
		uint8_t x = data[3] - '0';

		// 3. 范围校验 (协议规定 x 为 0-4)
		if (x > 4)
		{
			usb_send_byte(0x0a);
			return;
		}

		// 4. 根据 x 的值，决定回复什么
		uint8_t reply_val = 0;

		// sync_data.freq = 1;

//		switch (x)
//		{
//		case 0:
//			reply_val = sync_data.wave;
//			break; // 波形同步
//		case 1:
//			reply_val = sync_data.freq;
//			break; // 频率同步
//		case 2:
//			reply_val = sync_data.amp;
//			break; // 幅度同步
//		case 3:
//			reply_val = sync_data.offset;
//			break; // 偏置同步
//		case 4:
//			reply_val = sync_data.duty;
//			break; // 占空同步
//		}

		// 5. 按协议要求回复：返回数字字符串 + 0x0a
		// 协议示例：返回 "0" 或 "255"
		char reply_str[8] = {0};
		sprintf(reply_str, "%d", reply_val);

		usb_send_str(reply_str);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);

		return;
	}
	else if (strncmp((char *)data, "RBZ", 3) == 0) // 蜂鸣器
	{
		char reply_str[8] = {0};
		sprintf(reply_str, "%d", 255);

		usb_send_str(reply_str);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);

		return;
	}
	else if (strncmp((char *)data, "RMW", 3) == 0) // 读取CH1波形
	{
		// char reply[64] = {0};

		sprintf(reply, "%d", wave_params[DAC_CH1].type);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);

		return;
	}
	else if (strncmp((char *)data, "RMF", 3) == 0) // 读取CH1波形频率
	{
		sprintf(reply, "%08d.000000", wave_params[DAC_CH1].freq);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}
	else if (strncmp((char *)data, "RFF", 3) == 0) // 读取CH2波形频率
	{
		sprintf(reply, "%08d.000000", wave_params[DAC_CH2].freq);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RMA", 3) == 0) // 读取CH1幅度
	{
		sprintf(reply, "%d", wave_params[DAC_CH1].amp * 10);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RFA", 3) == 0) // 读取CH2幅度
	{
		sprintf(reply, "%d", wave_params[DAC_CH2].amp * 10);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RMO", 3) == 0) // 读取CH1偏置
	{
		sprintf(reply, "%d", wave_params[DAC_CH1].offset);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RFO", 3) == 0) // 读取CH2偏置
	{
		sprintf(reply, "%d", wave_params[DAC_CH2].offset);

		usb_send_str(reply);

		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RMO", 3) == 0) // 读取CH1偏置
	{
		sprintf(reply, "%d", wave_params[DAC_CH1].offset);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RFO", 3) == 0) // 读取CH2偏置
	{
		sprintf(reply, "%d", wave_params[DAC_CH2].offset);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RMD", 3) == 0) // 读取CH1占空比
	{
		sprintf(reply, "%d", wave_params[DAC_CH1].duty * 1000);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RFD", 3) == 0) // 读取CH2占空比
	{
		sprintf(reply, "%d", wave_params[DAC_CH2].duty * 1000);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RMH", 3) == 0 || strncmp((char *)data, "RFH", 3) == 0) // 读取 CH1 梯形波上升时间
	{
		sprintf(reply, "%d", 0);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}

	else if (strncmp((char *)data, "RML", 3) == 0 || strncmp((char *)data, "RFL", 3) == 0) // 读取 CH1 梯形波上升时间
	{
		sprintf(reply, "%d", 0);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}
	else if (strncmp((char *)data, "RMP", 3) == 0) // 读取CH1相位
	{
		sprintf(reply, "%d", wave_params[DAC_CH1].phase * 1000);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}
	else if (strncmp((char *)data, "FMP", 3) == 0) // 读取CH2相位
	{
		sprintf(reply, "%d", wave_params[DAC_CH2].phase * 1000);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}
	else if (strncmp((char *)data, "RSS", 3) == 0) // 读取 CH1 可调脉冲波脉宽时间
	{
		sprintf(reply, "%d", 0);
		usb_send_str(reply);
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		return;
	}
//	else if (strncmp((char *)data, "RMN", 3) == 0) // 读取CH1输出开始/停止
//	{
//		sprintf(reply, "%d", ch_en[DAC_CH1] ? 255 : 0);
//		usb_send_str(reply);
//		usb_send_byte(0x0a);
//		usb_send_byte(0x0a);
//		return;
//	}
//	else if (strncmp((char *)data, "RFN", 3) == 0) // 读取CH2输出开始/停止
//	{
//		sprintf(reply, "%d", ch_en[DAC_CH2] ? 255 : 0);
//		usb_send_str(reply);
//		usb_send_byte(0x0a);
//		usb_send_byte(0x0a);
//		return;
//	}
	else if (strncmp((char *)data, "DDS_WAVE", 3) == 0) // 任意波形
	{
		ARB_num = data[8] - '0';
		printf("ARB_num = %d\r\n", ARB_num);
		usb_send_str("w");
		usb_send_byte(0x0a);
		usb_send_byte(0x0a);
		ARB_flag = 1;
		return;
	}
	// ---------------------------------------------------------
	// 未知指令
	// ---------------------------------------------------------
	// else
	// {
	// 	// 未知指令也回复一个 0x0a 保持兼容，或者不回复
	// 	usb_send_str("w");
	// 	usb_send_byte(0x0a);
	// 	usb_send_byte(0x0a);
	// 	return;
	// }
}
 
void host_task(void)
{
	uint16_t len = lwrb_get_full(&USB_RxBuffer);
	if (len > 0 && HAL_GetTick() - USB_tick > 10)
	{
		// char myStr[64] = {0};							  // 定义一个数组，用于存放要输出的字符串
		// sprintf(myStr, "\r\r len = %d \r\r", len);		  // 格式化字符串
		// CDC_Transmit_FS((uint8_t *)myStr, strlen(myStr)); // 发送字符串

		lwrb_read(&USB_RxBuffer, Recv_data, len);
		// CDC_Transmit_FS(data, len);
		printf("Recv_data = %s\r\n", Recv_data);

		process_protocol_cmd(Recv_data, len);
	}
	else
	{
		osDelay(10);
	}
}
