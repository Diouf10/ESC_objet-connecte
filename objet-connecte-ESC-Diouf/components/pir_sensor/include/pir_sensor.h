#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "driver/gpio.h"

void pir_init(gpio_num_t pin);
int pir_detected(void);

#endif
