
#define BANK0_START 0x2007C000
// CANT_MUESTRAS < 4096
#define CANT_MUESTRAS 1024

// CONFIGURACION PINSEL
#define PORT_BUZZER 0
#define PIN_BUZZER 8

#define PORT_LED 0
#define PIN_LED 9

// Para ADC_CHANNEL_0
#define PORT_ADC 0
#define PIN_ADC 23

#define PORT_TX_UART0 0
#define PIN_TX_UART0 2

#define PORT_RX_UART0 0
#define PIN_RX_UART0 3

// FUNCION DE PIN
#define FUNC_GPIO 0
#define FUNC_ADC 1
#define FUNC_TX_UART0 1
#define FUNC_RX_UART0 1

// CONFIGURACION GPIO
#define INPUT 0
#define OUTPUT 1
#define BIT_VALUE(pos) (1 << pos)

// CONFIGURACION ADC
#define FREQ_MUESTREO 1000 // En Hz

// CONFIGURACION UART
#define BAUD_RATE 9600

// CONFIGURACION GPDMA
#define GPDMA_CHANNEL_ADC 0
#define GPDMA_CHANNEL_UART 1

// PROTOTIPOS
void configPCB(void);
void configGPIO(void);
void configADC(void);
void configUART(void);
void configGPDMA_ADC(void);
void configGPDMA_UART(void);
