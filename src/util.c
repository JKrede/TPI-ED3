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
