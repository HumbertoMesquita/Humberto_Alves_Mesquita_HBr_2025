#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

// Definições de pinos e constantes
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define BOTAO_A_PIN 5
#define BOTAO_B_PIN 6
#define DEBOUNCE_DELAY_US 200000

// Variáveis globais voláteis
volatile int contador = 0;
volatile int cliques_botao_b = 0;
volatile bool contagem_ativa = false;
volatile bool atualizar_display = true;
volatile uint32_t ultimo_debounce_a = 0;
volatile uint32_t ultimo_debounce_b = 0;

// Área de renderização do display
struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

uint8_t buffer_display[ssd1306_buffer_length];

// Funções de interrupção
void botao_a_callback() {
    uint32_t agora = time_us_32();
    if (agora - ultimo_debounce_a < DEBOUNCE_DELAY_US) return;
    ultimo_debounce_a = agora;

    contador = 9;
    cliques_botao_b = 0;
    contagem_ativa = true;
    atualizar_display = true;
}

void botao_b_callback() {
    uint32_t agora = time_us_32();
    if (agora - ultimo_debounce_b < DEBOUNCE_DELAY_US) return;
    ultimo_debounce_b = agora;

    if (contagem_ativa && contador > 0) {
        cliques_botao_b++;
        atualizar_display = true;
    }
}

void gpio_callback(uint gpio, uint32_t eventos) {
    if (gpio == BOTAO_A_PIN) botao_a_callback();
    else if (gpio == BOTAO_B_PIN) botao_b_callback();
}

// Função para atualizar o display
void atualizar_oled() {
    memset(buffer_display, 0, sizeof(buffer_display));

    char linha1[20], linha2[20], linha3[30];
    sprintf(linha1, "Contador: %d", contador);
    sprintf(linha2, "Cliques: %d", cliques_botao_b);
    sprintf(linha3, "%s", contagem_ativa ? (contador > 0 ? "Contagem ativa" : "Press A para reiniciar") : "Press A para iniciar");

    ssd1306_draw_string(buffer_display, 5, 0, linha1);
    ssd1306_draw_string(buffer_display, 5, 16, linha2);
    ssd1306_draw_string(buffer_display, 5, 32, linha3);

    render_on_display(buffer_display, &frame_area);
}

void setup() {
    stdio_init_all();

    // Inicialização do I2C e OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    render_on_display(buffer_display, &frame_area);

    // Configuração dos botões
    gpio_init(BOTAO_A_PIN);
    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);
    gpio_pull_up(BOTAO_B_PIN);

    gpio_set_irq_enabled_with_callback(BOTAO_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BOTAO_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    // Primeira tela
    atualizar_oled();
}

int main() {
    setup();

    absolute_time_t ultimo_segundo = get_absolute_time();

    while (true) {
        if (atualizar_display) {
            atualizar_oled();
            atualizar_display = false;
        }

        if (contagem_ativa && contador > 0 && absolute_time_diff_us(ultimo_segundo, get_absolute_time()) >= 1000000) {
            contador--;
            atualizar_display = true;
            ultimo_segundo = get_absolute_time();
            if (contador == 0) contagem_ativa = false;
        }

        sleep_ms(10);
    }
}
