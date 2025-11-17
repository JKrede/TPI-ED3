#include "arm_math.h"
#include "config.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "util.h"
#include <stdint.h>

// Filtro IIR de 50Hz
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

// Buffer circular para almacenar valores filtrados
volatile uint8_t ppmBuffer[CANT_MUESTRAS];
volatile uint16_t ppmBufferIndex = 0;

// Variables para conteo de pulsos acumulativo
volatile uint16_t pulsos_acumulados = 0; // Contador acumulativo durante 60s
volatile uint8_t ultimo_valor = 0;       // Para detectar flancos
volatile uint8_t r_flag = 0;             // Flag de detección de pico

// Variable de PPM (actualizada cada 60s)
volatile uint8_t ppm = 0;
volatile uint8_t ppm_actualizado = 0;

int main() {
  SystemInit();

  arm_biquad_cascade_df1_init_f32(&iir_instance, NUM_STAGES, iir_coeffs,
                                  pState_iir);

  configPCB();
  configGPIO();
  configUART();
  configADC();
  configTimerBuzzer(0);
  configTimerPPM();
  configTimerADC();
  configGPDMA_UART(txBufferA);

  NVIC_EnableIRQ(ADC_IRQn);
  NVIC_EnableIRQ(TIMER2_IRQn);

  // Iniciar el Timer2 para contar 60 segundos
  TIM_Cmd(LPC_TIM2, ENABLE);

  while (1) {
    // Actualizar buzzer cuando se actualice el PPM
    if (ppm_actualizado) {

      // Leer ppm de forma segura
      uint8_t ppm_local = ppm;
      ppm_actualizado = 0;

      // Validar rango del PPM
      if (ppm_local >= PPM_UMBRAL_MIN && ppm_local <= PPM_UMBRAL_MAX) {
        // Calcular el match value basándose en el PPM actual
        uint32_t matchValue = TOGGLE_TIME(ppm_local);

        // Validar que matchValue no sea demasiado pequeño
        if (matchValue >= 10) {
          // Detener el timer antes de reconfigurar
          TIM_Cmd(LPC_TIM1, DISABLE);

          // Reconfigurar con el nuevo valor
          configTimerBuzzer(matchValue);

          // Reiniciar y arrancar el timer
          TIM_ResetCounter(LPC_TIM1);
          TIM_Cmd(LPC_TIM1, ENABLE);
        } else {
          // matchValue muy pequeño, detener buzzer
          TIM_Cmd(LPC_TIM1, DISABLE);
        }
      } else {
        // PPM fuera de rango, detener el buzzer
        TIM_Cmd(LPC_TIM1, DISABLE);
      }
    }
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

    // --- Almacenar en buffer circular ---
    ppmBuffer[ppmBufferIndex] = data_u8;
    ppmBufferIndex = (ppmBufferIndex + 1) % CANT_MUESTRAS;

    // --- Conteo de pulsos en tiempo real ---
    // Detectar flanco de subida
    if (data_u8 >= VAL_UMBRAL && !r_flag) {
      pulsos_acumulados++;
      r_flag = 1;
    }

    // Detectar flanco de bajada para resetear detección
    if (data_u8 < VAL_UMBRAL && r_flag) {
      r_flag = 0;
    }

    // --- PING-PONG UART: escribir solo si hay lugar ---
    if (fillIndex < TX_BUFFER_SIZE) {
      txFillBuffer[fillIndex] = data_u8;
      fillIndex++;
    }

    adcIndex++;
    if (adcIndex >= TX_BUFFER_SIZE) {
      adcIndex = 0;
    }

    // ¿Se llenó el buffer actual?
    if (fillIndex >= TX_BUFFER_SIZE) {

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
      dmaUartBusy = 0;
    }

    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CHANNEL_UART)) {
      GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CHANNEL_UART);
      dmaUartBusy = 0;
    }
  }
}

/**
 * @brief Handler cada 60 segundos para evaluar PPM y controlar LED
 */
void TIMER2_IRQHandler(void) {

  if (TIM_GetIntStatus(LPC_TIM2, TIM_MR0_INT)) {

    uint16_t pulsos = pulsos_acumulados;
    pulsos_acumulados = 0;

    // Limitar PPM a 255 (máximo de uint8_t)
    ppm = (pulsos > 255) ? 255 : (uint8_t)pulsos;

    // Verificar si el PPM está fuera del rango normal
    if (ppm > PPM_UMBRAL_MAX || ppm < PPM_UMBRAL_MIN) {
      GPIO_SetPins(PORT_LED, BIT_VALUE(PIN_LED));
    } else {
      GPIO_ClearPins(PORT_LED, BIT_VALUE(PIN_LED));
    }

    // Marcar para actualizar buzzer
    ppm_actualizado = 1;

    // Limpiar la interrupción
    TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
  }
}
