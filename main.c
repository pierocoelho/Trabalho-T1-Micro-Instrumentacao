#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

// --- Pinos do Motor e Sensor ---
#define ENA_PIN 0
#define IN1_PIN 1
#define IN2_PIN 2
#define SENSOR_PIN 26

// --- Pinos do LCD ---
#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 10
#define LCD_D5 11
#define LCD_D6 12
#define LCD_D7 13

// --- Parâmetros de Funcionamento ---
#define VELOCIDADE_TRABALHO 32768 // 50% do PWM
#define TEMPO_VOLTA_MS 500       // Tempo de 0,5 s para 1 volta física

// --- Funções do LCD ---
void lcd_pulse_enable() {
    gpio_put(LCD_EN, 1);
    sleep_us(1);
    gpio_put(LCD_EN, 0);
    sleep_us(50);
}

void lcd_send_nibble(uint8_t nibble) {
    gpio_put(LCD_D4, (nibble >> 0) & 1);
    gpio_put(LCD_D5, (nibble >> 1) & 1);
    gpio_put(LCD_D6, (nibble >> 2) & 1);
    gpio_put(LCD_D7, (nibble >> 3) & 1);
    lcd_pulse_enable();
}

void lcd_send_byte(uint8_t val, int mode) {
    gpio_put(LCD_RS, mode);
    lcd_send_nibble(val >> 4);
    lcd_send_nibble(val & 0x0F);
}

void lcd_init() {
    uint pins[] = {LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7};
    for (int i = 0; i < 6; i++) {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_OUT);
        gpio_put(pins[i], 0);
    }

    sleep_ms(50);
    gpio_put(LCD_RS, 0);
    lcd_send_nibble(0x03); sleep_ms(5);
    lcd_send_nibble(0x03); sleep_us(150);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);

    lcd_send_byte(0x28, 0); 
    lcd_send_byte(0x0C, 0); 
    lcd_send_byte(0x01, 0); 
    sleep_ms(2);
    lcd_send_byte(0x06, 0); 
}

void lcd_putstr(const char *str) {
    while (*str) {
        lcd_send_byte(*str++, 1);
    }
}

void lcd_move_to(int row, int col) {
    int row_offsets[] = {0x00, 0x40};
    lcd_send_byte(0x80 | (col + row_offsets[row]), 0);
}

// --- Função Principal ---
int main() {
    // Inicializa LED Integrado
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1); // Mantém ligado indicando funcionamento

    // Inicializa Pinos de Direção do Motor
    gpio_init(IN1_PIN);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_init(IN2_PIN);
    gpio_set_dir(IN2_PIN, GPIO_OUT);

    // Inicializa PWM do Motor
    gpio_set_function(ENA_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(ENA_PIN);
    uint channel = pwm_gpio_to_channel(ENA_PIN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_enabled(slice_num, true);

    // Inicializa ADC (Sensor)
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    // Inicializa LCD
    lcd_init();
    lcd_move_to(0, 0);
    lcd_putstr("Iniciando...");

    // Define sentido de rotação inicial
    gpio_put(IN1_PIN, 1);
    gpio_put(IN2_PIN, 0);

    // Variáveis de controle de estado
    uint32_t ultimo_acionamento = 0;
    bool limpador_ativo = false;
    int estado_anterior_lcd = -2; // Valor impossível inicial para forçar a primeira escrita

    while (true) {
        uint16_t leitura_adc = adc_read();
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
        uint32_t velocidade = 0;
        int pausa_alvo = -1; // -1 indica estado desligado

        // Determina a faixa de atuação pelo valor analógico
        if (leitura_adc <= 3723 && leitura_adc > 2233) {
            pausa_alvo = -1;
        } else if (leitura_adc <= 2233 && leitura_adc > 1613) {
            pausa_alvo = 4000;
        } else if (leitura_adc <= 1613 && leitura_adc > 1116) {
            pausa_alvo = 2000;
        } else if (leitura_adc <= 1116) {
            pausa_alvo = 0;
        }

        // Lógica de atualização do Display LCD
        if (pausa_alvo != estado_anterior_lcd) {
            lcd_move_to(0, 0);
            if (pausa_alvo == -1) {
                lcd_putstr("Motor: DESLIGADO");
            } else if (pausa_alvo == 4000) {
                lcd_putstr("Motor: NIVEL 1  ");
            } else if (pausa_alvo == 2000) {
                lcd_putstr("Motor: NIVEL 2  ");
            } else {
                lcd_putstr("Motor: NIVEL 3  ");
            }
            
            // Limpa a segunda linha
            lcd_move_to(1, 0);
            lcd_putstr("                ");
            
            estado_anterior_lcd = pausa_alvo;
        }

        // Lógica de temporização do motor (Limpador)
        if (pausa_alvo == -1) {
            velocidade = 0;
            limpador_ativo = false;
        } else {
            if (!limpador_ativo && (tempo_atual - ultimo_acionamento >= pausa_alvo)) {
                limpador_ativo = true;
                ultimo_acionamento = tempo_atual;
            }

            if (limpador_ativo) {
                if (tempo_atual - ultimo_acionamento < TEMPO_VOLTA_MS) {
                    velocidade = VELOCIDADE_TRABALHO;
                } else {
                    limpador_ativo = false;
                    velocidade = 0;
                    ultimo_acionamento = tempo_atual;
                }
            }
        }

        pwm_set_chan_level(slice_num, channel, velocidade);
        sleep_ms(10);
    }
    return 0;
}