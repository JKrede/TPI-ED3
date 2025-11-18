/* USER CODE BEGIN Header */
/*
  ******************************************************************************
  * @file    buffer_and_usb.c
  * @brief   Implementación de buffer circular y envío de datos por USB.
  *
  * @author  Font Julián
  * @version 1.0.0
  ******************************************************************************
  * @attention
  * Funciones asociadas a la utilización de la estructura de datos buffer
  * circular y la transmisión de datos por USB OTG casteados a Q15.
  ******************************************************************************
  *	HISTORIAL DE CAMBIOS:
  *	v1.0.0		Creación de la librería
  *******************************************************************************
*/

/* USER CODE END Header */
#include "circular_buffer.h"
#include <string.h>
#include "arm_math.h"




/*
  * @brief  Verifica si el buffer circular está vacío
  * @param  buf Puntero a la estructura del buffer circular
  * @retval int Devuelve 1 si el buffer está vacío, 0 en caso contrario
  * @note   Función helper estática inline
  */
static inline int isBufferEmpty(circularBuffer_t *buf) {
    return (buf->head == buf->tail);
}

/*
  * @brief  Escribe datos en el buffer circular
  * @param  buf  Puntero a la estructura del buffer circular
  * @param  data Puntero a los datos a escribir
  * @retval None
  * @note   Descarta silenciosamente los datos si el buffer está lleno (sin sobreescritura)
  */

void bufferWrite(circularBuffer_t *buf, float32_t *data) {
    uint16_t next = (buf->head + 1) % CIRCULAR_BUFFER_SIZE;
    if (next == buf->tail) {
        // Buffer lleno. Por el momento se ignora el dato.
        return;
    }
    buf->buf[buf->head] = *data;
    buf->head = next;
}

/**
  * @brief  Lee datos del buffer circular
  * @param  buf  Puntero a la estructura del buffer circular
  * @param  data Puntero donde almacenar el dato leído
  * @retval None
  * @note   No realiza acción si el buffer está vacío
  */
void bufferRead(circularBuffer_t *buf, float32_t *data) {
    if (isBufferEmpty(buf)) {
        // Buffer vacío: no hay nada que leer.
        return;
    }
    *data = buf->buf[buf->tail];
    buf->tail = (buf->tail + 1) % CIRCULAR_BUFFER_SIZE;
}

/**
  * @brief  Calcula los bytes disponibles en el buffer
  * @param  buf Puntero a la estructura del buffer circular
  * @retval uint16_t Número de bytes disponibles para lectura
  */
uint16_t availableBytes(circularBuffer_t *buf) {
    if (buf->head >= buf->tail)
        return buf->head - buf->tail;
    else
        return (CIRCULAR_BUFFER_SIZE - buf->tail) + buf->head;
}

