

#include "score_display.h"
// clang-format off
/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
static const uint8_t score_display__number_table[] = {
	0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6F, /* 9 */
	0x77, /* a */
	0x7C, /* b */
	0x39, /* C */
	0x5E, /* d */
	0x79, /* E */
	0x71, /* F */
};

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
bool score_display__init() {
    gpio__construct_with_function(SCORE_DISPLAY__SDA_GPIO_PORT, SCORE_DISPLAY__SDA_GPIO_PIN, GPIO__FUNCTION_2);  //setup GPIO pins
    gpio__construct_with_function(SCORE_DISPLAY__SCL_GPIO_PORT, SCORE_DISPLAY__SCL_GPIO_PIN, GPIO__FUNCTION_2);  //setup GPIO pins
    if(!i2c__write_single(SCORE_DISPLAY__I2C_DEVICE,SCORE_DISPLAY__I2C_ADDRESS, HT16K33_CMD_OSCILLATOR | HT16K33_ON, 0x00)) return false; // turn on oscillator
    if(!score_display__set_blink_rate(HT16K33_BLINK_OFF)) return false;
    if(!score_display__set_brightness(HT16K33_MAX_BRIGHTNESS)) return false; // set brightness to max
    if(!score_display__write_score(0)) return false;
    return true;
}
bool score_display__write_score(uint16_t score) {
    if (score > 9999) return false;

    uint8_t digits[4] = {0};

    digits[3] =  score         % 10;
    digits[2] = (score / 10)   % 10;
    digits[1] = (score / 100)  % 10;
    digits[0] = (score / 1000) % 10;
    
    if(!i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_DIGIT0, score_display__number_table[digits[0]])) return false;
    if(!i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_DIGIT1, score_display__number_table[digits[1]])) return false;
    if(!i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_SEMICOLON, 0xFF)) return false;
    if(!i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_DIGIT2, score_display__number_table[digits[2]])) return false;
    if(!i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_DIGIT3, score_display__number_table[digits[3]])) return false;
    return true;
}
bool score_display__set_brightness(uint8_t brightness) {
    if (brightness < HT16K33_MIN_BRIGHTNESS || brightness > HT16K33_MAX_BRIGHTNESS) return false;
    return i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_CMD_BRIGHTNESS | brightness, 0x00);
}

bool score_display__set_blink_rate(score_display__blink_rate_e blink_rate) {
    if (blink_rate < HT16K33_BLINK_OFF || blink_rate > HT16K33_BLINK_HALFHZ) return false;
    return i2c__write_single(SCORE_DISPLAY__I2C_DEVICE, SCORE_DISPLAY__I2C_ADDRESS, HT16K33_BLINK_CMD | HT16K33_ON | (blink_rate << 1), 0x00);
}
// clang-format on