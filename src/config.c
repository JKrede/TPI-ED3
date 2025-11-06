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
#include "lpc17xx_uart.h"
#include "lpc_types.h"
#include <stdint.h>

/**
 * @brief Configura pines
 */
void configPCB(void) {

  PINSEL_CFG_Type cfgPinBuzzer = {0};
  PINSEL_CFG_Type cfgPinLed = {0};
  PINSEL_CFG_Type cfgADC = {0};
  PINSEL_CFG_Type cfg_TX_UART = {0};
  PINSEL_CFG_Type cfg_RX_UART = {0};

  cfgPinBuzzer.portNum = PORT_BUZZER;
  cfgPinBuzzer.pinNum = PIN_BUZZER;
  cfgPinBuzzer.funcNum = FUNC_GPIO;

  cfgPinLed.portNum = PORT_LED;
  cfgPinLed.pinNum = PIN_LED;
  cfgPinLed.funcNum = FUNC_GPIO;

  cfgADC.portNum = PORT_ADC;
  cfgADC.pinNum = PIN_ADC;
  cfgADC.funcNum = FUNC_ADC;
  cfgADC.pinMode = PINSEL_TRISTATE;

  cfg_TX_UART.portNum = PORT_TX_UART0;
  cfg_TX_UART.pinNum = PIN_TX_UART0;
  cfg_TX_UART.funcNum = FUNC_TX_UART0;

  cfg_RX_UART.portNum = PORT_RX_UART0;
  cfg_RX_UART.pinNum = PIN_RX_UART0;
  cfg_RX_UART.funcNum = FUNC_RX_UART0;
  cfg_RX_UART.pinMode = PINSEL_TRISTATE;

  PINSEL_ConfigPin(&cfgPinBuzzer);
  PINSEL_ConfigPin(&cfgPinLed);
  PINSEL_ConfigPin(&cfgADC);
  PINSEL_ConfigPin(&cfg_TX_UART);
  PINSEL_ConfigPin(&cfg_RX_UART);
}

/**
 * @brief Configura los GPIO y los limpia.
 */
void configGPIO(void) {

  GPIO_SetDir(PORT_BUZZER, BIT_VALUE(PIN_BUZZER), OUTPUT);
  GPIO_ClearPins(PORT_BUZZER, BIT_VALUE(PIN_BUZZER));

  GPIO_SetDir(PORT_LED, BIT_VALUE(PIN_LED), OUTPUT);
  GPIO_ClearPins(PORT_LED, BIT_VALUE(PIN_LED));
}

/**
 * @brief Configura el modulo ADC para funcionar con el modulo de GPDMA
 */
void configADC(void) {

  ADC_Init(FREQ_MUESTREO);
  ADC_BurstCmd(ENABLE);
  ADC_StartCmd(ADC_START_CONTINUOUS);
  ADC_IntConfig(ADC_CHANNEL_0, ENABLE); // Necesario para funcionar con GPDMA
}

/**
 * @brief Configura el modulo UART para funcionar con el modulo de GPDMA
 */
void configUART(void) {

  UART_CFG_Type cfgUART0 = {0};
  UART_FIFO_CFG_Type cfgFIFO = {0};

  cfgUART0.Parity = UART_PARITY_EVEN;
  cfgUART0.Databits = UART_DATABIT_8;
  cfgUART0.Baud_rate = BAUD_RATE;
  cfgUART0.Stopbits = UART_STOPBIT_1;

  cfgFIFO.FIFO_DMAMode = ENABLE;
  cfgFIFO.FIFO_Level = UART_FIFO_TRGLEV2;

  UART_FIFOConfig(LPC_UART0, &cfgFIFO);
  UART_Init(LPC_UART0, &cfgUART0);
}

/**
 * @brief Configura el modulo de GPDMA para funcionar con los modulos ADC y UART
 */
void configGPDMA(void) {

  GPDMA_Channel_CFG_Type dmaADC = {0};
  GPDMA_Channel_CFG_Type dmaUART = {0};

  GPDMA_LLI_Type lliADC = {0};
  GPDMA_LLI_Type lliUART = {0};

  // Linked lists configuradas de igual manera que sus correspondientes
  // configuraciones de dma con el fin de reutilizar el espacio de memoria
  // destinado al almacemamiento de las muestras

  lliADC.srcAddr = (uint32_t)(uintptr_t)&LPC_ADC->ADGDR;
  lliADC.dstAddr = (uint32_t)BANK0_START;
  lliADC.nextLLI = (uint32_t)(uintptr_t)&lliADC;
  lliADC.control = (CANT_MUESTRAS << 0) | (GPDMA_BSIZE_4 << 12) |
                   (GPDMA_BSIZE_4 << 15) | (GPDMA_WORD << 18) |
                   (GPDMA_WORD << 21) | (1 << 27) | (1 << 31);

  lliUART.srcAddr = (uint32_t)BANK0_START;
  lliUART.dstAddr = ((uint32_t)(uintptr_t)&LPC_UART0->THR);
  lliUART.nextLLI = (uint32_t)(uintptr_t)&lliUART;
  lliUART.control = (CANT_MUESTRAS << 0) | (GPDMA_BSIZE_1 << 12) |
                    (GPDMA_BSIZE_1 << 15) | (GPDMA_BYTE << 18) |
                    (GPDMA_BYTE << 21) | (1 << 26) | (1 << 31);

  dmaADC.channelNum = GPDMA_CHANNEL_ADC;
  dmaADC.srcConn = GPDMA_ADC;
  dmaADC.dstMemAddr = (uint32_t)BANK0_START;
  dmaADC.transferType = GPDMA_P2M;
  dmaADC.transferSize = (uint32_t)CANT_MUESTRAS;
  dmaADC.linkedList = (uint32_t)(uintptr_t)&lliADC;

  dmaUART.channelNum = GPDMA_CHANNEL_UART;
  dmaUART.srcMemAddr = (uint32_t)BANK0_START;
  dmaUART.dstConn = GPDMA_UART0_Tx;
  dmaUART.transferType = GPDMA_M2P;
  dmaUART.transferSize = (uint32_t)CANT_MUESTRAS;
  dmaUART.linkedList = (uint32_t)(uintptr_t)&lliUART;

  GPDMA_Init();
  GPDMA_Setup(&dmaADC);
  GPDMA_Setup(&dmaUART);
}
