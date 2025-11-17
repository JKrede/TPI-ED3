#include "config.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "util.h"
#include <stdint.h>

volatile uint8_t txBuffer[TX_BUFFER_SIZE];
volatile uint16_t txIndex = 0;
volatile uint8_t dmaUartBusy = 0;

static GPDMA_LLI_Type lliUART = {0};
static GPDMA_Channel_CFG_Type dmaUART = {0};


int main() {

	SystemInit();
	configPCB();
	configUART();
	configADC();
	configTimerADC();
	configGPDMA_UART();

	while (1) {}

	return 0;
}

void ADC_IRQHandler(void) {

    if (ADC_ChannelGetStatus(ADC_CHANNEL_0, ADC_DATA_DONE)) {

        uint16_t adcData = ADC_ChannelGetData(ADC_CHANNEL_0);
        uint8_t  data_u8 = (uint8_t)((uint16_t)adcData >> 4);

        // Guardamos la muestra en el buffer de TX
        txBuffer[txIndex++] = data_u8;

        // Si se llenó el buffer y el DMA está libre, lo mandamos
        if (txIndex >= TX_BUFFER_SIZE) {
            if (!dmaUartBusy) {
                startUART_DMA((uint8_t *)txBuffer);
                txIndex = 0;    // volvemos a llenar desde el principio
            } else {
                // Caso "buffer lleno y DMA ocupado": según tu aplicación
                // podrías descartar, usar doble buffer, etc.
            }
        }
    }
}



void DMA_IRQHandler(void) {

    // ¿Terminó la transferencia del canal de UART?
    if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_UART)) {
    	if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_UART)) {
            GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_UART);

            dmaUartBusy = 0;
    	}
    }
}


/**
 * @brief Configura el modulo de GPDMA par funcionar con el modulo UART
 *
 * @note: Recordar habilitar el canal correspondiente
 */
void configGPDMA_UART(void) {
  GPDMA_Init();

  lliUART.nextLLI = 0;

  dmaUART.channelNum = GPDMA_CHANNEL_UART;
  dmaUART.srcMemAddr = (uint32_t)txBuffer;;
  dmaUART.dstConn = GPDMA_UART2_Tx;
  dmaUART.transferType = GPDMA_M2P;
  dmaUART.transferSize = (uint32_t)(TX_BUFFER_SIZE);
  dmaUART.linkedList = (uint32_t)(uintptr_t)&lliUART;

  //GPDMA_Setup(&dmaUART);
  NVIC_EnableIRQ(DMA_IRQn);
}


void startUART_DMA(uint8_t *buf) {

    // Configuramos el LLI para este bloque
    lliUART.srcAddr = (uint32_t)(uintptr_t)buf;
    lliUART.dstAddr = (uint32_t)(uintptr_t)&LPC_UART2->THR;
    lliUART.nextLLI = 0;   // una sola transferencia

    lliUART.control =
        (TX_BUFFER_SIZE    << 0 ) |   // número de transfers
        (GPDMA_BSIZE_1     << 12) |   // burst size fuente
        (GPDMA_BSIZE_1     << 15) |   // burst size destino
        (GPDMA_BYTE        << 18) |   // ancho fuente = 8 bits
        (GPDMA_BYTE        << 21) |   // ancho destino = 8 bits
        (1                 << 26) |   // incrementar dirección de fuente
        (0                 << 27) |   // NO incrementar destino (THR siempre igual)
        (1                 << 31);    // interrupción al final de la transferencia

    dmaUART.srcMemAddr   = (uint32_t)(uintptr_t)buf;
    dmaUART.transferSize = (uint32_t)TX_BUFFER_SIZE;
    dmaUART.linkedList   = (uint32_t)(uintptr_t)&lliUART;

    GPDMA_Setup(&dmaUART);
    GPDMA_ChannelCmd(GPDMA_CHANNEL_UART, ENABLE);  // ¡ahora sí, arranca el DMA!

    dmaUartBusy = 1;
}




