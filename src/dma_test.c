#include "config.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "util.h"
#include <stdint.h>
#include "arm_math.h"


//Filter IIR
#define NUM_STAGES 1
#define BLOCK_SIZE 1
arm_biquad_casd_df1_inst_f32 iir_instance;
static float32_t pState_iir[4*NUM_STAGES];
const float32_t G = 0.9875889421f;
const float32_t b0 = G * 1.0f;
const float32_t b1 = G * -1.902263284f;
const float32_t b2 = G * 1.0f;
const float32_t a1 = G * -1.878654122f;
const float32_t a2 = G * 0.9751778841f;

float32_t iir_coeffs[5] = {b0, b1, b2, -a1, -a2};

volatile float32_t fifoInputIIR[TX_BUFFER_SIZE];
volatile float32_t fifoOutputIIR[TX_BUFFER_SIZE];

//UART2
volatile uint8_t txBuffer0[TX_BUFFER_SIZE];
volatile uint8_t txBuffer1[TX_BUFFER_SIZE];
volatile uint16_t txIndex0 = 0;
volatile uint16_t txIndex1 = 0;
volatile uint8_t dmaUartBusy = 0;
static GPDMA_LLI_Type lliUART = {0};
static GPDMA_Channel_CFG_Type dmaUART = {0};

//ADC
volatile uint16_t adcIndex = 0;





int main() {

	SystemInit();

    arm_biquad_cascade_df1_init_f32(&iir_instance, NUM_STAGES, iir_coeffs, pState_iir);

	configPCB();
	configUART();
	configADC();
	configTimerADC();
	configGPDMA_UART();

	NVIC_EnableIRQ(ADC_IRQn);



	while (1) {}

	return 0;
}

void ADC_IRQHandler(void) {

  if (ADC_ChannelGetStatus(ADC_CHANNEL_0, ADC_DATA_DONE)) {

    uint16_t adcData = ADC_ChannelGetData(ADC_CHANNEL_0);


    uint8_t data_u8 = (uint8_t)((uint16_t)adcData >> 4);


    if (txIndex0 <= TX_BUFFER_SIZE) {
      txBuffer0[txIndex0++] = data_u8;
    } else {
      if (!dmaUartBusy) {
        startUART_DMA((uint8_t *)txBuffer0);
        txIndex0 = 0;
      }
    }
    if (txIndex1 <= TX_BUFFER_SIZE) {
      txBuffer1[txIndex1++] = data_u8;
    } else {
      if (!dmaUartBusy) {
        startUART_DMA((uint8_t *)txBuffer1);
        txIndex1 = 0;
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
  dmaUART.srcMemAddr = (uint32_t)txBuffer0;
  dmaUART.dstConn = GPDMA_UART2_Tx;
  dmaUART.transferType = GPDMA_M2P;
  dmaUART.transferSize = (uint32_t)(TX_BUFFER_SIZE);
  dmaUART.linkedList = (uint32_t)(uintptr_t)&lliUART;

  //GPDMA_Setup(&dmaUART);

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
    NVIC_EnableIRQ(DMA_IRQn);

    dmaUartBusy = 1;
}




