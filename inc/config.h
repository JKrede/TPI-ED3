// CONFIGURACION DEL SISTEMA
#define BANK0_START 0x2007C000
#define BANK0_MUESTRAS 0x2007C800

#define CANT_MUESTRAS 2048 // CANT_MUESTRAS DEBE SER < 4096

#define PPM_UMBRAL_MAX 180
#define PPM_UMBRAL_MIN 30
// Valor umbral para identificar el R-Peak. Valor ajustable
// (4095*80%)
#define VAL_UMBRAL 3276

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
#define FUNC_EXTMAT 3

// CONFIGURACION GPIO
#define INPUT 0
#define OUTPUT 1
#define BIT_VALUE(pos) (1 << pos)

// CONFIGURACION TIMER
// Timer cada 1 minuto para contar latidos
#define TIMER_PS_1MS 1000
#define TIMER_60S 60000
#define TIMER_CHANNEL_0 0
// Timer para hacer funcionar el buzzer
#define TIMER_CHANNEL_2 2

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
void configTimerBuzzer(void);
void configTimerPPM(void);
void configADC(void);
void configUART(void);
void configGPDMA_ADC(void);
void configGPDMA_UART(void);
