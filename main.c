#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define ENA_PIN 0
#define IN1_PIN 1
#define IN2_PIN 2
#define SENSOR_PIN 26

// Níveis de velocidade do PWM (0 a 65535)
#define VELOCIDADE_0 0
#define VELOCIDADE_1 21626 // ~33%
#define VELOCIDADE_2 32768 // ~50%
#define VELOCIDADE_3 65535 // 100%

// Constantes de tempo do limpador
#define TEMPO_VOLTA_MS 100     // Dedução: tempo para 1 volta física
#define INTERVALO_PAUSA_MS 2000 // Pausa de 2 s

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

    // Variáveis de controle de estado de tempo
    uint32_t ultimo_acionamento = 0;
    bool limpador_ativo = false;

    while (true) {
        uint16_t leitura_adc = adc_read();
        uint32_t velocidade = VELOCIDADE_0;
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

        if (leitura_adc <= 3102 && leitura_adc > 2233) {
            // Entre 2.5 V e 1.8 V
            velocidade = VELOCIDADE_0;
            limpador_ativo = false;
        } else if (leitura_adc <= 2233 && leitura_adc > 1613) {
            // Entre 1.8 V e 1.3 V (Ciclo do Limpador)
            if (!limpador_ativo && (tempo_atual - ultimo_acionamento >= INTERVALO_PAUSA_MS)) {
                limpador_ativo = true;
                ultimo_acionamento = tempo_atual;
            }

            if (limpador_ativo) {
                if (tempo_atual - ultimo_acionamento < TEMPO_VOLTA_MS) {
                    velocidade = VELOCIDADE_3; // Acionamento para vencer o atrito e girar
                } else {
                    limpador_ativo = false;
                    velocidade = VELOCIDADE_0;
                    ultimo_acionamento = tempo_atual; // Inicia a contagem de pausa
                }
            } else {
                velocidade = VELOCIDADE_0;
            }
        } else if (leitura_adc <= 1613 && leitura_adc > 1116) {
            // Entre 1.3 V e 0.9 V
            velocidade = VELOCIDADE_2;
            limpador_ativo = false;
        } else if (leitura_adc <= 1116) {
            // Entre 0.9 V e 0 V
            velocidade = VELOCIDADE_3;
            limpador_ativo = false;
        } else {
            // Acima de 2.5 V
            velocidade = VELOCIDADE_0;
            limpador_ativo = false;
        }

        pwm_set_chan_level(slice_num, channel, velocidade);
        sleep_ms(10); // Loop em alta frequência para estabilidade de tempo
    }
}