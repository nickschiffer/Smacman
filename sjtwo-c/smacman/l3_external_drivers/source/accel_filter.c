

#include "accel_filter.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
static uint16_t position = 0;
/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
uint16_t accel_filter__get_position(void) {
    return position;
}

void accel_filter__freertos_task(void *pvParams){
    bool accel_init_status = acceleration__init(); 
    assert(true == accel_init_status);
    while (1) {
        uint32_t sample_sum = 0;
        for (int i = 0; i < accel_filter__sampling_length; ++i) {
            sample_sum += acceleration__get_data().y;
            vTaskDelay(pdMS_TO_TICKS(accel_filter__intersample_delay_ms));
        }
        position = (sample_sum / accel_filter__sampling_length) & 0xFFFF;
        vTaskDelay(pdMS_TO_TICKS(accel_filter__sampling_interval_ms));
    }
}