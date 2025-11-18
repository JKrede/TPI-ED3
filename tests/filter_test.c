#include "arm_math.h"
#include "config.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_uart.h"
#include "util.h"
#include <stdint.h>

arm_biquad_casd_df1_inst_f32 iir_instance;
static float32_t pState_iir[4 * NUM_STAGES];
const float32_t b0 = B0;
const float32_t b1 = B1;
const float32_t b2 = B2;
const float32_t a1 = A1;
const float32_t a2 = A2;

float32_t iir_coeffs[5] = {b0, b1, b2, -a1, -a2};

volatile float32_t fifoInputIIR[TX_BUFFER_SIZE];
volatile float32_t fifoOutputIIR[TX_BUFFER_SIZE];

// ---------- UART + DMA: Ping-Pong ----------
volatile uint8_t txBufferA[TX_BUFFER_SIZE];
volatile uint8_t txBufferB[TX_BUFFER_SIZE];

// puntero al buffer que se está llenando
volatile uint8_t *txFillBuffer = txBufferA;

// índice dentro del buffer que se está llenando
volatile uint16_t fillIndex = 0;

// flag: DMA ocupada transmitiendo algún buffer
volatile uint8_t dmaUartBusy = 0;

// opcional: para debug de pérdidas
volatile uint32_t uartOverflowCount = 0;

// ---------- ADC ----------
volatile uint16_t adcIndex = 0;

int main() {

  SystemInit();

  arm_biquad_cascade_df1_init_f32(&iir_instance, NUM_STAGES, iir_coeffs,
                                  pState_iir);

  configPCB();
  configUART();
  configADC();
  configTimerADC();
  configGPDMA_UART(txBufferA);

  NVIC_EnableIRQ(ADC_IRQn);

  while (1) {
  }

  return 0;
}

void ADC_IRQHandler(void) {

  if (ADC_ChannelGetStatus(ADC_CHANNEL_0, ADC_DATA_DONE)) {

    uint16_t adcRawData = ADC_ChannelGetData(ADC_CHANNEL_0);
    fifoInputIIR[adcIndex] = (float32_t)adcRawData;

    // Guardamos la muestra cruda (si te sirve para debug)

    // Si quisieras aplicar el filtro IIR muestra a muestra:
    arm_biquad_cascade_df1_f32(&iir_instance, &fifoInputIIR[0] + adcIndex,
                               &fifoOutputIIR[0] + adcIndex, 1);

    // y luego escalar y = fifoOutputIIR[...] en vez de x.

    // Escalamos a 0–255 (simple, sin saturación extra)
    float32_t scaled = fifoOutputIIR[adcIndex] * 255.0f / 4095.0f;
    if (scaled < 0.0f)
      scaled = 0.0f;
    else if (scaled > 255.0f)
      scaled = 255.0f;

    uint8_t data_u8 = (uint8_t)(scaled + 0.5f); // redondeo

    // --- PING-PONG: escribir solo si hay lugar en el buffer actual ---
    if (fillIndex < TX_BUFFER_SIZE) {
      txFillBuffer[fillIndex] = data_u8;
      fillIndex++;
    }

    adcIndex++;
    if (adcIndex >= TX_BUFFER_SIZE) {
      adcIndex = 0; // solo índice de log IIR
    }

    // ¿Se llenó el buffer actual?
    if (fillIndex >= TX_BUFFER_SIZE) {

      if (!dmaUartBusy) {
        // Buffer lleno y DMA libre -> arrancamos DMA con el buffer lleno

        uint8_t *txDMABuffer = (uint8_t *)txFillBuffer;

        // Lanzar DMA con este buffer
        startUART_DMA(txDMABuffer);

        // Cambiar buffer de llenado al otro
        if (txFillBuffer == txBufferA) {
          txFillBuffer = txBufferB;
        } else {
          txFillBuffer = txBufferA;
        }

        // Reiniciar índice de llenado
        fillIndex = 0;
      } else {
        // Buffer lleno y DMA ocupada -> no hay buffer libre
        // Evitamos escribir fuera de rango
        fillIndex = TX_BUFFER_SIZE;
        uartOverflowCount++; // para debug: contamos pérdidas
      }
    }
  }
}

void DMA_IRQHandler(void) {

  // ¿Interrupción del canal de UART?
  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_UART)) {

    // ¿Terminal Count?
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_UART)) {
      GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_UART);
      dmaUartBusy = 0;
    }

    // ¿Error?
    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CHANNEL_UART)) {
      GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_UART);
      dmaUartBusy = 0; // y podrías loguear el error
    }
  }
}
