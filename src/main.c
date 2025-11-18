#include "arm_math.h"
#include "config.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include <stdint.h>

// Filtro IIR Notch 50Hz Direct Form I Bicascada
arm_biquad_casd_df1_inst_f32 iir_instance;
static float32_t pState_iir[4 * NUM_STAGES];
const float32_t b0 = B0;
const float32_t b1 = B1;
const float32_t b2 = B2;
const float32_t a1 = A1;
const float32_t a2 = A2;

// Arreglo de coeficientes
float32_t iir_coeffs[5] = {b0, b1, b2, -a1, -a2};

// Buffers auxiliares
volatile float32_t fifoInputIIR[TX_BUFFER_SIZE];
volatile float32_t fifoOutputIIR[TX_BUFFER_SIZE];

// Buffer para DMA UART
volatile uint8_t txBufferA[TX_BUFFER_SIZE];
volatile uint8_t txBufferB[TX_BUFFER_SIZE];
// puntero al buffer que se está llenando
volatile uint8_t *txFillBuffer = txBufferA;
// índice dentro del buffer que se está llenando
volatile uint16_t fillIndex = 0;
// flag: DMA ocupada transmitiendo algún buffer
volatile uint8_t dmaUartBusy = 0;

// ADC
volatile uint16_t adcIndex = 0;

// Variables para conteo de pulsos acumulativo
volatile uint16_t pulsos_acumulados = 0; // Contador acumulativo durante 60s
volatile uint8_t ultimo_valor = 0;       // Para detectar flancos
volatile uint8_t r_flag = 0;             // Flag de detección de pico

// Variable de ppm
volatile uint8_t ppm = 0;

int main() {
  SystemInit();

  arm_biquad_cascade_df1_init_f32(&iir_instance, NUM_STAGES, iir_coeffs,
                                  pState_iir); // Inicializa filtro

  // Configura los módulos y periféricos
  configPCB();
  configGPIO();
  configUART();
  configADC();
  configTimerPPM();
  configTimerADC();
  configGPDMA_UART(txBufferA);

  // Habilita interrupciones
  NVIC_EnableIRQ(ADC_IRQn);
  NVIC_EnableIRQ(TIMER2_IRQn);

  TIM_Cmd(LPC_TIM2, ENABLE);

  while (1) {
  }

  return 0;
}

void ADC_IRQHandler(void) {

  if (ADC_ChannelGetStatus(ADC_CHANNEL_0, ADC_DATA_DONE)) {

    uint16_t adcRawData = ADC_ChannelGetData(ADC_CHANNEL_0);

    // Procesamiento de la señal mediante filtro IIR
    fifoInputIIR[adcIndex] = (float32_t)adcRawData;

    arm_biquad_cascade_df1_f32(&iir_instance, &fifoInputIIR[0] + adcIndex,
                               &fifoOutputIIR[0] + adcIndex, 1);

    // Escalado a 0–255
    float32_t scaled = fifoOutputIIR[adcIndex] * 255.0f / 4095.0f;
    if (scaled < 0.0f)
      scaled = 0.0f;
    else if (scaled > 255.0f)
      scaled = 255.0f;

    // Redondeo
    uint8_t data_u8 = (uint8_t)(scaled + 0.5f);

    // --- Conteo de pulsos en tiempo real ---
    // Detectar flanco de subida
    if (data_u8 >= VAL_UMBRAL && !r_flag) {
      GPIO_ClearPins(PORT_LED_GREEN, BIT_VALUE(PIN_LED_GREEN));
      pulsos_acumulados++;
      r_flag = 1;
    }

    // Detectar flanco de bajada para resetear detección
    if (data_u8 < VAL_UMBRAL && r_flag) {
      GPIO_SetPins(PORT_LED_GREEN, BIT_VALUE(PIN_LED_GREEN));
      r_flag = 0;
    }

    adcIndex++;
    if (adcIndex >= TX_BUFFER_SIZE) {
      adcIndex = 0;
    }

    // --- Ping Pong UART ---
    if (fillIndex < TX_BUFFER_SIZE) {
      txFillBuffer[fillIndex] = data_u8;
      fillIndex++;
    } else {
      if (!dmaUartBusy) {
        // Buffer lleno y DMA libre
        uint8_t *txDMABuffer = (uint8_t *)txFillBuffer;
        startUART_DMA(txDMABuffer, &dmaUartBusy);

        // Cambiar buffer de llenado
        txFillBuffer = (txFillBuffer == txBufferA) ? txBufferB : txBufferA;
        fillIndex = 0;
      } else {
        // Buffer lleno y DMA ocupada
        fillIndex = TX_BUFFER_SIZE;
      }
    }
  }
}

void DMA_IRQHandler(void) {

  if (GPDMA_IntGetStatus(GPDMA_INT, GPDMA_CHANNEL_UART)) {

    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CHANNEL_UART)) {
      GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CHANNEL_UART);
      dmaUartBusy = 0; // Reinicia transferencia
    }

    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CHANNEL_UART)) {
      GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_UART);
      dmaUartBusy = 0; // Reinicia transferencia
    }
  }
}

/**
 * @brief Handler cada 60 segundos para calcular ppm y controlar LED RED
 */
void TIMER2_IRQHandler(void) {

  if (TIM_GetIntStatus(LPC_TIM2, TIM_MR0_INT)) {

    uint16_t pulsos = pulsos_acumulados;
    pulsos_acumulados = 0;

    // Limita ppm a 255
    ppm = (pulsos > 255) ? 255 : (uint8_t)pulsos;

    // Verifica si el PPM está fuera del rango normal
    if (ppm > PPM_UMBRAL_MAX || ppm < PPM_UMBRAL_MIN) {
      GPIO_ClearPins(PORT_LED_RED, BIT_VALUE(PIN_LED_RED));
    } else {
      GPIO_SetPins(PORT_LED_RED, BIT_VALUE(PIN_LED_RED));
    }

    TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
  }
}
