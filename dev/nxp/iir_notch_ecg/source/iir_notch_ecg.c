/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    iir_notch_ecg.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_lpadc.h"
/* TODO: insert other include files here. */
#include "arm_math.h"
#include "circular_buffer.h"
/* TODO: insert other definitions and declarations here. */
#define NUM_STAGES 1
#define BLOCK_SIZE 1
arm_biquad_casd_df1_inst_f32 iir_instance;
static float32_t pState_iir[4*NUM_STAGES];
circularBuffer_t adcBuffer = {{0}, 0, 0};
circularBuffer_t filteredBuffer = {{0}, 0, 0};
float32_t filterInput[CIRCULAR_BUFFER_SIZE];
float32_t filterOutput[CIRCULAR_BUFFER_SIZE];

const float32_t G = 0.9875889421f;
const float32_t b0 = G * 1.0f;
const float32_t b1 = G * -1.902263284f;
const float32_t b2 = G * 1.0f;
const float32_t a1 = G * -1.878654122f;
const float32_t a2 = G * 0.9751778841f;

static const float32_t iir_coeffs[5] = {b0, b1, b2, -a1, -a2};
/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    arm_biquad_cascade_df1_init_f32(&iir_instance, NUM_STAGES, iir_coeffs, pState_iir);

    while(1) {}

    return 0 ;
}

void ADC1_IRQHANDLER(void) {
  uint32_t trigger_status_flag;
  uint32_t status_flag;
  /* Trigger interrupt flags */
  trigger_status_flag = LPADC_GetTriggerStatusFlags(ADC1_PERIPHERAL);
  /* Interrupt flags */
  status_flag = LPADC_GetStatusFlags(ADC1_PERIPHERAL);
  /* Clears trigger interrupt flags */
  LPADC_ClearTriggerStatusFlags(ADC1_PERIPHERAL, trigger_status_flag);
  /* Clears interrupt flags */
  LPADC_ClearStatusFlags(ADC1_PERIPHERAL, status_flag);


  static lpadc_conv_result_t res;

  if (LPADC_GetConvResult(ADC1, &res, 0U)) {
	static uint16_t adcIndex = 0;
  	uint16_t adcValue = (uint16_t)res.convValue;
  	float32_t adcFloat = (float32_t)(adcValue);

  	bufferWrite(&adcBuffer, &adcFloat);
  	bufferRead(&adcBuffer, &filterInput[adcIndex]);

  	arm_biquad_cascade_df1_f32(&iir_instance, &filterInput[0] + adcIndex, &filterOutput[0] + adcIndex, BLOCK_SIZE);

  	uint8_t dac_u8 = (uint8_t)((float32_t)((filterOutput[adcIndex]/65536.0f) * 255.0f));
    PUTCHAR(dac_u8);

  	adcIndex = (adcIndex + 1) % CIRCULAR_BUFFER_SIZE;
  }
  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}
