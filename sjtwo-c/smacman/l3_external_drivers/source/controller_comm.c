

#include "controller_comm.h"
// clang-format off

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
static uint16_t      controller_comm__player_1_accel         = 0;
static uint16_t      controller_comm__player_2_accel         = 0;
static uint8_t       controller_comm__player_1_score         = 0;
static uint8_t       controller_comm__player_2_score         = 0;
static bool          controller_comm__button_1_is_pressed    = false;
static bool          controller_comm__button_2_is_pressed    = false;
static QueueHandle_t controller_comm__button_queue           = NULL;
static TickType_t    controller_comm__last_button_press_time = 0;
/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/
static controller_comm__message_s controller_comm__wait_on_next_message(controller_comm_s controller) {
    // recipient, sender, message_type, message_data_byte1, message_data_byte2, stop_byte
    char message_components[7] = {0};
    controller_comm__message_header_t header = {0};
    controller_comm__message_s message = {0};
#ifdef CONTROLLER_COMM__CHECKSUM
    uint8_t message_checksum_byte1 = {0};
    uint8_t message_checksum_byte2 = {0};
#endif

#ifdef CONTROLLER_COMM__CHECKSUM
    uart__get(controller.uart, &sender,                 controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &recipient,              controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_data_byte1,     controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_data_byte2,     controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_checksum_byte1, controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_checksum_byte2, controller_comm__message_receive_timeout_ms);
#else

    if (controller.role != CONTROLLER_COMM__ROLE_MASTER) {
        while ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_START_BYTE] != controller_comm__message_start_byte){
            uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_START_BYTE], controller_comm__message_receive_timeout_ms);
        }
    }
    else {
        uint32_t retry_count = 0;
        while ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_START_BYTE] != controller_comm__message_start_byte){
            uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_START_BYTE], controller_comm__message_receive_timeout_ms);
            if (retry_count++ >= controller_comm__master_rx_retry_threshold) {
                memset(&message, 0, sizeof(controller_comm__message_s));
                puts("Master Timed out on RX");
                return message;
            }
        }
    }
    // printf("valid start_byte received\n");
    if (!uart__get(controller.uart, &header.byte, controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: header\n");
            return message;
    }
    message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_RECIPIENT] = (uint8_t)header.recipient;
    message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_SENDER]    = (uint8_t)header.sender;
    message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_TYPE]      = (uint8_t)header.message_type;

    if (message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_TYPE] == CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL) {
        if (!uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE1], controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: data_byte_1\n");
            return message;
        }
        if (!uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE], controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: stop_byte\n");
            return message;
        }
        if ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE] != controller_comm__message_stop_byte) {
            memset(&message, 0, sizeof(controller_comm__message_s));
            puts("failed stop byte");
            for (int i = 0; i < sizeof(message_components); i++) {
                printf("message components[%d] = %u\n", i, (uint8_t)message_components[i]);
            }   
            return message;
        }
    }
    else {
        if (!uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE1], controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: data_byte_1\n");
            return message;
        }
        if (!uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE2], controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: data_byte_2\n");
            return message;
        }
        if (!uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE], controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: stop_byte\n");
            return message;
        }
        if ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE] != controller_comm__message_stop_byte) {
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("failed stop byte, got %X\n", (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE]);
            for (int i = 0; i < sizeof(message_components); i++) {
                printf("message components[%d] = %u\n", i, (uint8_t)message_components[i]);
            } 
            return message;
        }
    }
    //puts("successful rx."); fflush(stdout);

#endif
    message.recipient = (controller_comm__role_e) (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_RECIPIENT];
    message.sender    = (controller_comm__role_e) (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_SENDER];
    message.message_type = (controller_comm__message_type_e) (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_TYPE];
    message.data      = ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE2] << 8 
                      |  (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE1]) & 0xFFFF;
#ifdef CONTROLLER_COMM__CHECKSUM
    message.checksum  = (message_checksum_byte2 << 8 | message_checksum_byte1) & 0xFFF;
#endif
    return message;
}
static bool controller_comm__send_message(controller_comm_s controller, controller_comm__message_s message) {
    uint8_t message_data_byte1 =  message.data         & 0xFF;
    uint8_t message_data_byte2 = (message.data >> 8)   & 0xFF;

    controller_comm__message_header_t header = {0};
    header.recipient    = ((char)message.recipient    & 0x3);
    header.sender       = ((char)message.sender       & 0x3);
    header.message_type = ((char)message.message_type & 0x3);

    (void)uart__put(controller.uart, (char)controller_comm__message_start_byte, UINT32_MAX);
    (void)uart__put(controller.uart, (char)header.byte,                         UINT32_MAX);

    if (message.message_type == CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL) {
        (void)uart__put(controller.uart, (char)message_data_byte1, UINT32_MAX);
        (void)uart__put(controller.uart, (char)controller_comm__message_stop_byte, UINT32_MAX);
        return true;
    }
    else {
        (void)uart__put(controller.uart, (char)message_data_byte1, UINT32_MAX);
        (void)uart__put(controller.uart, (char)message_data_byte2, UINT32_MAX);
        (void)uart__put(controller.uart, (char)controller_comm__message_stop_byte, UINT32_MAX);
        return true;
    }
}

