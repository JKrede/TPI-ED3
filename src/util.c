#include "util.h"
#include "config.h"

/**
 * @brief Cuenta la cantidad de pulsos por minuto y actualiza el valor de
 * ppm
 *
 * @param[in] reg_adc Puntero a la memoria que almacena las muestras
 * @param[out] ppm Puntero al registro que almacena el valor de ppm actual
 */
void contarPulsos(const uint32_t *muestras, uint8_t *ppm) {

  uint8_t r_flag = 0;

  for (int i = 0; i < CANT_MUESTRAS; i++) {
    uint16_t muestra = muestras[i];

    if (muestra >= VAL_UMBRAL && !r_flag) {
      (*ppm)++;
      r_flag = 1;
    }
    if (muestra < VAL_UMBRAL) {
      r_flag = 0;
    }
  }
}

/**
 * @brief Almacena en las muestras limpias en muestras_bank
 *
 * @param reg_bank Memoria donde se almacenan las salidas del ADC
 * @param muestras_bank Memoria donde se almacenan las muestras del ADC
 */
void extraerMuestras(const uint32_t *reg_bank, uint8_t *muestras_bank)
{
    for (int i = 0; i < CANT_MUESTRAS; i++)
    {
        // Cada palabra viene del registro AD0DRx:
        // bits 15:4 = RESULT (12 bits)
        uint16_t sample12 = (reg_bank[i] >> 4) & 0x0FFF;   // 0..4095

        // Lo paso a 8 bits tomando los 8 MSB (equivale a dividir por 16)
        // 0..4095  ->  0..255
        uint8_t sample8 = (uint8_t)(sample12 >> 4);

        muestras_bank[i] = sample8;
    }
}



