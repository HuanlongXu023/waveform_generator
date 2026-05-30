#ifndef __HOST_DEVICE_H
#define __HOST_DEVICE_H

#include "main.h"
#include "waveform_generator.h"

void host_device_task_init(void);

void host_task(void);

#define USB_BUFFER_SIZE (MAX_WAVEFORM_POINTS * 2 + 1)

extern uint16_t Recv_data_dac[USB_BUFFER_SIZE / 2];

#endif
