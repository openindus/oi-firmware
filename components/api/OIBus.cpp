#include "OIBus.h"
#include "OISystem.h"


/**********************************************************************************/
/*  SYNC  |  SIZE_DATA  |  ID  |  ACK_NEEDED  |  CRC_HEADER  |  DATA  | CRC_DATA  */
/*--------------------------------------------------------------------------------*/
/*    1   |      1      |   1  |       1      |       1      |    8   |     1     */
/**********************************************************************************/

static const char OI_BUS_TAG[] = "OIBus";

bool OIBusRS::_isInitialized = false;
bool OIBusRS::_waitAck = true;
QueueHandle_t OIBusRS::_uartEventQueueHandle;


void OIBusRS::init(void)
{
    uart_config_t uart_config = {
        .baud_rate = OI_BUS_RS_SPEED,
        .data_bits = OI_BUS_RS_DATA_BIT,
        .parity = OI_BUS_RS_PARITY,
        .stop_bits = OI_BUS_RS_STOP_BITS,
        .flow_ctrl = OI_BUS_RS_FLOW_CONTROL,
        .rx_flow_ctrl_thresh = OI_BUS_RS_RX_FLOW_CTRL_THRESH,
        .source_clk = OI_BUS_RS_SOURCE_CLK,
    };

    /* install UART driver, and get the queue */
    ESP_LOGI(OI_BUS_TAG, "install driver uart");

    ESP_ERROR_CHECK(uart_driver_install(
        OI_BUS_RS_NUM_PORT, 
        OI_BUS_RS_DEFAULT_BUF_SIZE, 
        OI_BUS_RS_DEFAULT_BUF_SIZE, 
        OI_BUS_RS_DEFAULT_QUEUE_SIZE, 
        &_uartEventQueueHandle, 
        0
    ));

    /* configure uart parameters */
    ESP_LOGI(OI_BUS_TAG, "configure uart parameters");

    ESP_ERROR_CHECK(uart_param_config(
        OI_BUS_RS_NUM_PORT, 
        &uart_config
    ));

    /* pin settings */
    ESP_LOGI(OI_BUS_TAG, "set uart pins TX: GPIO_NUM_%u | RX: GPIO_NUM_%u", OI_BUS_PIN_RS_UART_TX, OI_BUS_PIN_RS_UART_RX);

    ESP_ERROR_CHECK(uart_set_pin(
        OI_BUS_RS_NUM_PORT, 
        OI_BUS_PIN_RS_UART_TX, 
        OI_BUS_PIN_RS_UART_RX, 
        UART_PIN_NO_CHANGE, 
        UART_PIN_NO_CHANGE
    ));

    /* set rx full threshold */
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(
        OI_BUS_RS_NUM_PORT, 
        OI_BUS_RS_TRAME_SIZE
    ));

    _isInitialized = true;
}

void OIBusRS::deinit(void)
{
    ESP_LOGI(OI_BUS_TAG, "delete uart driver");
    ESP_ERROR_CHECK(uart_driver_delete(OI_BUS_RS_NUM_PORT));

    _isInitialized = false;
}

void OIBusRS::flush(void)
{
    ESP_LOGV(OI_BUS_TAG, "flush uart RX Fifo");
    ESP_ERROR_CHECK(uart_flush(OI_BUS_RS_NUM_PORT));
}

void OIBusRS::resetEventQueue(void)
{
    xQueueReset(_uartEventQueueHandle);
}

void OIBusRS::writeMessage(OIMessage const& msg, uint8_t id)
{
    uint8_t data[OI_BUS_RS_TRAME_SIZE];
    int length = 0;

    ESP_LOGV(OI_BUS_TAG, "write message (type=0x%02x; id=0x%02x; conf=0x%04x; data=0x%08x)", 
        msg.getType(), msg.getId(), msg.getConf(), msg.getData());

    data[0] = OI_BUS_RS_SYNC_BYTE;
    data[1] = OI_BUS_RS_DATA_LENGTH;
    data[2] = id;
    data[3] = (_waitAck == true) ? 1U : 0U;
    data[4] = _crc(data, OI_BUS_RS_HEADER_LENGTH);
    data[5] = msg.getType();
    data[6] = msg.getId();
    data[7] = msg.getConf() & 0x00FF;
    data[8] = (msg.getConf() & 0xFF00) >> 8;
    data[9] = msg.getData() & 0x000000FF;
    data[10] = (msg.getData() & 0x0000FF00) >> 8;
    data[11] = (msg.getData() & 0x00FF0000) >> 16;
    data[12] = (msg.getData() & 0xFF000000) >> 24;
    data[13] = _crc(&data[OI_BUS_RS_HEADER_LENGTH + 1], OI_BUS_RS_DATA_LENGTH);

    length = uart_write_bytes(OI_BUS_RS_NUM_PORT, (const char *)data, OI_BUS_RS_TRAME_SIZE);
    assert(length == OI_BUS_RS_TRAME_SIZE);
}

