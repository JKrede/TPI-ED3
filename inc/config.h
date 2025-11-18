#include <stdint.h>

#define TX_BUFFER_SIZE 2048

#define PPM_UMBRAL_MAX 100
#define PPM_UMBRAL_MIN 60
// Valor umbral para identificar el R-Peak. Valor ajustable
// (255*60%)
#define VAL_UMBRAL 153

// CONFIGURACION PINES se cambia al 0 22 porque no funciono el buzzer
#define PORT_LED_GREEN 3
#define PIN_LED_GREEN 25

#define PORT_LED_RED 0
#define PIN_LED_RED 22

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

// Canales de Match
#define TIMER_CHANNEL_1 1 // Usando en ADC
#define TIMER_CHANNEL_0 0 // Usado en T2 para ppm

// CONFIGURACION UART
#define BAUD_RATE 9600

// CONFIGURACION GPDMA
#define GPDMA_CHANNEL_ADC 0
#define GPDMA_CHANNEL_UART 1

// Filtro IIR de 1 etapa y un block size de 1 para filtrado en tiempo real
#define NUM_STAGES 1
#define BLOCK_SIZE 1

// Coeficientes del IIR
#define G 0.9875889421f
#define B0 (G * 1.0f)
#define B1 (G * -1.902263284f)
#define B2 (G * 1.0f)
#define A1 (G * -1.878654122f)
#define A2 (G * 0.9751778841f)

// PROTOTIPOS DE FUNCIONES
void configPCB(void);
void configGPIO(void);
void configTimerPPM(void);
void configTimerADC(void);
void configADC(void);
void configUART(void);
void configGPDMA_UART(volatile uint8_t *txBuffer);
void startUART_DMA(uint8_t *buffer, volatile uint8_t *dmaUartBusy);