static bool controller_comm__handle_received_message(controller_comm_s controller, controller_comm__message_s message) {
    switch (message.message_type) {
        case CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL : {
            #ifdef CONTROLLER_COMM__DEBUG
                puts("Received Request for Accel Value");fflush(stdout);
            #endif
            uint8_t new_player_score = (uint8_t)message.data;
            if (controller.role == CONTROLLER_COMM__ROLE_PLAYER_1
                && controller_comm__player_1_score != new_player_score) {
                controller_comm__player_1_score = new_player_score;
                if(!score_display__write_score((uint16_t)new_player_score)) return false;
            }
            else if (controller.role == CONTROLLER_COMM__ROLE_PLAYER_2
                    && controller_comm__player_2_score != new_player_score) {
                controller_comm__player_2_score = (uint8_t)message.data;
                if(!score_display__write_score((uint16_t)new_player_score)) return false;
            }
            controller_comm__message_s message_reply = {0};
            message_reply.recipient    = message.sender;
            message_reply.sender       = controller.role;
            char c = 'n';
            if (xQueueReceive(controller_comm__button_queue, &c, 0)) {
                if (controller.role == CONTROLLER_COMM__ROLE_PLAYER_1) {
                    controller_comm__button_1_is_pressed = true;
                }
                else if (controller.role == CONTROLLER_COMM__ROLE_PLAYER_2) {
                    controller_comm__button_2_is_pressed = true;
                }
            }
            if ((controller.role == CONTROLLER_COMM__ROLE_PLAYER_1 && controller_comm__button_1_is_pressed) ||
                (controller.role == CONTROLLER_COMM__ROLE_PLAYER_2 && controller_comm__button_2_is_pressed)) {
                message_reply.message_type = CONTROLLER_COMM__MESSAGE_TYPE_SEND_ACCEL_VAL_BTN_PRESSED;
                controller_comm__button_1_is_pressed = false;
                controller_comm__button_2_is_pressed = false;
            }
            else {
                message_reply.message_type = CONTROLLER_COMM__MESSAGE_TYPE_SEND_ACCEL_VAL;
            }
            #ifdef CONTROLLER_COMM__USING_ACCEL_FILTER
                message_reply.data         = accel_filter__get_position(); // TODO: which axis do we want?
            #else
                message_reply.data         = acceleration__get_data().x; // TODO: which axis do we want?
            #endif
            return controller_comm__send_message(controller, message_reply);

        } break;
        default:
            return false;
    }
    return true;
}

static void controller_comm__master_request_accel(controller_comm_s controller, controller_comm__role_e player_to_request) {
    controller_comm__message_s request_message = {0};
    request_message.recipient    = player_to_request;
    request_message.sender       = CONTROLLER_COMM__ROLE_MASTER;
    request_message.message_type = CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL;
    if (player_to_request == CONTROLLER_COMM__ROLE_PLAYER_1) {
        request_message.data = controller_comm__player_1_score;
    }
    else if (player_to_request == CONTROLLER_COMM__ROLE_PLAYER_2) {
        request_message.data = controller_comm__player_2_score;
    }
    controller_comm__send_message(controller, request_message);
    return;
}

static void controller_comm__master_update_controller_states(controller_comm__message_s message) {
    if (message.sender == CONTROLLER_COMM__ROLE_PLAYER_1) {
        controller_comm__player_1_accel = message.data;
    }
    else if (message.sender == CONTROLLER_COMM__ROLE_PLAYER_2) {
        controller_comm__player_2_accel = message.data;
    }
    if (message.message_type == CONTROLLER_COMM__MESSAGE_TYPE_SEND_ACCEL_VAL_BTN_PRESSED) {
        if (message.sender == CONTROLLER_COMM__ROLE_PLAYER_1) {
            controller_comm__button_1_is_pressed = true;
        }
        else if (message.sender == CONTROLLER_COMM__ROLE_PLAYER_2) {
            controller_comm__button_2_is_pressed = true;
        }
    }
    else {
        // controller_comm__button_1_is_pressed = false;
        // controller_comm__button_2_is_pressed = false;
    }
}

#ifdef CONTROLLER_COMM__CHECKSUM
uint16_t controller_comm__generate_message_checksum(controller_comm__message_s message) {
    uint16_t checksum = 0;
    checksum ^= message.recipient;
    checksum ^= message.sender;
    checksum ^= message.data;
    return checksum;
}

bool controller_comm__check_message_integrity(controller_comm__message_s message) {
    return (message.checksum == controller_comm__generate_message_checksum(message));
}
#endif

