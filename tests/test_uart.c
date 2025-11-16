#include "config.h"
#include "lpc17xx_gpdma.h"
#include <stdint.h>

uint16_t *ptr_muestras = (uint16_t *)BANK0_MUESTRAS;

int main() {

  configGPDMA_UART();

  for (int i = 0; i < CANT_MUESTRAS; i++) {
    ptr_muestras[i] = (i % 4095);
  }

  GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, DISABLE);
  while (1) {
    __WFI();
  }
}

void DMA_IRQHandler(void) {

  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_UART)) {
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_UART)) {
      // No hace nada en el test
    }
    // Limpia banderas de finalizacion y error
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_UART);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_UART);
  }
}
