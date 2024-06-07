#include "Relays.h"

static const char TAG[] = "Relays";

uint8_t Relays::_nb;
const gpio_num_t* Relays::_gpio_num;
uint8_t* Relays::_level;
SemaphoreHandle_t Relays::_mutex;

int Relays::init(const gpio_num_t *gpio, int nb)
{
    int err = 0;

    /* Save number of DOUT */
    _nb = nb;

    /* Init memory and copy gpio numbers in _gpio_num table */
    _gpio_num = gpio;

    /* Init memory of _level */
    _level = (uint8_t*) calloc(nb, sizeof(uint8_t));

    gpio_config_t doutConf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        doutConf.pin_bit_mask |= (1ULL <<_gpio_num[i]);
    }
    err |= gpio_config(&doutConf);
    

    /* Create mutex */
    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    return err;
}

int Relays::digitalWrite(Relay_Num_t num, uint8_t level)
{
    if (num >= _nb)
    {
        ESP_LOGE(TAG, "Invalid RELAY_%d", num+1);
        return -1;
    }

    xSemaphoreTake(_mutex, portMAX_DELAY);
    _level[num] = level;
    xSemaphoreGive(_mutex);
    gpio_set_level(_gpio_num[num], level);

    return 0;
}

int Relays::toggleOutput(Relay_Num_t num)
{
    if (num >= _nb)
    {
        ESP_LOGE(TAG, "Invalid RELAY_%d", num+1);
        return -1;
    }

    xSemaphoreTake(_mutex, portMAX_DELAY);
    _level[num] = !_level[num];
    xSemaphoreGive(_mutex);
    gpio_set_level(_gpio_num[num], _level[num]);

    return 0;
}