int OIBusRS::readMessage(OIMessage& msg, uint8_t& id, TickType_t timeout)
{
    uart_event_t event;
    uint8_t data[OI_BUS_RS_TRAME_SIZE];
    uint8_t length = 0;    

    if (xQueueReceive(_uartEventQueueHandle, (void*) &event, timeout) == pdTRUE)
    {
        if ((event.type == UART_DATA) && (event.size == OI_BUS_RS_TRAME_SIZE)) 
        {
            length = uart_read_bytes (
                OI_BUS_RS_NUM_PORT, 
                data, 
                OI_BUS_RS_TRAME_SIZE, 
                pdMS_TO_TICKS(OI_BUS_RS_DEFAULT_TIMEOUT
            ));

            //Checking message size
            if (length != OI_BUS_RS_TRAME_SIZE) {
                ESP_LOGW(OI_BUS_TAG, "failed to read RS message: incorrect length:%u\n", length);
                return -1;
            }

            // Checking SYNC Byte
            if (data[0] != OI_BUS_RS_SYNC_BYTE) {
                ESP_LOGW(OI_BUS_TAG, "failed to read RS message: incorrect sync byte:%u\n", data[0]);
                return -1;
            }

            /* set wait return message */
            _waitAck = (bool)data[3];

            // Checking crc header
            if (_crc(data, OI_BUS_RS_HEADER_LENGTH) != data[4]) {
                ESP_LOGW(OI_BUS_TAG, "failed to read RS message: incorrect header_crc:%u\n", data[4]);
                return -1;
            }

            // Checking crc data
            if (_crc(&data[OI_BUS_RS_HEADER_LENGTH + 1], OI_BUS_RS_DATA_LENGTH) != data[13]) {
                ESP_LOGW(OI_BUS_TAG, "failed to read RS message: incorrect data_crc:%u", data[13]);
                return -1;
            }

            /* return msg and id */
            msg(&data[5], OI_BUS_RS_DATA_LENGTH);
            id = data[2];

            ESP_LOGV(OI_BUS_TAG, "read message (type=0x%02x; id=0x%02x; conf=0x%04x; data=0x%08x)", 
                msg.getType(), msg.getId(), msg.getConf(), msg.getData());

            return 0;
        }
        else
        {
            ESP_LOGE(OI_BUS_TAG, "unexpected uart event : event = %d, size = %d", event.type, event.size);
            length = uart_read_bytes (
                OI_BUS_RS_NUM_PORT, 
                data, 
                event.size, 
                pdMS_TO_TICKS(OI_BUS_RS_DEFAULT_TIMEOUT
            ));
            return -1;
        }
    }
    else
    {
        ESP_LOGW(OI_BUS_TAG, "RS timeout");
        return -1;
    }
}

uint8_t OIBusRS::_crc(uint8_t *data, uint8_t size)
{
    uint8_t crc = 0;

    for (int i(0); i<size; ++i) {
        crc += data[i];
    }

    return crc;
}


bool OIBusTWAI::_isInitialized = false;

void OIBusTWAI::init(void)
{
    ESP_LOGV(OI_BUS_TAG, "init OIBusTWAI");

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        OI_BUS_PIN_CAN_TX, 
        OI_BUS_PIN_CAN_RX, 
        OI_BUS_TWAI_MODE
    );

    g_config.rx_queue_len = 255;
        
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(); // TODO: add filter for module

    /* install TWAI driver */
    ESP_LOGI(OI_BUS_TAG, "install twai driver");
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));

    ESP_LOGI(OI_BUS_TAG, "set twai pins TX: GPIO_NUM_%u | RX: GPIO_NUM_%u", OI_BUS_PIN_CAN_TX, OI_BUS_PIN_CAN_RX);

    /* Start TWAI driver */
    ESP_LOGI(OI_BUS_TAG, "start twai driver");
    ESP_ERROR_CHECK(twai_start());
    
    _isInitialized = true;
}

void OIBusTWAI::deinit(void)
{
    ESP_LOGV(OI_BUS_TAG, "deinit OIBusTWAI");

    /* Stop TWAI driver */
    ESP_LOGI(OI_BUS_TAG, "stop twai driver");
    ESP_ERROR_CHECK(twai_stop());

    /* uninstall twai driver */
    ESP_LOGI(OI_BUS_TAG, "stop twai driver");
    ESP_ERROR_CHECK(twai_driver_uninstall());

    _isInitialized = false;
}

void OIBusTWAI::writeMessage(OIMessage const& msg, uint8_t id)
{
    twai_message_t msgCan = {
        .extd = 1,
        .identifier = static_cast<uint32_t>(id | (msg.getId() << 8)),
        .data_length_code = 8,
        .data = {}
    };

    ESP_LOGV(OI_BUS_TAG, "write message (type=0x%02x; id=0x%02x; conf=0x%04x; data=0x%08x)", 
        msg.getType(), msg.getId(), msg.getConf(), msg.getData());

    msgCan.data[0] = msg.getType();
    msgCan.data[1] = msg.getId();
    msgCan.data[2] = msg.getConf() & 0x00FF;
    msgCan.data[3] = (msg.getConf() & 0xFF00) >> 8;
    msgCan.data[4] = msg.getData() & 0x000000FF;
    msgCan.data[5] = (msg.getData() & 0x0000FF00) >> 8;
    msgCan.data[6] = (msg.getData() & 0x00FF0000) >> 16;
    msgCan.data[7] = (msg.getData() & 0xFF000000) >> 24;

    ESP_ERROR_CHECK(twai_transmit(&msgCan, pdMS_TO_TICKS(OI_BUS_TWAI_DEFAULT_TIMEOUT)));
}

int OIBusTWAI::readMessage(OIMessage& msg, uint8_t& id, TickType_t timeout)
{
    twai_message_t msgCan;
#ifdef CONFIG_OI_TEST
    ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&msgCan, timeout));
#else
    ESP_ERROR_CHECK(twai_receive(&msgCan, timeout));
#endif
    msg(msgCan.data, msgCan.data_length_code);
    id = (uint8_t) msgCan.identifier;

    ESP_LOGV(OI_BUS_TAG, "read message (type=0x%02x; id=0x%02x; conf=0x%04x; data=0x%08x)", 
        msg.getType(), msg.getId(), msg.getConf(), msg.getData());

    return 0;
}

OIBusRS BusRS;
OIBusTWAI BusTWAI;