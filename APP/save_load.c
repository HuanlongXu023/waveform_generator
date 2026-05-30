#include "save_load.h"
#include <easyflash.h>
#include "sfud.h"

// uint64_t save_flag = 0;

// #define SAVE_FLAG_KEY "save_flag"

static void test_env(void)
{
  uint32_t i_boot_times = NULL;
  char *c_old_boot_times, c_new_boot_times[11] = {0};

  /* get the boot count number from Env */
  c_old_boot_times = ef_get_env("boot_times");
  assert_param(c_old_boot_times);
  i_boot_times = atol(c_old_boot_times);
  /* boot count +1 */
  i_boot_times++;
  printf("The system now boot %d times\n\r", i_boot_times);
  /* interger to string */
  sprintf(c_new_boot_times, "%d", i_boot_times);
  /* set and store the boot count number to Env */
  ef_set_env("boot_times", c_new_boot_times);
  ef_save_env();
}

void save_load_init(void)
{
  easyflash_init();
  test_env();

  // ef_get_env_blob(SAVE_FLAG_KEY, &save_flag, sizeof(uint64_t), NULL);
}

#include "waveform_generator.h"

/**
 * @brief 保存波形数据到环境变量中
 *
 * @param index 波形数据的索引号，用于标识不同的波形数据
 */
void save_wave(uint8_t index)
{
  char key[16] = {0};
  sprintf(key, "wave_%d", index + 1);
  printf("%s\n\r", key);
  ef_set_env_blob(key, wave_params, sizeof(ChannelParams_t) * 2);
  ef_save_env();
}
 
/**
 * @brief 加载波形数据到内存中
 *
 * @param index 波形数据的索引号，用于标识不同的波形数据
 */
void load_wave(uint8_t index)
{
  char key[16] = {0};
  sprintf(key, "wave_%d", index + 1);
  printf("%s\n\r", key);
  ef_get_env_blob(key, wave_params, sizeof(ChannelParams_t) * 2, NULL);
}

#include "host_device.h"

// 保存的标志位

/**
 * @brief 保存任意波形数据到环境变量中
 *
 * @param index 波形数据的索引号，用于标识不同的波形数据
 */
void save_arb(uint8_t index)
{
  uint32_t addr = 7 * 1024 * 1024 + (index - 1) * 1024 * 16;
  sfud_flash *flash = sfud_get_device(0);

  sfud_erase_write(flash, addr, USB_BUFFER_SIZE, (const uint8_t *)Recv_data_dac);

  // SET_BIT(save_flag, index);
  // ef_set_env_blob(SAVE_FLAG_KEY, &save_flag, sizeof(uint64_t));
  // ef_save_env();
}

extern uint16_t Waveform[DAC_CH_MAX][MAX_WAVEFORM_POINTS];

void load_arb(uint8_t index, uint8_t ch)
{
  uint32_t addr = 7 * 1024 * 1024 + (index - 1) * 1024 * 16;
  // if (!READ_BIT(save_flag, index))
  // {
  //   return;
  // }
  // printf("load addr %x\n\r", addr);
  sfud_flash *flash = sfud_get_device(0);
  sfud_read(flash, addr, USB_BUFFER_SIZE, (uint8_t *)Waveform[ch]);
  // hexdump_16(Recv_data_dac, USB_BUFFER_SIZE);
}








