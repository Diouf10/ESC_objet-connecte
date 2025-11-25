#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H

#include "driver/gpio.h"

void buttons_init(gpio_num_t *pins, int count);
int read_button(void);

#endif
