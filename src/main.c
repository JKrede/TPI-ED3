#include "LPC17xx.h"
#include "config.h"
#include "lpc17xx_gpdma.h"
#include "lpc_types.h"

int main() {

  // Inicio de programa carga de configuraciones iniciales
  configPCB();
  configGPIO();
  configADC();
  configUART();
  configGPDMA_ADC();
  GPDMA_ChannelCmd(GPDMA_CHANNEL_ADC, ENABLE);

  while (1) {
    __WFI();
  }

  return 0;
}

void DMA_IRQHandler(void) {

  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_ADC)) {
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_ADC)) {

      GPDMA_ChannelCmd(GPDMA_CHANNEL_ADC, DISABLE);
      configGPDMA_UART();
      GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, ENABLE);
    }
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_ADC);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_ADC);
  }

  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_UART)) {
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_UART)) {

      GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, DISABLE);
      configGPDMA_ADC();
      GPDMA_ChannelCmd(GPDMA_CHANNEL_ADC, ENABLE);
    }
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_UART);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_UART);
  }
}
