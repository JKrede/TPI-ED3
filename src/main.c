#include "LPC17xx.h"
#include "config.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc_types.h"
#include "util.h"
#include <stdint.h>
#include <sys/types.h>

// Variables globales
uint8_t ppm = 0;
volatile uint8_t muestras_listas = 0;

int main() {

  // Inicio de programa carga de configuraciones iniciales
  configPCB();
  configGPIO();
  configADC();
  configUART();
  configTimerPPM();
  configTimerBuzzer();
  configGPDMA_ADC();
  // Inicia el conteo de los 60 segundos
  TIM_Cmd(LPC_TIM0, ENABLE);
  // Incia la conversion del ADC y almacenamiento en memoria mediante DMA
  GPDMA_ChannelCmd(GPDMA_CHANNEL_ADC, ENABLE);

  while (1) {

    if (muestras_listas) {
      contarPulsos((uint32_t *)BANK0_START, &ppm);
      muestras_listas = 0;
      uint32_t mr_load = (ppm * 1000) / (60 * 2); // Resolver magic numbers
      TIM_UpdateMatchValue(LPC_TIM2, TIMER_CHANNEL_2, mr_load);
      configGPDMA_UART();
      GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, ENABLE);
    }
  }

  return 0;
}

void DMA_IRQHandler(void) {

  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_ADC)) {
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_ADC)) {
      // Cuando el ADC finaliza levanta la bandera para procesar los datos
      // obtenidos
      GPDMA_ChannelCmd(GPDMA_CHANNEL_ADC, DISABLE);
      muestras_listas = 1;
    }
    // Limpia banderas de finalizacion y error
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_ADC);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_ADC);
  }

  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_UART)) {
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_UART)) {
      // Cuando Finaliza la transimision UART
      GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, DISABLE);
      configGPDMA_ADC();
      GPDMA_ChannelCmd(GPDMA_CHANNEL_ADC, ENABLE);
    }
    // Limpia banderas de finalizacion y error
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_UART);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_UART);
  }
}

void TIMER0_IRQHandler(void) {
  // Aca va la logica que enciende el LED en base a los ppm
  if (ppm > PPM_UMBRAL_MAX || ppm < PPM_UMBRAL_MIN) {
    GPIO_SetPins(PORT_LED, BIT_VALUE(PIN_LED));
  } else {
    GPIO_ClearPins(PORT_LED, BIT_VALUE(PIN_LED));
  }
  ppm = 0;
}
