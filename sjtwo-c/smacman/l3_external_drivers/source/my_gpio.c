#include "my_gpio.h"

void my_gpio_init(int port, int pin_num, int state, struct IO_PORT_PIN *gpio_p) {
  gpio_p->pin_num = pin_num;
  gpio_p->state = state;
  gpio_p->status = 0;
  switch (port) {
  case 0:
    gpio_p->port = LPC_GPIO0;
    break;
  case 1:
    gpio_p->port = LPC_GPIO1;
    break;
  case 2:
    gpio_p->port = LPC_GPIO2;
    break;
  default:
    gpio_p->port = LPC_GPIO0;
    break;
  }

  if (state == OUT) {
    gpio_p->port->PIN |= 1 << (gpio_p->pin_num);
    gpio_p->port->DIR |= 1 << (gpio_p->pin_num);
  } else {
    gpio_p->port->PIN &= ~(1 << (gpio_p->pin_num));
    gpio_p->port->DIR &= ~(1 << (gpio_p->pin_num));
  }
  return;
}

void my_gpio_set(struct IO_PORT_PIN *pin_set) { pin_set->port->SET |= 1 << (pin_set->pin_num); }

void my_gpio_clr(struct IO_PORT_PIN *pin_set) { pin_set->port->CLR |= 1 << (pin_set->pin_num); }

void my_gpio_toggle(struct IO_PORT_PIN *pin_set) { pin_set->port->PIN ^= 1 << (pin_set->pin_num); }

int my_gpio_get(struct IO_PORT_PIN *pin_get) {
  int data;
  data = (pin_get->port)->PIN & (1 << (pin_get->pin_num));
  if (data) {
    return 1;
  } else {
    return 0;
  }
}
// int my_gpio_get(struct IO_PORT_PIN *pin_get) { return 1; }

void my_gpio_board_led_toggle(void) {
  LPC_GPIO1->PIN ^= (1 << 26) | (1 << 24) | (1 << 18);
  LPC_GPIO2->PIN ^= (1 << 3);
  vTaskDelay(500);
}

void my_gpio_board_led_wave(void) {
  LPC_GPIO1->SET |= (1 << 18);
  vTaskDelay(500);
  LPC_GPIO1->SET |= (1 << 24);
  vTaskDelay(500);
  LPC_GPIO1->SET |= (1 << 26);
  vTaskDelay(500);
  LPC_GPIO2->SET |= (1 << 3);
  vTaskDelay(500);
  LPC_GPIO2->CLR |= (1 << 3);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 26);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 24);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 18);
  vTaskDelay(500);
}

void my_gpio_led_piano(void) {
  LPC_GPIO1->SET |= (1 << 18);
  LPC_GPIO2->SET |= (1 << 3);
  LPC_GPIO1->CLR |= (1 << 24);
  LPC_GPIO1->CLR |= (1 << 26);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 18);
  LPC_GPIO2->CLR |= (1 << 3);
  LPC_GPIO1->SET |= (1 << 24);
  LPC_GPIO1->SET |= (1 << 26);
  vTaskDelay(500);
}

void my_gpio_board_led_wave_rev(void) {
  LPC_GPIO2->SET |= (1 << 3);
  vTaskDelay(500);
  LPC_GPIO1->SET |= (1 << 26);
  vTaskDelay(500);
  LPC_GPIO1->SET |= (1 << 24);
  vTaskDelay(500);
  LPC_GPIO1->SET |= (1 << 18);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 18);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 24);
  vTaskDelay(500);
  LPC_GPIO1->CLR |= (1 << 26);
  vTaskDelay(500);
  LPC_GPIO2->CLR |= (1 << 3);
  vTaskDelay(500);
}

void gpio0_set_as_input(uint8_t pin_num) { LPC_GPIO0->DIR &= ~(1 << 29); }

void gpio0_set_as_output(uint8_t pin_num) { LPC_GPIO1->DIR |= (1 << 26); }

void gpio0_set_high(uint8_t pin_num) { LPC_GPIO1->SET |= (1 << 26); }
void gpio0_set_low(uint8_t pin_num) { LPC_GPIO1->CLR |= (1 << 26); }

void gpio0_toggle(uint8_t pin_num) { LPC_GPIO1->PIN ^= (1 << 26); }

void gpio0_set(uint8_t pin_num, bool high) {
  if (high) {
    LPC_GPIO1->PIN &= ~(1 << pin_num);
  } else {
    LPC_GPIO1->PIN |= (1 << pin_num);
  }
}
bool gpio0_get_level(uint8_t pin_num) { return LPC_GPIO0->PIN & (1 << 29); }
