#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

// -----------------------------
// Definições dos pinos
// -----------------------------
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define LED_RGB_RED_PIN 13
#define LED_RGB_GREEN_PIN 11
#define LED_RGB_BLUE_PIN 12
#define DEBOUNCE_DELAY 200 // Debounce em milissegundos

// -----------------------------
// Variáveis globais
// -----------------------------
int display_number = 0; // Número a ser exibido (0 a 9)
uint64_t last_interrupt_time_a = 0;
uint64_t last_interrupt_time_b = 0;

uint32_t led_buffer[NUM_PIXELS] = {0};

// -----------------------------
// Funções auxiliares
// -----------------------------
// Envia um pixel para o PIO do WS2812
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb);
}

// Converte valores de cor para o formato GRB usado pelo WS2812
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// -----------------------------
// Padrões para os dígitos (0 a 9) em uma matriz 5x5
// -----------------------------
static const uint8_t digit_patterns[10][5][5] = {
    // Dígito 0 
    {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    // Dígito 1 
    {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0}
    },
    // Dígito 2 
    {
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0}
    },
    // Dígito 3 
    {
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0}
    },
    // Dígito 4 
    {
        {0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0},
        {0, 1, 0, 0, 1}
    },
    // Dígito 5 
    {
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0}
    },
    // Dígito 6 
    {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0}
    },
    // Dígito 7 
    {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    // Dígito 8 
    {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0}
    },
    // Dígito 9 
    {
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0}
    }
};


// Atualiza o buffer dos LEDs com o padrão do dígito atual
void display_number_on_leds(int number)
{
    uint32_t on_color = urgb_u32(0xFF, 0xFF, 0xFF);
    uint32_t off_color = urgb_u32(0, 0, 0);

    for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < 5; c++)
        {
            int index = r * 5 + c;
            led_buffer[index] = (digit_patterns[number][r][c] == 1) ? on_color : off_color;
        }
    }
}

void ws2812_send()
{
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        put_pixel(led_buffer[i]);
    }
}

// -----------------------------
// Função para piscar o LED vermelho do LED RGB
// -----------------------------
void led_rgb_blink()
{
    static uint64_t last_blink_time = 0;
    static bool led_on = false;
    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_blink_time >= 100)
    {
        gpio_put(LED_RGB_RED_PIN, led_on);
        led_on = !led_on;
        last_blink_time = current_time;
    }
}

// -----------------------------
// Callback de interrupção para os botões
// -----------------------------
void gpio_callback(uint gpio, uint32_t events)
{
    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    if (gpio == BUTTON_A_PIN)
    {
        if (current_time - last_interrupt_time_a >= DEBOUNCE_DELAY)
        {
            display_number = (display_number + 1) % 10;
            last_interrupt_time_a = current_time;
        }
    }
    else if (gpio == BUTTON_B_PIN)
    {
        if (current_time - last_interrupt_time_b >= DEBOUNCE_DELAY)
        {
            display_number = (display_number - 1 + 10) % 10;
            last_interrupt_time_b = current_time;
        }
    }
}

// -----------------------------
// Função principal
// -----------------------------
int main()
{
    stdio_init_all();

    // Inicializa o PIO e carrega o programa WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    // Configura os botões com pull-up e ativa interrupções
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    // Configura os pinos do LED RGB
    gpio_init(LED_RGB_RED_PIN);
    gpio_set_dir(LED_RGB_RED_PIN, GPIO_OUT);
    gpio_init(LED_RGB_GREEN_PIN);
    gpio_set_dir(LED_RGB_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RGB_BLUE_PIN);
    gpio_set_dir(LED_RGB_BLUE_PIN, GPIO_OUT);
    // Desliga inicialmente todos os canais do LED RGB
    gpio_put(LED_RGB_RED_PIN, 0);
    gpio_put(LED_RGB_GREEN_PIN, 0);
    gpio_put(LED_RGB_BLUE_PIN, 0);

    // Loop principal
    while (1)
    {
        display_number_on_leds(display_number);
        ws2812_send();

        led_rgb_blink();

        sleep_ms(10); 
    }

    return 0;
}