void button_isr() {
    TickType_t current_time_ms =  xTaskGetTickCountFromISR();
    if (current_time_ms < controller_comm__last_button_press_time + controller_comm__button_press_filter_time_ms) {
        return;
    }
    else {
        controller_comm__last_button_press_time = current_time_ms;
        char c = 'p';
        xQueueSendFromISR(controller_comm__button_queue, &c, NULL);
    }
}


/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
controller_comm_s controller_comm__init(controller_comm__role_e role, uart_e uart, gpio_s gpio_tx, gpio_s gpio_rx) {
    controller_comm_s controller = {0};
    controller.role = role;
    controller.uart = uart;
    controller.rx_queue = xQueueCreate(controller_comm__rx_queue_size, sizeof(char));
    controller.tx_queue = xQueueCreate(controller_comm__tx_queue_size, sizeof(char));
    controller.gpio_rx = gpio_rx;
    controller.gpio_tx = gpio_tx;
    
    // set up UART
    gpio__set_as_input(controller.gpio_rx);
    gpio__set_as_output(controller.gpio_tx);
    gpio__set_function(controller.gpio_rx, GPIO__FUNCTION_2);
    gpio__set_function(controller.gpio_tx, GPIO__FUNCTION_2);
    
    uart__init(uart, clock__get_peripheral_clock_hz(), controller_comm__uart_baud_rate);
    uart__enable_queues(controller.uart, controller.rx_queue, controller.tx_queue);
    if (role != CONTROLLER_COMM__ROLE_MASTER) {
        // set up Button
        controller_comm__button_queue = xQueueCreate(1, sizeof(char));
        gpiolab__enable_gpio();
        gpiolab__set_as_input(controller_comm__button_gpio_port, controller_comm__button_gpio_pin);
        gpiolab__attach_interrupt(controller_comm__button_gpio_port, controller_comm__button_gpio_pin, GPIO_INTR__RISING_EDGE, button_isr);
        gpiolab__enable_interrupts();
        if (score_display__init()) {
            puts("Display Init Successful");
        }
#ifdef CONTROLLER_COMM__USING_ACCEL_FILTER
       xTaskCreate(accel_filter__freertos_task, "accel_filter", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
#else
        (void)acceleration__init();
#endif
    }
    return controller;
}
void controller_comm__freertos_task(void *controller_comm_struct){
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("controller Struct &: %p\n", controller_comm_struct);
    controller_comm_s controller = *(controller_comm_s *) controller_comm_struct;
    printf("controller role: %d\n", controller.role);
    printf("controller uart: %d\n", controller.uart);
    switch (controller.role) {
        case CONTROLLER_COMM__ROLE_MASTER: {
            uint32_t message_count = 0;
            uint32_t message_success_count = 0;
            while(1) {
                controller_comm__message_s reply = {0};
                do {
                    // printf("master sending request\n");
                    controller_comm__master_request_accel(controller, CONTROLLER_COMM__ROLE_PLAYER_1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    reply = controller_comm__wait_on_next_message(controller);
                    message_count++;
                } while (reply.recipient + reply.sender == 0);
                message_success_count++;
                controller_comm__master_update_controller_states(reply);
                #ifdef CONTROLLER_COMM__DEBUG
                    puts("Master (me) Requested Accel Value from Player 1\n");
                    printf("value received: %u\n", reply.data);
                    printf("button pressed? %s\n\n", controller_com__get_player_1_button() ? "yes" : "no");
                    //printf("success rate: %lu / %lu\n", message_success_count, message_count);
                #endif
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            break;
        }
        case CONTROLLER_COMM__ROLE_PLAYER_1:
        case CONTROLLER_COMM__ROLE_PLAYER_2: {
            score_display__init();
            while(1) {
                controller_comm__message_s message = controller_comm__wait_on_next_message(controller);
                if (message.recipient == controller.role) { // Is this message for me?
                    controller_comm__handle_received_message(controller, message);
                }
            }
            break;
        }
        default:
            fprintf(stderr, "Invalid controller_comm role, task failed\n");
            return;

    }
}

uint16_t controller_comm__get_player_1_accel() {
    return controller_comm__player_1_accel;
}
uint16_t controller_comm__get_player_2_accel() {
    return controller_comm__player_2_accel;
}

bool controller_comm__update_player_score(controller_comm__role_e player, uint8_t score) {
    if (player == CONTROLLER_COMM__ROLE_PLAYER_1) {
        controller_comm__player_1_score = score;
        return true;
    }
    else if (player == CONTROLLER_COMM__ROLE_PLAYER_2) {
        controller_comm__player_2_score = score;
        return true;
    }
    else {
        return false;
    }

}
bool controller_com__get_player_1_button() {
    if (controller_comm__button_1_is_pressed) {
        controller_comm__button_1_is_pressed = false;
        return true;
    }
    else {
        return false;
    }
}
bool controller_com__get_player_2_button() {
     if (controller_comm__button_2_is_pressed) {
        controller_comm__button_2_is_pressed = false;
        return true;
    }
    else {
        return false;
    }
}
// clang-format on