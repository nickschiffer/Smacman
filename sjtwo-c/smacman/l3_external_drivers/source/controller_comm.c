

#include "controller_comm.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
static uint16_t      controller_comm__player_1_accel    = 0;
static uint16_t      controller_comm__player_2_accel    = 0;
static QueueHandle_t controller_com__score_update_queue = {0};
/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/
static controller_comm__message_s controller_comm__wait_on_next_message(controller_comm_s controller) {
    // recipient, sender, message_type, message_data_byte1, message_data_byte2, stop_byte
    char message_components[7] = {0};
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
    for (size_t i = 1; i < sizeof(message_components); ++i) {
        if (!uart__get(controller.uart, &message_components[i], controller_comm__message_receive_byte_timeout_ms)){ 
            memset(&message, 0, sizeof(controller_comm__message_s));
            printf("timeout on receive: component #%u\n", i);
            return message;
        }
        // This is a special case for the request accel value message as it is a smaller packet.
        if (i == CONTROLLER_COMM__MESSAGE_COMPONENT_TYPE && (uint8_t)message_components[i] == CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL){
            uart__get(controller.uart, &message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE], controller_comm__message_receive_byte_timeout_ms);
            break;
        }
    }
    if ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_STOP_BYTE] != 0xAD) {
        memset(&message, 0, sizeof(controller_comm__message_s));
        puts("failed stop byte");
        for (int i = 0; i < sizeof(message_components); i++) {
            printf("message components[%d] = %u\n", i, (uint8_t)message_components[i]);
        }   
        return message;
    }
    //puts("successful rx."); fflush(stdout);

#endif
    message.recipient = (controller_comm__role_e) (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_RECIPIENT];
    message.sender    = (controller_comm__role_e) (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_SENDER];
    message.data      = ((uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE2] << 8 
                      |  (uint8_t)message_components[CONTROLLER_COMM__MESSAGE_COMPONENT_DATA_BYTE1]) & 0xFFFF;
#ifdef CONTROLLER_COMM__CHECKSUM
    message.checksum  = (message_checksum_byte2 << 8 | message_checksum_byte1) & 0xFFF;
#endif
    return message;
}
static bool controller_comm__send_message(controller_comm_s controller, controller_comm__message_s message) {
    uint8_t message_data_byte1 =  message.data       & 0xFF;
    uint8_t message_data_byte2 = (message.data >> 8) & 0xFF;
    uint8_t recipient          = (message.recipient    & 0xFF);
    uint8_t sender             = (message.sender       & 0xFF);
    uint8_t message_type       = (message.message_type & 0xFF);

    (void)uart__put(controller.uart, (char)controller_comm__message_start_byte, UINT32_MAX);
    (void)uart__put(controller.uart, (char)recipient,          UINT32_MAX);
    (void)uart__put(controller.uart, (char)sender,             UINT32_MAX);
    (void)uart__put(controller.uart, (char)message_type,       UINT32_MAX);
    if (message_type == CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL) {
        uart__put(controller.uart, (char)controller_comm__message_stop_byte, UINT32_MAX);
        return true;
    }
    (void)uart__put(controller.uart, (char)message_data_byte1, UINT32_MAX);
    (void)uart__put(controller.uart, (char)message_data_byte2, UINT32_MAX);
    (void)uart__put(controller.uart, (char)controller_comm__message_stop_byte, UINT32_MAX);
    return true;
}

static bool controller_comm__update_score(controller_comm__message_s message) {
    // TODO: based on hw config
    return false;
}

