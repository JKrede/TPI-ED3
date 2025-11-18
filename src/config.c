/**
 * @file
 * @brief Funciones de configuracion basadas en macros establecidos en
 * config.h
 */

#include "config.h"
#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc_types.h"
#include <stdint.h>

//Estructuras del DMA globales
static GPDMA_LLI_Type lliUART = {0};
static GPDMA_Channel_CFG_Type dmaUART = {0};

/**
 * @brief Configura pines
 */
void configPCB(void) {
  PINSEL_CFG_Type cfgPinLedG = {0}; // Seria el buzzer
  PINSEL_CFG_Type cfgPinLed = {0};
  PINSEL_CFG_Type cfgADC = {0};
  PINSEL_CFG_Type cfg_TX_UART = {0};
  PINSEL_CFG_Type cfg_RX_UART = {0};

  cfgPinLedG.portNum = PORT_LED_GREEN;
  cfgPinLedG.pinNum = PIN_LED_GREEN;
  cfgPinLedG.funcNum = FUNC_GPIO;

  cfgPinLed.portNum = PORT_LED_RED;
  cfgPinLed.pinNum = PIN_LED_RED;
  cfgPinLed.funcNum = FUNC_GPIO;

  cfgADC.portNum = PORT_ADC;
  cfgADC.pinNum = PIN_ADC;
  cfgADC.funcNum = FUNC_ADC;
  cfgADC.pinMode = PINSEL_TRISTATE;

  cfg_TX_UART.portNum = PORT_TX_UART2;
  cfg_TX_UART.pinNum = PIN_TX_UART2;
  cfg_TX_UART.funcNum = FUNC_TX_UART2;

  cfg_RX_UART.portNum = PORT_RX_UART2;
  cfg_RX_UART.pinNum = PIN_RX_UART2;
  cfg_RX_UART.funcNum = FUNC_RX_UART2;
  cfg_RX_UART.pinMode = PINSEL_TRISTATE;

  PINSEL_ConfigPin(&cfgPinLedG);
  PINSEL_ConfigPin(&cfgPinLed);
  PINSEL_ConfigPin(&cfgADC);
  PINSEL_ConfigPin(&cfg_TX_UART);
  PINSEL_ConfigPin(&cfg_RX_UART);
}

/**
 * @brief Configura los GPIO y los limpia.
 */
void configGPIO(void) {

  GPIO_SetDir(PORT_LED_RED, BIT_VALUE(PIN_LED_RED), OUTPUT);
  GPIO_SetDir(PORT_LED_GREEN, BIT_VALUE(PIN_LED_GREEN), OUTPUT);

  GPIO_SetPins(PORT_LED_RED, BIT_VALUE(PIN_LED_RED));
  GPIO_SetPins(PORT_LED_GREEN, BIT_VALUE(PIN_LED_GREEN));
}

/**
 * @brief Configura el timer destinado a interrumpir cada 1 minuto para
 * calcular la cantidad de ppm (pulsos por minuto)
 */
void configTimerPPM(void) {

  TIM_TIMERCFG_Type tim_min = {0};
  TIM_MATCHCFG_Type match_min = {0};

  tim_min.prescaleOption = TIM_USVAL;
  tim_min.prescaleValue = TIMER_PS_1MS;

  match_min.matchChannel = TIMER_CHANNEL_0;
  match_min.matchValue = TIMER_60S; // 60000 ms = 60 s
  match_min.intOnMatch = ENABLE;
  match_min.resetOnMatch = ENABLE;
  match_min.stopOnMatch = DISABLE;

  TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &tim_min);
  TIM_ConfigMatch(LPC_TIM2, &match_min);
}

/**
 * @brief Configura el timer del ADC para realizar el muestreo a 1 kHz
 *
 */
