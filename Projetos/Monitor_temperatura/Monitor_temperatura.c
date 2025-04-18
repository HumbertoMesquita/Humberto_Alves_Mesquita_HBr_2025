#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Função para inicializar o sensor de temperatura interno
void init_temperature_sensor() {
    adc_init();                     // Inicializa o ADC
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura interno
    adc_select_input(4);             // Seleciona o canal 4 (sensor interno)
}

// Função para ler e calcular a temperatura em graus Celsius
float read_temperature() {
    uint16_t raw = adc_read();                       // Lê o valor bruto do ADC (12 bits)
    const float conversion_factor = 3.3f / (1 << 12); // Fator de conversão (3.3V / 4096)
    float voltage = raw * conversion_factor;         // Converte o valor para tensão
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula do datasheet
    return temperature;
}

int main() {
    stdio_init_all();            // Inicializa comunicação com o console (USB Serial)
    init_temperature_sensor();   // Inicializa o sensor de temperatura

    while (true) {
        float temperature = read_temperature();      // Lê a temperatura
        printf("Temperatura = %.2f °C\n", temperature); // Exibe com 2 casas decimais
        sleep_ms(1000);                               // Aguarda 1 segundo
    }
}
