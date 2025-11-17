#include <stdint.h>

// CONFIGURACION DEL SISTEMA
#define BANK0_START 0x2007C000
#define BANK0_MUESTRAS 0x20080000

#define CANT_MUESTRAS 2048 // CANT_MUESTRAS DEBE SER < 4095
#define TX_BUFFER_SIZE 2048

#define PPM_UMBRAL_MAX 180
#define PPM_UMBRAL_MIN 30
// Valor umbral para identificar el R-Peak. Valor ajustable
// (255*60%)
#define VAL_UMBRAL 153

// CONFIGURACION PINES
#define PORT_BUZZER 0
#define PIN_BUZZER 8

#define PORT_LED 0
#define PIN_LED 9

// ADC_CHANNEL_0
#define PORT_ADC 0
#define PIN_ADC 23

// UART2
#define PORT_TX_UART2 0
#define PIN_TX_UART2 10

#define PORT_RX_UART2 0
#define PIN_RX_UART2 11

// FUNCION DE PIN
#define FUNC_GPIO 0
#define FUNC_ADC 1
#define FUNC_TX_UART2 1
#define FUNC_RX_UART2 1
#define FUNC_EXTMAT 3

// CONFIGURACION GPIO
#define INPUT 0
#define OUTPUT 1
#define BIT_VALUE(pos) (1 << pos)

// CONFIGURACION TIMER
// Timer PPM
#define TIMER_PS_1MS 1000
#define TIMER_60S 60000

/*
 * Ecuacion del Match Value:
 * - divide por 60 para tener el la cantidad de pps
 * - multiplica por 1000 para tener el resultado final en segundos (ms a s)
 * - divide por 2 dado que se requieren 2 toogles para un periodo
 */
#define TOGGLE_TIME(ppm) (ppm * 1000 / (60 * 2))

// Canales de Match
#define TIMER_CHANNEL_1 1 // Usando en ADC
#define TIMER_CHANNEL_0 0 // Usando en T1 para buzzer y en T2 para ppm

// CONFIGURACION ADC
#define FREQ_MUESTREO 1000 // En Hz

// CONFIGURACION UART
#define BAUD_RATE 9600

// CONFIGURACION GPDMA
#define GPDMA_CHANNEL_ADC 0
#define GPDMA_CHANNEL_UART 1

// Filter IIR
#define NUM_STAGES 1
#define BLOCK_SIZE 1

// Coeficientes
#define G 0.9875889421f
#define B0 (G * 1.0f)
#define B1 (G * -1.902263284f)
#define B2 (G * 1.0f)
#define A1 (G * -1.878654122f)
#define A2 (G * 0.9751778841f)

// PROTOTIPOS DE FUNCIONES
void configPCB(void);
void configGPIO(void);
void configTimerBuzzer(uint32_t matVal);
void configTimerPPM(void);
void configTimerADC(void);
void configADC(void);
void configUART(void);
void configGPDMA_UART(volatile uint8_t *txBuffer);
void startUART_DMA(uint8_t *buffer, volatile uint8_t *dmaUartBusy);
