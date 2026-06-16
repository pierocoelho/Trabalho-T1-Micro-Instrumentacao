#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define ENA_PIN 0
#define IN1_PIN 1
#define IN2_PIN 2

int main() {
    // Configura e liga o LED da placa imediatamente
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    // Configura IN1 e IN2 como saídas digitais
    gpio_init(IN1_PIN);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    
    gpio_init(IN2_PIN);
    gpio_set_dir(IN2_PIN, GPIO_OUT);

    // Configura o pino ENA para a função PWM
    gpio_set_function(ENA_PIN, GPIO_FUNC_PWM);
    
    // Obtém o slice e o canal do PWM associados ao pino
    uint slice_num = pwm_gpio_to_slice_num(ENA_PIN);
    uint channel = pwm_gpio_to_channel(ENA_PIN);

    // Define o valor máximo do contador (resolução 16-bit)
    pwm_set_wrap(slice_num, 65535);
    
    // Define a velocidade (duty cycle)
    pwm_set_chan_level(slice_num, channel, 45000);
    
    // Habilita o PWM
    pwm_set_enabled(slice_num, true);

    while (true) {
        // Move para frente
        gpio_put(IN1_PIN, 1);
        gpio_put(IN2_PIN, 0);
    }
}