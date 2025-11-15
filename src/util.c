#include "util.h"
#include "config.h"

/**
 * @brief Cuenta la cantidad de pulsos por minuto y actualiza el valor de
 * ppm
 *
 * @param[in] reg_adc: Puntero a la memoria que almacena el valor de ADGDR
 * @param[out] lpm: Puntero al registro que almacena el valor de ppm actual
 */
void contarPulsos(const uint32_t *reg_adc, uint8_t *ppm) {

  uint8_t r_flag = 0;

  for (int i = 0; i < CANT_MUESTRAS; i++) {
    uint32_t muestra = ((reg_adc[i]) >> 4) & 0xFFF;

    if (muestra >= VAL_UMBRAL && !r_flag) {
      (*ppm)++;
      r_flag = 1;
    }
    if (muestra < VAL_UMBRAL) {
      r_flag = 0;
    }
  }
}
