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

  PINSEL_CFG_Type cfgADC = {0};
  PINSEL_CFG_Type cfg_TX_UART = {0};
  PINSEL_CFG_Type cfg_RX_UART = {0};

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

  PINSEL_ConfigPin(&cfgADC);
  PINSEL_ConfigPin(&cfg_TX_UART);
  PINSEL_ConfigPin(&cfg_RX_UART);
}

/**
 * @brief Configura los GPIO y los limpia.
 */
void configGPIO(void) {

  GPIO_SetDir(PORT_LED, BIT_VALUE(PIN_LED), OUTPUT);
  GPIO_ClearPins(PORT_LED, BIT_VALUE(PIN_LED));
}

/**
 * @brief Configura el modulo ADC para funcionar con el modulo de GPDMA
 */
void configADC(void) {

  ADC_Init(2000);
  ADC_BurstCmd(DISABLE);
  ADC_StartCmd(ADC_START_ON_MAT01);
  ADC_ChannelCmd(ADC_CHANNEL_0, ENABLE);
  ADC_EdgeStartConfig(ADC_START_ON_RISING);
  ADC_IntConfig(ADC_CHANNEL_0, ENABLE); // Necesario para funcionar con GPDMA
}

/**
 * @brief
 *
 * @param
 *
 * @note
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
 * @brief Configura el modulo UART para funcionar con el modulo de GPDMA
 */
void configUART(void) {

  UART_CFG_Type cfgUART2;
  UART_FIFO_CFG_Type cfgFIFO;

  UART_ConfigStructInit(&cfgUART2);
  cfgUART2.Baud_rate = BAUD_RATE;
  cfgUART2.Parity = UART_PARITY_NONE;
  cfgUART2.Databits = UART_DATABIT_8;
  cfgUART2.Stopbits = UART_STOPBIT_1;

  UART_Init(LPC_UART2, &cfgUART2);

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
 * @note: Recordar habilitar el canal correspondiente
 */
void configGPDMA_UART(uint8_t *txBuffer) {

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

void startUART_DMA(uint8_t *buffer, uint8_t *dmaUartBusy) {

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
