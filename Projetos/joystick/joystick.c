#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "inc/ssd1306.h"

// Definições dos pinos I2C
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15

// Funções auxiliares
void init_i2c_display() {
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    ssd1306_init();
}

void init_adc_joystick() {
    adc_init();
    adc_gpio_init(26); // Y-axis (ADC0)
    adc_gpio_init(27); // X-axis (ADC1)
}

uint16_t read_adc_channel(uint channel) {
    adc_select_input(channel);
    return adc_read();
}

void update_display(uint8_t *buffer, struct render_area *area, uint16_t x_value, uint16_t y_value) {
    memset(buffer, 0, ssd1306_buffer_length);

    char text[32];
    snprintf(text, sizeof(text), "Joystick X: %4u", x_value);
    ssd1306_draw_string(buffer, 0, 0, text);

    snprintf(text, sizeof(text), "Joystick Y: %4u", y_value);
    ssd1306_draw_string(buffer, 0, 10, text);

    render_on_display(buffer, area);
}

int main() {
    stdio_init_all();

    init_i2c_display();
    init_adc_joystick();

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    uint8_t display_buffer[ssd1306_buffer_length];

    while (true) {
        uint16_t adc_y = read_adc_channel(0); // Y-axis
        uint16_t adc_x = read_adc_channel(1); // X-axis

        update_display(display_buffer, &frame_area, adc_x, adc_y);
        sleep_ms(100);
    }

    return 0;
}
