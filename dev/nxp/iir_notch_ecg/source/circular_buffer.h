

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_



#include <stdint.h>
#include "arm_math.h"
#define CIRCULAR_BUFFER_SIZE 1024
/**
 * @defgroup BufferCircular Estructura del Búfer Circular
 * @{
 */

/**
 * @brief Estructura de búfer circular para datos Q15
 *
 * Búfer de almacenamiento temporal para operaciones de lectura/escritura no bloqueantes.
 * Utiliza índices de head y tail para acceso concurrente seguro.
 */
typedef struct {
    volatile float32_t buf[CIRCULAR_BUFFER_SIZE];  /**< Almacenamiento de datos en formato Q15 */
    volatile uint16_t head;                        /**< Índice de escritura */
    volatile uint16_t tail;                        /**< Índice de lectura */
} circularBuffer_t;
/** @} */

/**
 * @brief Escribe datos en el buffer circular
 * @param buf Puntero al búfer destino
 * @param data Datos a escribir (formato Q15)
 * @note Maneja sobreescritura automática de datos no leídos
 * @return None
 */
void bufferWrite(circularBuffer_t *buf, float32_t *data);

/**
 * @brief Lee datos del buffer circular
 * @param buf Puntero al buffer fuente
 * @param data Destino para los datos leídos (formato Q15)
 * @return None
 */
void bufferRead(circularBuffer_t *buf, float32_t *data);
/** @} */



#endif /* CIRCULAR_BUFFER_H_ */
