// #include "game_gpio.h"
// #include "lpc40xx.h"
// #include "lpc_peripherals.h"
// int game_gpio_init() {}

// uint32_t *game_gpio_iocon_get(s_gpio io) {
//   uint32_t *lpc_iocon_pointer_to_pin = (uint32_t *)LPC_IOCON;
//   lpc_iocon_pointer_to_pin += (io.port * 32);
//   lpc_iocon_pointer_to_pin += (io.pin);
//   return lpc_iocon_pointer_to_pin;
// }

// s_gpio game_gpio_set_as_function(e_gpio_port which_port, uint8_t which_pin, e_gpio_function which_function) {
//   s_gpio temp_gpio = {which_port, which_pin};

//   /* Enable Power For GPIO*/
//   if (!lpc_peripheral__is_powered_on(LPC_PERIPHERAL__GPIO)) {
//     lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__GPIO);
//   }

//   uint32_t *iocon_pointer_to_pin = game_iocon_get(temp_gpio);
//   uint32_t mask = UINT8_C(7);
//   *iocon_pointer_to_pin &= ~(mask << 0);
// }

// void game_gpio_set_direction(e_direction_gpio direction) {
//   switch (direction) {
//   case INPUT:
//     /* code */
//     break;

//   default:
//     break;
//   }
// }