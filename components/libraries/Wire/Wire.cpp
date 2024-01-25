#include "Wire.h"

#define ACK_CHECK_EN                    (0x01)  /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                   (0x00)  /*!< I2C master will not check ack from slave */

static const char OIWIRE_TAG[] = "TwoWire";

TwoWire::TwoWire(uint8_t bus_num)
    :num(bus_num & 0)
    ,i2cStarted(false)
    ,rxIndex(0)
    ,rxLength(0)
    ,rxQueued(0)
    ,txIndex(0)
    ,txLength(0)
    ,txAddress(0)
    ,txQueued(0)
    ,transmitting(0)
    ,last_error(ESP_OK)
    ,_timeOutMillis(50)
{}

TwoWire::~TwoWire()
{
    flush();
    i2cStarted = false;
}

bool TwoWire::setPins(int sdaPin, int sclPin)
{
    // Pins are cannot be changed
    return true;
}

bool TwoWire::begin(int sdaPin, int sclPin, uint32_t frequency)
{
    i2cStarted = true;
    return true;
}

void TwoWire::setClock(uint32_t frequency)
{
    // Clock cannot be changed
}

size_t TwoWire::getClock()
{
    return (size_t)100000;
}

void TwoWire::setTimeOut(uint16_t timeOutMillis)
{
    _timeOutMillis = timeOutMillis;
}

uint16_t TwoWire::getTimeOut()
{
    return _timeOutMillis;
}

void TwoWire::beginTransmission(uint16_t address)
{
    transmitting = 1;
    txAddress = address;
    txIndex = txQueued; // allow multiple beginTransmission(),write(),endTransmission(false) until endTransmission(true)
    txLength = txQueued;
    last_error = ESP_OK;
}

uint8_t TwoWire::endTransmission(bool sendStop)
{
    if(transmitting == 1) 
    {
        // txlength is howmany bytes in txbuffer have been use
        last_error = writeTransmission(txAddress, &txBuffer[txQueued], txLength - txQueued, sendStop);
        if(last_error == ESP_ERR_NOT_FOUND) // Should be I2C_ERROR_CONTINUE
        {
            txQueued = txLength;
        } 
        else if( last_error == ESP_OK)
        {
            rxIndex = 0;
            rxLength = rxQueued;
            rxQueued = 0;
            txQueued = 0; // the SendStop=true will restart all Queueing
        }
    } 
    else 
    {
        last_error = ESP_ERR_INVALID_STATE;
        flush();
    }
    txIndex = 0;
    txLength = 0;
    transmitting = 0;
    return (last_error ==  ESP_ERR_NOT_FOUND)?ESP_OK:last_error;
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t size, bool sendStop)
{
    //use internal Wire rxBuffer, multiple requestFrom()'s may be pending, try to share rxBuffer
    uint32_t cnt = rxQueued; // currently queued reads, next available position in rxBuffer
    if (cnt < (I2C_BUFFER_LENGTH-1) && (size + cnt) <= I2C_BUFFER_LENGTH) 
    { // any room left in rxBuffer
        rxQueued += size;
    } 
    else 
    { // no room to receive more!
        // log_e("rxBuff overflow %d", cnt + size);
        cnt = 0;
        last_error = ESP_ERR_NO_MEM;
        flush();
        return cnt;
    }

    last_error = readTransmission(address, &rxBuffer[cnt], size, sendStop, &cnt);
    rxIndex = 0;
  
    rxLength = cnt;
  
    if (last_error != ESP_ERR_NOT_FOUND)  // Should be I2C_ERROR_CONTINUE
    { 
        // not a  buffered ReSTART operation
        // so this operation actually moved data, queuing is done.
        rxQueued = 0;
        txQueued = 0; // the SendStop=true will restart all Queueing or error condition
    }
  
    if (last_error != ESP_OK)
    { 
        // ReSTART on read does not return any data
        cnt = 0;
    }

    return cnt;
}

size_t TwoWire::write(uint8_t data)
{
    if(transmitting) 
    {
        if(txLength >= I2C_BUFFER_LENGTH) 
        {
            last_error = ESP_ERR_NO_MEM;
            return 0;
        }
        txBuffer[txIndex] = data;
        ++txIndex;
        txLength = txIndex;
        return 1;
    }
    last_error = ESP_ERR_INVALID_STATE; // no begin, not transmitting
    return 0;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
    for(size_t i = 0; i < quantity; ++i) 
    {
        if(!write(data[i])) 
        {
            return i;
        }
    }
    return quantity;
}

int TwoWire::available(void)
{
    int result = rxLength - rxIndex;
    return result;
}

int TwoWire::read(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
        ++rxIndex;
    }
    return value;
}

int TwoWire::peek(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
    }
    return value;
}

void TwoWire::flush(void)
{
    rxIndex = 0;
    rxLength = 0;
    txIndex = 0;
    txLength = 0;
    rxQueued = 0;
    txQueued = 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(address, static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity)
{
    return requestFrom(address, static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
    return static_cast<uint8_t>(requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop)));
}

void TwoWire::beginTransmission(int address)
{
    beginTransmission(static_cast<uint16_t>(address));
}

void TwoWire::beginTransmission(uint8_t address)
{
    beginTransmission(static_cast<uint16_t>(address));
}

uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

esp_err_t TwoWire::writeTransmission(uint16_t address, uint8_t* buff, uint16_t size, bool sendStop)
{
    ESP_LOGV(OIWIRE_TAG, "WRITE %i byte(s) to %#04x ", size, address);
    for (int i = 0; i < size; i++)
        ESP_LOGV(OIWIRE_TAG, "%#02x", buff[i]);
    ESP_LOGV(OIWIRE_TAG, "");

    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, buff, size, ACK_CHECK_EN);
    if (sendStop)
    {
        i2c_master_stop(cmd);
    }
    ret = i2c_master_cmd_begin(num, cmd, _timeOutMillis / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t TwoWire::readTransmission(uint16_t address, uint8_t* buff, uint16_t size, bool sendStop, uint32_t *readCount)
{
    ESP_LOGV(OIWIRE_TAG, "READ %i byte(s) to %#04x: ", size, address);
    
    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read(cmd, buff, size, I2C_MASTER_LAST_NACK);
    if (sendStop)
    {
        i2c_master_stop(cmd);
    }
    ret = i2c_master_cmd_begin(num, cmd, _timeOutMillis / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    *readCount += size;

    for (int i = 0; i < size; i++)
        ESP_LOGV(OIWIRE_TAG, "%#02x", buff[i]);
    ESP_LOGV(OIWIRE_TAG, "");

    return ret;
}

TwoWire Wire = TwoWire(0);