void configTimerADC(void) {

  TIM_TIMERCFG_Type tim_adc = {0};
  TIM_MATCHCFG_Type mat_adc = {0};

  tim_adc.prescaleOption = TIM_TICKVAL;
  tim_adc.prescaleValue = 99;

  mat_adc.matchChannel = 1;
  mat_adc.intOnMatch = DISABLE;
  mat_adc.resetOnMatch = ENABLE;
  mat_adc.stopOnMatch = DISABLE;
  mat_adc.extMatchOutputType = TIM_TOGGLE;
  mat_adc.matchValue = 124;

  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tim_adc);
  TIM_ConfigMatch(LPC_TIM0, &mat_adc);

  TIM_Cmd(LPC_TIM0, ENABLE);
}
/**
 * @brief Configura el modulo ADC por trigger de MAT0 TMR1
 */
void configADC(void) {

  ADC_Init(2000);
  ADC_BurstCmd(DISABLE);
  ADC_StartCmd(ADC_START_ON_MAT01);
  ADC_ChannelCmd(ADC_CHANNEL_0, ENABLE);
  ADC_EdgeStartConfig(ADC_START_ON_RISING);
  ADC_IntConfig(ADC_CHANNEL_0, ENABLE);
}

/**
 * @brief Configura el modulo UART para funcionar con el modulo de GPDMA
 */
void configUART(void) {

    UART_CFG_Type      cfgUART2;
    UART_FIFO_CFG_Type cfgFIFO;

    UART_ConfigStructInit(&cfgUART2);
    cfgUART2.Baud_rate = BAUD_RATE;
    cfgUART2.Parity    = UART_PARITY_NONE;
    cfgUART2.Databits  = UART_DATABIT_8;
    cfgUART2.Stopbits  = UART_STOPBIT_1;

    UART_Init(LPC_UART2, &cfgUART2);

    UART_FIFOConfigStructInit(&cfgFIFO);
    cfgFIFO.FIFO_DMAMode    = ENABLE;
    cfgFIFO.FIFO_Level      = UART_FIFO_TRGLEV2;
    cfgFIFO.FIFO_ResetRxBuf = ENABLE;
    cfgFIFO.FIFO_ResetTxBuf = ENABLE;
    UART_FIFOConfig(LPC_UART2, &cfgFIFO);

    UART_TxCmd(LPC_UART2, ENABLE);
}
/**
 * @brief Configura el modulo de GPDMA par funcionar con el modulo UART
 *
 */
void configGPDMA_UART(volatile uint8_t *txBuffer) {

  GPDMA_Init();

  lliUART.nextLLI = 0;

  dmaUART.channelNum = GPDMA_CHANNEL_UART;
  dmaUART.srcMemAddr = (uint32_t)(uintptr_t)txBuffer;
  dmaUART.dstConn = GPDMA_UART2_Tx;
  dmaUART.transferType = GPDMA_M2P;
  dmaUART.transferSize = (uint32_t)TX_BUFFER_SIZE;
  dmaUART.linkedList = (uint32_t)(uintptr_t)&lliUART;

  NVIC_EnableIRQ(DMA_IRQn);
}

void startUART_DMA(uint8_t *buffer, volatile uint8_t *dmaUartBusy) {


  lliUART.srcAddr = (uint32_t)(uintptr_t)buffer;
  lliUART.dstAddr = (uint32_t)(uintptr_t)&LPC_UART2->THR;
  lliUART.nextLLI = 0;

  lliUART.control = (TX_BUFFER_SIZE << 0) |
                    (GPDMA_BSIZE_1 << 12) |
                    (GPDMA_BSIZE_1 << 15) |
                    (GPDMA_BYTE << 18) |
                    (GPDMA_BYTE << 21) |
                    (1 << 26) |
                    (0 << 27) |
                    (1 << 31);

  dmaUART.srcMemAddr = (uint32_t)(uintptr_t)buffer;
  dmaUART.transferSize = (uint32_t)TX_BUFFER_SIZE;
  dmaUART.linkedList = (uint32_t)(uintptr_t)&lliUART;

  GPDMA_Setup(&dmaUART);
  GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, ENABLE);

  *dmaUartBusy = 1;
}