static bool controller_comm__handle_received_message(controller_comm_s controller, controller_comm__message_s message) {
    switch (message.message_type) {
        case CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL : {
            #ifdef CONTROLLER_COMM__DEBUG
                puts("Received Request for Accel Value");fflush(stdout);
            #endif
            controller_comm__message_s message_reply = {0};
            message_reply.recipient    = message.sender;
            message_reply.sender       = controller.role;
            message_reply.message_type = CONTROLLER_COMM__MESSAGE_TYPE_SEND_ACCEL_VAL;
            #ifdef CONTROLLER_COMM__USING_ACCEL_FILTER
                message_reply.data         = accel_filter__get_position(); // TODO: which axis do we want?
            #else
                message_reply.data         = acceleration__get_data().x; // TODO: which axis do we want?
            #endif
            return controller_comm__send_message(controller, message_reply);

        } break;

        case CONTROLLER_COMM__MESSAGE_TYPE_UPDATE_SCORE : {
            controller_comm__update_score(message);
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
    controller_comm__send_message(controller, request_message);
    return;
}

// static void controller_com__request_retransmit(controller_comm_s controller){

// }

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

void controller_comm__master_check_for_score_to_send(controller_comm_s controller) {
    controller_comm__score_update_s update = {0};
    if (xQueueReceive(controller_com__score_update_queue, &update, 0)) {
        controller_comm__message_s message = {0};
        message.sender = CONTROLLER_COMM__ROLE_MASTER;
        message.recipient = update.player;
        message.message_type = CONTROLLER_COMM__MESSAGE_TYPE_UPDATE_SCORE;
        message.data = update.score;
        controller_comm__send_message(controller, message);
    }
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
controller_comm_s controller_comm__init(controller_comm__role_e role, uart_e uart){
    controller_comm_s controller = {0};
    controller.role = role;
    controller.uart = uart;
    controller.rx_queue = xQueueCreate(controller_comm__rx_queue_size, sizeof(char));
    controller.tx_queue = xQueueCreate(controller_comm__tx_queue_size, sizeof(char));
    controller_com__score_update_queue = xQueueCreate(controller_comm__score_send_queue_size, sizeof(controller_comm__score_update_s));
    
    uart__init(uart, clock__get_peripheral_clock_hz(), controller_comm__uart_baud_rate);
    uart__enable_queues(controller.uart, controller.rx_queue, controller.tx_queue);
#ifdef CONTROLLER_COMM__USING_ACCEL_FILTER
       xTaskCreate(accel_filter__freertos_task, "accel_filter", (2048U / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
#else
    if (role != CONTROLLER_COMM__ROLE_MASTER){
        (void)acceleration__init();
    }
#endif
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
                controller_comm__master_check_for_score_to_send(controller);
                do {
                    // printf("master sending request\n");
                    controller_comm__master_request_accel(controller, CONTROLLER_COMM__ROLE_PLAYER_1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    reply = controller_comm__wait_on_next_message(controller);
                    message_count++;
                } while (reply.recipient + reply.sender == 0);
                message_success_count++;
                #ifdef CONTROLLER_COMM__DEBUG
                    // puts("Master (me) Requested Accel Value from Player 1\n");
                    // printf("value received: %u\n\n", reply.data);
                    printf("success rate: %lu / %lu\n", message_success_count, message_count);
                #endif
                if (reply.sender == CONTROLLER_COMM__ROLE_PLAYER_1) {
                    controller_comm__player_1_accel = reply.data;
                }
                else if (reply.sender == CONTROLLER_COMM__ROLE_PLAYER_2) {
                    controller_comm__player_2_accel = reply.data;
                }
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            break;
        }
        case CONTROLLER_COMM__ROLE_PLAYER_1:
        case CONTROLLER_COMM__ROLE_PLAYER_2: {
            while(1) {
                controller_comm__message_s message = controller_comm__wait_on_next_message(controller);
                // puts("Message Received");fflush(stdout);
                // printf("role: %d\n",message.recipient);
                if (message.recipient == controller.role) { // Is this message for me?
                    // puts("Controller Received Message");fflush(stdout);
                    
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

bool controller_com__send_score_to_player(controller_comm__role_e role, uint16_t score) {
    controller_comm__score_update_s request = {0};
    request.player = role;
    request.score = score;
    return xQueueSend(controller_com__score_update_queue, &request, 0);
}