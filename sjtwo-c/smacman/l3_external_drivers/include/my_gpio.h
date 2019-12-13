#ifndef my_gpio
#define my_gpio

#include "LPC40xx.h"
#include "delay.h"
#include <stdbool.h>
#include <stdint.h>

#define IN 1
#define OUT 0

typedef struct IO_PORT_PIN {
  LPC_GPIO_TypeDef *port;
  int pin_num;
  int state;
  int status;
} IO_PORT_PIN;

void my_gpio_init(int port, int pin_num, int state, struct IO_PORT_PIN *gpio_p);
void my_gpio_set(struct IO_PORT_PIN *pin_set);
void my_gpio_clr(struct IO_PORT_PIN *pin_set);
int my_gpio_get(struct IO_PORT_PIN *pin_get);
void my_gpio_toggle(struct IO_PORT_PIN *pin_get);
void my_gpio_board_led_toggle(void);
void my_gpio_board_led_wave(void);
void my_gpio_led_piano(void);
void my_gpio_board_led_wave_rev(void);

void gpio0_set_as_input(uint8_t pin_num);
void gpio0_set_as_output(uint8_t pin_num);
void gpio0_set_high(uint8_t pin_num);
void gpio0_set_low(uint8_t pin_num);
void gpio0_set(uint8_t pin_num, bool high);
bool gpio0_get_level(uint8_t pin_num);
void gpio0_toggle(uint8_t pin_num);

#endif