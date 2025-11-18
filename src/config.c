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

  cfgPinLedG.portNum = PORT_BUZZER;
  cfgPinLedG.pinNum = PIN_BUZZER;
  cfgPinLedG.funcNum = FUNC_GPIO;

  cfgPinLed.portNum = PORT_LED;
  cfgPinLed.pinNum = PIN_LED;
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

  GPIO_SetDir(PORT_LED, BIT_VALUE(PIN_LED), OUTPUT);
  GPIO_SetDir(PORT_BUZZER, BIT_VALUE(PIN_BUZZER), OUTPUT);

  GPIO_ClearPins(PORT_LED, BIT_VALUE(PIN_LED));
  GPIO_ClearPins(PORT_BUZZER, BIT_VALUE(PIN_BUZZER));
}

/**
 * @brief Configura el timer asociado al buzzer.
 *
 * @param matVal Valor de Match register(MR) a establecer
 *
 * @note: El valor a cargar en MR es (ppm/60)*1000*2
 */
void configTimerBuzzer(uint32_t matVal) {

  TIM_TIMERCFG_Type tim_extmat = {0};
  TIM_MATCHCFG_Type mat_extmat = {0};

  tim_extmat.prescaleOption = TIM_USVAL;
  tim_extmat.prescaleValue = TIMER_PS_1MS;

  mat_extmat.matchChannel = TIMER_CHANNEL_0;
  mat_extmat.intOnMatch = DISABLE;
  mat_extmat.resetOnMatch = ENABLE;
  mat_extmat.stopOnMatch = DISABLE;
  mat_extmat.extMatchOutputType = TIM_TOGGLE;
  mat_extmat.matchValue = matVal;

  TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &tim_extmat);
  TIM_ConfigMatch(LPC_TIM1, &mat_extmat);
}

/**
 * @brief Configura el timer destinado a interrumpir cada 1 minuto para
 * establecer la cantidad de ppm (pulsos por minuto)
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
 * @brief Configura el timer del ADC para realizar el muestreo
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
 * @brief Configura el modulo ADC
 */
void configADC(void) {

  ADC_Init(2000);
  ADC_BurstCmd(DISABLE);
  ADC_StartCmd(ADC_START_ON_MAT01);
  ADC_ChannelCmd(ADC_CHANNEL_0, ENABLE);
  ADC_EdgeStartConfig(ADC_START_ON_RISING);
  ADC_IntConfig(ADC_CHANNEL_0, ENABLE);
}

void configUART(void) {

  UART_CFG_Type cfgUART2;
  UART_FIFO_CFG_Type cfgFIFO;

  LPC_SC->PCONP |= (1 << 24);
  LPC_SC->PCLKSEL1 &= ~(3 << 16); // CCLK/4

  UART_ConfigStructInit(&cfgUART2);
  cfgUART2.Baud_rate = 115200; // BAUD_RATE;
  cfgUART2.Parity = UART_PARITY_NONE;
  cfgUART2.Databits = UART_DATABIT_8;
  cfgUART2.Stopbits = UART_STOPBIT_1;

  UART_Init(LPC_UART2, &cfgUART2);

  /*-------Override manual--------
   * DL = 12, DIVADDVAL=2, MULVAL=15
   * Baud = 114889 bps (error porcentual del 0,27%)*/
  uint8_t lcr = LPC_UART2->LCR;

  LPC_UART2->LCR = lcr | (1 << 7); // Divisor Latch Access Bit=1
  // Habilitado para escribir en DLL y DLM/
  LPC_UART2->DLL = 12;            // DL=12
  LPC_UART2->DLM = 0;             // DL=12
  LPC_UART2->FDR = (15 << 4) | 2; // Mulval y divaddval
  // Deshabilito para usar la UART normalmente/
  LPC_UART2->LCR = lcr & ~(1 << 7); // DLAB=0 ()

  UART_FIFOConfigStructInit(&cfgFIFO);
  cfgFIFO.FIFO_DMAMode = ENABLE;
  cfgFIFO.FIFO_Level = UART_FIFO_TRGLEV2;
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

  GPDMA_Init(); // habilita el controlador DMA

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

  // Configuramos el LLI para este bloque
  lliUART.srcAddr = (uint32_t)(uintptr_t)buffer;
  lliUART.dstAddr = (uint32_t)(uintptr_t)&LPC_UART2->THR;
  lliUART.nextLLI = 0; // una sola transferencia

  lliUART.control = (TX_BUFFER_SIZE << 0) | // número de transfers
                    (GPDMA_BSIZE_1 << 12) | // burst size fuente
                    (GPDMA_BSIZE_1 << 15) | // burst size destino
                    (GPDMA_BYTE << 18) |    // ancho fuente = 8 bits
                    (GPDMA_BYTE << 21) |    // ancho destino = 8 bits
                    (1 << 26) |             // incrementar dirección de fuente
                    (0 << 27) | // NO incrementar destino (THR = fijo)
                    (1 << 31);  // interrupción al final

  dmaUART.srcMemAddr = (uint32_t)(uintptr_t)buffer;
  dmaUART.transferSize = (uint32_t)TX_BUFFER_SIZE;
  dmaUART.linkedList = (uint32_t)(uintptr_t)&lliUART;

  GPDMA_Setup(&dmaUART);
  GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, ENABLE);

  *dmaUartBusy = 1;
}
