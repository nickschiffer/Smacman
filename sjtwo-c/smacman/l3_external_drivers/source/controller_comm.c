

#include "controller_comm.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
// Private data for modules should be rare because all data should be part
// of the module's struct instance

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/
controller_comm__message_s controller_comm__wait_on_next_message(controller_comm_s controller) {
    controller_comm__role_e recipient            = {0};
    controller_comm__role_e sender               = {0};
    controller_comm__message_type_e message_type = {0};
    uint8_t message_data_byte1                   = {0};
    uint8_t message_data_byte2                   = {0};
#ifdef CONTROLLER_COMM__CHECKSUM
    uint8_t message_checksum_byte1               = {0};
    uint8_t message_checksum_byte2               = {0};
#endif

#ifdef CONTROLLER_COMM__CHECKSUM
    uart__get(controller.uart, &sender,                 controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &recipient,              controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_data_byte1,     controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_data_byte2,     controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_checksum_byte1, controller_comm__message_receive_timeout_ms);
    uart__get(controller.uart, &message_checksum_byte2, controller_comm__message_receive_timeout_ms);
#else
    xQueueReceive(controller.rx_queue, &recipient,              portMAX_DELAY);
    xQueueReceive(controller.rx_queue, &sender,                 portMAX_DELAY);
    xQueueReceive(controller.rx_queue, &message_type,           portMAX_DELAY);
    xQueueReceive(controller.rx_queue, &message_data_byte1,     portMAX_DELAY);
    xQueueReceive(controller.rx_queue, &message_data_byte2,     portMAX_DELAY);
#endif
    controller_comm__message_s message = {0};
    message.recipient = recipient;
    message.sender    = sender;
    message.data      = (message_data_byte2 << 8 | message_data_byte1) & 0xFFFF;
#ifdef CONTROLLER_COMM__CHECKSUM
    message.checksum  = (message_checksum_byte2 << 8 | message_checksum_byte1) & 0xFFF;
#endif

    return message;
}
bool controller_comm__send_message(controller_comm_s controller, controller_comm__message_s message) {
    uint8_t message_data_byte1 = message.data & 0xFF;
    uint8_t message_data_byte2 = (message.data >> 8) & 0xFF;

    xQueueSend(controller.tx_queue, &message.recipient,    portMAX_DELAY);
    xQueueSend(controller.tx_queue, &message.sender,       portMAX_DELAY);
    xQueueSend(controller.tx_queue, &message.message_type, portMAX_DELAY);
    xQueueSend(controller.tx_queue, &message_data_byte1,   portMAX_DELAY);
    xQueueSend(controller.tx_queue, &message_data_byte2,   portMAX_DELAY);
    return true;
}

bool controller_comm__update_score(controller_comm__message_s message) {
    // TODO: based on hw config
    return false;
}

bool controller_comm__handle_received_message(controller_comm_s controller, controller_comm__message_s message) {
    switch (message.message_type) {
        case CONTROLLER_COMM__MESSAGE_TYPE_REQUEST_ACCEL_VAL : {
            controller_comm__message_s message_reply = {0};
            message_reply.recipient    = message.sender;
            message_reply.sender       = controller.role;
            message_reply.message_type = CONTROLLER_COMM__MESSAGE_TYPE_SEND_ACCEL_VAL;
            message_reply.data         = acceleration__get_data().x; // TODO: which axis do we want?
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

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/
controller_comm_s controller_comm__init(controller_comm__role_e role, uart_e uart){
    controller_comm_s controller = {.role = role, .uart = uart};
    controller.rx_queue = xQueueCreate(controller_comm__rx_queue_size, sizeof(uint8_t));
    controller.tx_queue = xQueueCreate(controller_comm__tx_queue_size, sizeof(uint8_t));
    
    uart__init(uart, clock__get_peripheral_clock_hz(), controller_comm__uart_baud_rate);
    uart__enable_queues(controller.uart, controller.rx_queue, controller.tx_queue);
    if (role != CONTROLLER_COMM__ROLE_MASTER){
        acceleration__init();
    }
    return controller;
}
void controller_comm__freertos_task(void *controller_comm_struct){
    controller_comm_s controller = *(controller_comm_s *) controller_comm_struct;
    switch (controller.role) {
        case CONTROLLER_COMM__ROLE_MASTER: {
            while(1) {
                // TODO: this is heavily reliant on game logic
            }
            break;
        }
        case CONTROLLER_COMM__ROLE_PLAYER_1:
        case CONTROLLER_COMM__ROLE_PLAYER_2: {
            while(1) {
                controller_comm__message_s message = controller_comm__wait_on_next_message(controller);
                if (message.recipient == controller.role) { // Is this message for me?
                    controller_comm__handle_received_message(controller, message);
                }
            }
            break;
        }
        default:
            fprintf(stderr, "Invalid controller_com role, task failed\n");
            return;

    }
}