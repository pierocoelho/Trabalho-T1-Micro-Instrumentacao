#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define ENA_PIN 0
#define IN1_PIN 1
#define IN2_PIN 2
#define SENSOR_PIN 26

// Parâmetros de funcionamento
#define VELOCIDADE_TRABALHO 32768 // 50% do PWM
#define TEMPO_VOLTA_MS 500       // Tempo para 1 volta física

int main() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    gpio_init(IN1_PIN);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_init(IN2_PIN);
    gpio_set_dir(IN2_PIN, GPIO_OUT);

    gpio_set_function(ENA_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(ENA_PIN);
    uint channel = pwm_gpio_to_channel(ENA_PIN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_enabled(slice_num, true);

    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    gpio_put(IN1_PIN, 1);
    gpio_put(IN2_PIN, 0);

    // Variáveis de tempo e estado
    uint32_t ultimo_acionamento = 0;
    bool limpador_ativo = false;

    while (true) {
        uint16_t leitura_adc = adc_read();
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
        uint32_t velocidade = 0;
        int pausa_alvo = -1; // -1 indica estado desligado

        // Determina o tempo de pausa com base na leitura analógica
        if (leitura_adc <= 3723 && leitura_adc > 2233) {
            pausa_alvo = -1; // Entre 3.0 V e 1.8 V (Desligado)
        } else if (leitura_adc <= 2233 && leitura_adc > 1613) {
            pausa_alvo = 4000; // Nível 1: 4 s de pausa
        } else if (leitura_adc <= 1613 && leitura_adc > 1116) {
            pausa_alvo = 2000; // Nível 2: 2 s de pausa
        } else if (leitura_adc <= 1116) {
            pausa_alvo = 0;    // Nível 3: Contínuo
        }

        // Lógica de atuação do limpador
        if (pausa_alvo == -1) {
            velocidade = 0;
            limpador_ativo = false;
        } else {
            // Verifica se é hora de iniciar um novo ciclo
            if (!limpador_ativo && (tempo_atual - ultimo_acionamento >= pausa_alvo)) {
                limpador_ativo = true;
                ultimo_acionamento = tempo_atual;
            }

            // Mantém o motor ligado até completar a volta
            if (limpador_ativo) {
                if (tempo_atual - ultimo_acionamento < TEMPO_VOLTA_MS) {
                    velocidade = VELOCIDADE_TRABALHO;
                } else {
                    limpador_ativo = false;
                    velocidade = 0;
                    ultimo_acionamento = tempo_atual; // Reinicia a contagem para a próxima pausa
                }
            }
        }

        pwm_set_chan_level(slice_num, channel, velocidade);
        sleep_ms(10);
    }
}