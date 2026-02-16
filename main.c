#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define BUTTON_GPIO 18
#define DEBOUNCE_US 30000  // 30 ms

static const char *TAG = "button";

static esp_timer_handle_t debounce_timer;

static void debounce_timer_cb(void *arg) {
    int level = gpio_get_level(BUTTON_GPIO);

    if (level == 0) {  // assuming active low
        ESP_LOGI(TAG, "Button pressed (debounced)");
    }
}

static void IRAM_ATTR button_isr(void *arg) {
    esp_timer_stop(debounce_timer);
    esp_timer_start_once(debounce_timer, DEBOUNCE_US); }

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&io_conf);

    esp_timer_create_args_t timer_args = {
        .callback = debounce_timer_cb,
        .name = "debounce"
    };
    esp_timer_create(&timer_args, &debounce_timer);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr, NULL); } 
