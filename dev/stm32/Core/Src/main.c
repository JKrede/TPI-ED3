/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define FREQ_SAMPLING 1000
#define LED_VERDE GPIO_PIN_12
#define LED_NARANJA GPIO_PIN_13
#define LED_ROJO GPIO_PIN_14
#define LED_AZUL GPIO_PIN_15

#define ECG_30_SIZE (sizeof(ecg_30ppm)/sizeof(ecg_30ppm[0]))
#define ECG_60_SIZE (sizeof(ecg_60ppm)/sizeof(ecg_60ppm[0]))
#define ECG_120_SIZE (sizeof(ecg_120ppm)/sizeof(ecg_120ppm[0]))
#define ECG_180_SIZE (sizeof(ecg_180ppm)/sizeof(ecg_180ppm[0]))

volatile uint16_t i = 0;
volatile uint8_t n = 0;

const uint16_t ecg_30ppm[2000] = {
    189, 189, 189, 189, 189, 189, 189, 189, 189, 190,
    190, 190, 190, 190, 190, 190, 190, 190, 190, 190,
    190, 190, 190, 190, 190, 190, 190, 190, 190, 190,
    190, 190, 191, 191, 191, 191, 191, 191, 191, 191,
    191, 191, 191, 191, 191, 191, 191, 191, 191, 191,
    191, 191, 191, 191, 191, 191, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    193, 193, 193, 193, 193, 193, 193, 193, 193, 193,
    193, 193, 193, 193, 193, 193, 193, 193, 193, 193,
    193, 193, 193, 193, 194, 194, 194, 194, 194, 194,
    194, 194, 194, 194, 194, 194, 194, 194, 194, 194,
    194, 194, 194, 194, 194, 194, 194, 194, 194, 195,
    195, 195, 195, 195, 195, 195, 195, 195, 195, 195,
    195, 195, 195, 195, 195, 195, 195, 195, 195, 195,
    195, 195, 195, 195, 195, 196, 196, 196, 196, 196,
    196, 196, 196, 196, 196, 196, 196, 196, 196, 196,
    196, 196, 196, 196, 196, 196, 196, 196, 196, 196,
    196, 196, 197, 197, 197, 197, 197, 197, 197, 197,
    197, 197, 197, 197, 197, 197, 197, 197, 197, 197,
    197, 197, 197, 197, 197, 197, 197, 197, 197, 198,
    198, 198, 198, 198, 198, 198, 198, 198, 198, 198,
    198, 198, 198, 198, 198, 198, 198, 198, 198, 198,
    198, 198, 198, 198, 198, 198, 198, 199, 199, 199,
    199, 199, 199, 199, 199, 199, 199, 199, 199, 199,
    199, 199, 199, 199, 199, 199, 199, 199, 199, 199,
    199, 199, 199, 199, 199, 199, 200, 200, 200, 200,
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
    200, 200, 200, 200, 200, 200, 201, 201, 201, 201,
    201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
    201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
    201, 201, 201, 201, 201, 201, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 204, 204, 204, 204, 204, 204, 204, 204, 204,
    204, 204, 204, 204, 204, 204, 204, 204, 204, 204,
    204, 204, 204, 204, 204, 204, 204, 204, 204, 204,
    204, 204, 204, 204, 205, 205, 205, 205, 205, 205,
    205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
    205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
    205, 205, 205, 205, 205, 205, 205, 205, 205, 206,
    206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
    206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
    206, 206, 206, 206, 206, 206, 206, 206, 206, 206,
    206, 206, 206, 206, 206, 206, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208, 208, 208,
    208, 209, 209, 209, 209, 209, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 210, 210, 210, 210,
    210, 210, 210, 210, 210, 210, 211, 211, 211, 211,
    211, 212, 212, 212, 212, 213, 213, 213, 214, 214,
    214, 215, 215, 216, 216, 217, 217, 218, 219, 219,
    220, 221, 221, 222, 223, 224, 225, 226, 227, 228,
    229, 231, 232, 233, 235, 236, 238, 239, 241, 243,
    244, 246, 248, 250, 252, 254, 257, 259, 261, 264,
    266, 268, 271, 274, 276, 279, 282, 284, 287, 290,
    293, 296, 298, 301, 304, 307, 310, 313, 316, 319,
    322, 325, 328, 331, 333, 336, 339, 342, 344, 347,
    349, 351, 354, 356, 358, 360, 362, 363, 365, 366,
    368, 369, 370, 371, 372, 372, 373, 373, 373, 373,
    373, 373, 373, 372, 372, 371, 370, 369, 368, 366,
    365, 363, 361, 360, 358, 356, 353, 351, 349, 346,
    344, 341, 338, 336, 333, 330, 327, 324, 321, 318,
    315, 312, 309, 306, 302, 299, 296, 293, 290, 287,
    284, 281, 278, 275, 272, 270, 267, 264, 262, 259,
    256, 254, 252, 249, 247, 245, 243, 241, 239, 237,
    235, 233, 231, 230, 228, 226, 225, 224, 222, 221,
    220, 219, 218, 217, 216, 215, 214, 213, 212, 211,
    211, 210, 209, 209, 208, 208, 207, 207, 206, 206,
    206, 205, 205, 205, 204, 204, 204, 204, 203, 203,
    203, 203, 203, 203, 203, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 202, 202, 202, 202, 201, 201, 200, 200,
    199, 198, 198, 197, 196, 194, 193, 192, 190, 188,
    186, 184, 182, 179, 176, 173, 170, 166, 162, 159,
    155, 150, 146, 142, 137, 133, 129, 125, 121, 118,
    115, 113, 111, 109, 108, 108, 109, 111, 115, 119,
    126, 134, 144, 156, 170, 186, 204, 224, 247, 271,
    298, 327, 358, 391, 426, 462, 501, 540, 582, 624,
    668, 713, 758, 804, 849, 894, 938, 980, 1021, 1059,
    1096, 1129, 1160, 1187, 1211, 1232, 1249, 1262, 1271, 1275,
    1276, 1272, 1265, 1252, 1236, 1215, 1191, 1162, 1129, 1093,
    1054, 1013, 970, 925, 878, 831, 782, 734, 685, 636,
    588, 540, 494, 448, 404, 362, 322, 283, 247, 213,
    182, 154, 128, 105, 84, 66, 51, 37, 26, 17,
    10, 5, 2, 0, 0, 1, 4, 7, 12, 17,
    23, 30, 37, 44, 51, 59, 66, 73, 81, 88,
    95, 102, 108, 114, 120, 126, 131, 136, 140, 144,
    148, 152, 155, 158, 160, 163, 165, 167, 168, 170,
    171, 172, 173, 174, 175, 176, 177, 177, 178, 178,
    178, 179, 179, 179, 179, 180, 180, 180, 180, 180,
    180, 180, 181, 181, 181, 181, 181, 181, 181, 181,
    181, 181, 181, 181, 181, 181, 181, 182, 182, 182,
    182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
    182, 182, 182, 182, 182, 182, 182, 183, 183, 183,
    183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
    183, 183, 183, 183, 183, 183, 183, 184, 184, 184,
    184, 184, 184, 184, 184, 184, 184, 184, 184, 184,
    184, 184, 184, 184, 184, 184, 184, 185, 185, 185,
    185, 185, 185, 185, 185, 185, 185, 185, 185, 185,
    185, 185, 185, 185, 185, 185, 186, 186, 186, 186,
    186, 186, 186, 186, 186, 186, 186, 186, 186, 186,
    186, 186, 187, 187, 187, 187, 187, 187, 187, 187,
    187, 187, 187, 187, 187, 188, 188, 188, 188, 188,
    188, 188, 188, 188, 188, 189, 189, 189, 189, 189,
    189, 189, 189, 190, 190, 190, 190, 190, 190, 191,
    191, 191, 191, 191, 192, 192, 192, 192, 193, 193,
    193, 194, 194, 194, 195, 195, 195, 196, 196, 196,
    197, 197, 198, 198, 199, 199, 200, 200, 201, 201,
    202, 202, 203, 204, 204, 205, 206, 207, 207, 208,
    209, 210, 211, 212, 213, 214, 215, 216, 217, 218,
    219, 220, 221, 222, 224, 225, 226, 227, 229, 230,
    232, 233, 235, 236, 238, 240, 241, 243, 245, 247,
    248, 250, 252, 254, 256, 258, 260, 262, 264, 267,
    269, 271, 273, 276, 278, 281, 283, 285, 288, 291,
    293, 296, 298, 301, 304, 306, 309, 312, 315, 318,
    321, 323, 326, 329, 332, 335, 338, 341, 344, 347,
    350, 353, 356, 359, 362, 365, 368, 371, 374, 377,
    380, 383, 386, 389, 392, 395, 398, 401, 403, 406,
    409, 412, 414, 417, 420, 422, 425, 427, 429, 432,
    434, 436, 439, 441, 443, 445, 447, 448, 450, 452,
    453, 455, 456, 458, 459, 460, 461, 463, 463, 464,
    465, 466, 466, 467, 467, 468, 468, 468, 468, 468,
    468, 468, 467, 467, 467, 466, 465, 465, 464, 463,
    462, 461, 459, 458, 457, 455, 454, 452, 450, 449,
    447, 445, 443, 441, 439, 436, 434, 432, 429, 427,
    424, 422, 419, 417, 414, 411, 408, 405, 402, 399,
    396, 393, 390, 387, 384, 381, 378, 375, 371, 368,
    365, 362, 358, 355, 352, 349, 345, 342, 339, 336,
    332, 329, 326, 323, 319, 316, 313, 310, 307, 304,
    301, 298, 295, 292, 289, 286, 283, 280, 277, 274,
    272, 269, 266, 263, 261, 258, 256, 253, 251, 248,
    246, 244, 241, 239, 237, 235, 233, 230, 228, 226,
    225, 223, 221, 219, 217, 215, 214, 212, 211, 209,
    207, 206, 205, 203, 202, 200, 199, 198, 197, 196,
    194, 193, 192, 191, 190, 189, 188, 187, 187, 186,
    185, 184, 183, 183, 182, 181, 181, 180, 179, 179,
    178, 178, 177, 177, 176, 176, 175, 175, 174, 174,
    174, 173, 173, 173, 172, 172, 172, 172, 171, 171,
    171, 171, 171, 170, 170, 170, 170, 170, 170, 170,
    170, 169, 169, 169, 169, 169, 169, 169, 169, 169,
    169, 169, 169, 169, 169, 169, 169, 169, 169, 169,
    169, 169, 169, 169, 169, 169, 169, 169, 169, 169,
    169, 169, 169, 169, 169, 169, 169, 169, 169, 169,
    169, 169, 169, 169, 170, 170, 170, 170, 170, 170,
    170, 170, 170, 170, 170, 170, 170, 170, 170, 170,
    170, 170, 170, 170, 171, 171, 171, 171, 171, 171,
    171, 171, 171, 171, 171, 171, 171, 171, 171, 171,
    171, 172, 172, 172, 172, 172, 172, 172, 172, 172,
    172, 172, 172, 172, 172, 172, 172, 173, 173, 173,
    173, 173, 173, 173, 173, 173, 173, 173, 173, 173,
    173, 173, 173, 173, 174, 174, 174, 174, 174, 174,
    174, 174, 174, 174, 174, 174, 174, 174, 174, 174,
    174, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 176, 176,
    176, 176, 176, 176, 176, 176, 176, 176, 176, 176,
    176, 176, 176, 176, 176, 177, 177, 177, 177, 177,
    177, 177, 177, 177, 177, 177, 177, 177, 177, 177,
    177, 177, 178, 178, 178, 178, 178, 178, 178, 178,
    178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
    179, 179, 179, 179, 179, 179, 179, 179, 179, 179,
    179, 179, 179, 179, 179, 179, 179, 179, 179, 180,
    180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
    180, 180, 180, 180, 180, 180, 180, 181, 181, 181,
    181, 181, 181, 181, 181, 181, 181, 181, 181, 181,
    181, 181, 181, 181, 181, 181, 182, 182, 182, 182,
    182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
    182, 182, 182, 182, 182, 182, 183, 183, 183, 183,
    183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
    183, 183, 183, 183, 183, 183, 184, 184, 184, 184,
    184, 184, 184, 184, 184, 184, 184, 184, 184, 184,
    184, 184, 184, 184, 184, 184, 185, 185, 185, 185,
    185, 185, 185, 185, 185, 185, 185, 185, 185, 185,
    185, 185, 185, 185, 185, 185, 186, 186, 186, 186,
    186, 186, 186, 186, 186, 186, 186, 186, 186, 186,
    186, 186, 186, 186, 186, 186, 186, 187, 187, 187,
    187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
    187, 187, 187, 187, 187, 187, 187, 187, 187, 188,
    188, 188, 188, 188, 188, 188, 188, 188, 188, 188,
    188, 188, 188, 188, 188, 188, 188, 188, 188, 188,
    188, 189, 189, 189, 189, 189, 189, 189, 189, 189
};


const uint16_t ecg_180ppm[334] = {
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 202, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 204, 204,
    204, 204, 204, 204, 204, 204, 204, 204, 205, 205,
    205, 206, 206, 207, 207, 208, 209, 210, 211, 212,
    214, 216, 218, 220, 223, 226, 229, 233, 237, 242,
    247, 252, 258, 264, 271, 278, 285, 293, 301, 309,
    317, 325, 333, 340, 348, 355, 361, 367, 372, 377,
    381, 383, 385, 386, 386, 385, 383, 381, 377, 372,
    367, 361, 354, 347, 340, 332, 324, 316, 308, 300,
    292, 284, 277, 270, 263, 256, 250, 245, 240, 235,
    230, 227, 223, 220, 217, 215, 212, 211, 209, 207,
    206, 204, 203, 201, 199, 196, 192, 188, 183, 176,
    168, 159, 148, 137, 126, 115, 105, 98, 96, 99,
    111, 133, 166, 212, 273, 348, 437, 539, 651, 770,
    891, 1010, 1121, 1219, 1297, 1351, 1379, 1380, 1352, 1298,
    1219, 1118, 1000, 872, 741, 611, 488, 375, 274, 189,
    120, 68, 31, 9, 0, 1, 9, 23, 40, 60,
    79, 98, 115, 131, 144, 155, 164, 172, 177, 181,
    185, 187, 189, 190, 191, 192, 193, 194, 194, 195,
    196, 196, 197, 198, 199, 200, 201, 202, 204, 205,
    207, 209, 211, 213, 215, 218, 220, 223, 226, 229,
    233, 237, 241, 245, 249, 254, 259, 265, 270, 276,
    282, 288, 295, 302, 309, 316, 324, 331, 339, 347,
    355, 364, 372, 380, 388, 397, 405, 413, 421, 429,
    436, 444, 451, 458, 464, 470, 476, 481, 486, 490,
    494, 497, 500, 502, 503, 504, 504, 504, 503, 501,
    499, 496, 493, 489, 484, 480, 474, 469, 462, 456,
    449, 442, 435, 427, 419, 411, 403, 395, 386, 378,
    369, 361, 352, 344, 336, 327, 319, 312, 304, 296,
    289, 282, 276, 269, 263, 257, 252, 247, 242, 237,
    232, 228, 224, 221, 217, 214, 211, 208, 206, 203,
    201, 199, 197, 196
};
const uint16_t ecg_120ppm[500] = {
    192, 192, 193, 193, 193, 193, 193, 193, 193, 193,
    193, 194, 194, 194, 194, 194, 194, 194, 194, 194,
    194, 195, 195, 195, 195, 195, 195, 195, 195, 195,
    195, 195, 195, 196, 196, 196, 196, 196, 196, 196,
    196, 196, 196, 196, 197, 197, 197, 197, 197, 197,
    197, 197, 197, 197, 197, 197, 198, 198, 198, 198,
    198, 198, 198, 198, 198, 198, 198, 199, 199, 199,
    199, 199, 199, 199, 199, 199, 199, 199, 199, 200,
    200, 200, 200, 200, 200, 200, 200, 200, 201, 201,
    201, 201, 201, 201, 201, 202, 202, 202, 202, 203,
    203, 204, 204, 205, 205, 206, 207, 208, 209, 210,
    211, 213, 214, 216, 218, 220, 223, 226, 229, 232,
    236, 240, 244, 248, 253, 258, 263, 269, 274, 280,
    286, 293, 299, 305, 312, 318, 325, 331, 338, 344,
    350, 355, 360, 365, 369, 373, 376, 379, 381, 383,
    383, 384, 383, 382, 381, 379, 376, 372, 369, 364,
    359, 354, 348, 343, 336, 330, 324, 317, 311, 304,
    298, 291, 285, 279, 273, 267, 262, 257, 252, 247,
    243, 239, 235, 231, 228, 225, 222, 220, 218, 216,
    214, 212, 211, 210, 209, 208, 207, 206, 206, 205,
    205, 204, 204, 204, 203, 203, 202, 201, 200, 199,
    198, 196, 194, 191, 188, 183, 178, 172, 165, 158,
    149, 140, 131, 122, 114, 106, 101, 99, 101, 107,
    119, 139, 166, 201, 246, 301, 366, 439, 521, 609,
    703, 801, 899, 995, 1086, 1168, 1239, 1297, 1340, 1366,
    1374, 1362, 1332, 1283, 1219, 1141, 1052, 955, 852, 745,
    639, 536, 439, 350, 270, 200, 141, 93, 55, 28,
    11, 2, 0, 4, 13, 25, 39, 54, 70, 86,
    101, 116, 128, 140, 150, 158, 165, 171, 176, 180,
    183, 186, 188, 189, 190, 191, 192, 192, 193, 193,
    194, 194, 194, 194, 195, 195, 195, 195, 196, 196,
    196, 197, 197, 197, 198, 198, 199, 199, 200, 200,
    201, 202, 203, 203, 204, 205, 206, 207, 209, 210,
    211, 213, 214, 216, 218, 220, 222, 224, 227, 229,
    232, 235, 238, 241, 244, 247, 251, 255, 259, 263,
    267, 271, 276, 281, 286, 291, 296, 301, 307, 313,
    319, 325, 331, 338, 344, 351, 357, 364, 371, 377,
    384, 391, 397, 404, 411, 417, 423, 430, 436, 442,
    447, 453, 458, 464, 469, 473, 478, 482, 486, 490,
    493, 496, 499, 501, 503, 504, 506, 506, 507, 507,
    506, 505, 504, 502, 500, 497, 494, 491, 488, 484,
    480, 475, 470, 465, 460, 455, 449, 443, 437, 431,
    425, 418, 412, 405, 398, 391, 385, 378, 371, 364,
    357, 350, 344, 337, 330, 324, 318, 311, 305, 299,
    294, 288, 283, 277, 272, 268, 263, 258, 254, 250,
    246, 242, 239, 235, 232, 229, 226, 223, 220, 218,
    216, 213, 211, 209, 208, 206, 204, 203, 202, 201,
    199, 198, 198, 197, 196, 195, 194, 194, 193, 193,
    192, 192, 192, 191, 191, 191, 191, 190, 190, 190,
    190, 190, 190, 190, 190, 190, 190, 190, 190, 190
};

const uint16_t ecg_60ppm[1000] = {
    196, 196, 196, 196, 196, 196, 196, 197, 197, 197,
    197, 197, 197, 197, 197, 197, 197, 197, 197, 197,
    197, 198, 198, 198, 198, 198, 198, 198, 198, 198,
    198, 198, 198, 198, 198, 199, 199, 199, 199, 199,
    199, 199, 199, 199, 199, 199, 199, 199, 199, 200,
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
    200, 200, 200, 200, 201, 201, 201, 201, 201, 201,
    201, 201, 201, 201, 201, 201, 201, 201, 201, 202,
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
    202, 202, 202, 202, 203, 203, 203, 203, 203, 203,
    203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
    204, 204, 204, 204, 204, 204, 204, 204, 204, 204,
    204, 204, 204, 204, 204, 205, 205, 205, 205, 205,
    205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
    205, 206, 206, 206, 206, 206, 206, 206, 206, 206,
    206, 206, 206, 206, 206, 206, 206, 206, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208, 208, 208,
    208, 209, 209, 209, 209, 209, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 210, 210, 210,
    210, 210, 210, 210, 210, 210, 210, 210, 210, 210,
    210, 210, 210, 210, 211, 211, 211, 211, 211, 211,
    211, 211, 211, 211, 211, 211, 211, 211, 211, 211,
    211, 211, 211, 212, 212, 212, 212, 212, 212, 212,
    212, 212, 213, 213, 213, 213, 213, 213, 214, 214,
    214, 214, 215, 215, 215, 216, 216, 216, 217, 217,
    218, 219, 219, 220, 221, 222, 223, 224, 225, 227,
    228, 230, 231, 233, 235, 237, 239, 241, 243, 246,
    248, 251, 254, 257, 260, 263, 267, 270, 274, 278,
    282, 286, 290, 294, 299, 303, 308, 313, 317, 322,
    326, 331, 335, 340, 344, 348, 353, 357, 361, 364,
    368, 372, 375, 378, 381, 384, 386, 388, 390, 392,
    393, 394, 395, 395, 395, 395, 394, 393, 392, 390,
    388, 386, 383, 380, 377, 374, 371, 367, 363, 359,
    355, 351, 347, 342, 338, 333, 328, 324, 319, 314,
    309, 305, 300, 296, 291, 287, 282, 278, 274, 270,
    267, 263, 260, 256, 253, 250, 247, 244, 242, 239,
    237, 235, 232, 231, 229, 227, 225, 224, 223, 221,
    220, 219, 218, 217, 217, 216, 215, 215, 214, 214,
    213, 213, 213, 212, 212, 212, 212, 211, 211, 211,
    211, 211, 211, 211, 211, 211, 211, 211, 211, 211,
    211, 211, 211, 211, 211, 211, 211, 211, 211, 211,
    211, 211, 211, 211, 211, 211, 210, 210, 210, 210,
    209, 209, 208, 207, 206, 205, 203, 201, 199, 197,
    194, 191, 187, 183, 179, 174, 168, 162, 156, 150,
    143, 137, 130, 124, 119, 114, 110, 108, 107, 108,
    112, 119, 129, 142, 160, 182, 208, 238, 273, 314,
    359, 408, 463, 521, 583, 648, 715, 784, 854, 923,
    992, 1058, 1121, 1180, 1233, 1280, 1320, 1351, 1375, 1389,
    1394, 1389, 1375, 1351, 1319, 1278, 1229, 1173, 1110, 1043,
    972, 898, 822, 745, 669, 594, 522, 452, 387, 326,
    270, 219, 173, 133, 99, 70, 47, 29, 16, 6,
    1, 0, 2, 6, 12, 20, 30, 40, 51, 63,
    74, 86, 97, 107, 117, 127, 135, 143, 150, 157,
    162, 167, 172, 176, 179, 182, 184, 186, 188, 190,
    191, 192, 193, 193, 194, 194, 195, 195, 195, 195,
    196, 196, 196, 196, 196, 196, 196, 196, 197, 197,
    197, 197, 197, 197, 197, 197, 197, 197, 197, 197,
    197, 198, 198, 198, 198, 198, 198, 198, 198, 198,
    198, 198, 199, 199, 199, 199, 199, 199, 199, 199,
    199, 200, 200, 200, 200, 200, 200, 200, 201, 201,
    201, 201, 201, 202, 202, 202, 202, 203, 203, 203,
    203, 204, 204, 204, 205, 205, 206, 206, 207, 207,
    208, 208, 209, 209, 210, 211, 211, 212, 213, 214,
    215, 215, 216, 217, 218, 220, 221, 222, 223, 225,
    226, 227, 229, 230, 232, 234, 235, 237, 239, 241,
    243, 245, 248, 250, 252, 255, 257, 260, 263, 266,
    269, 272, 275, 278, 281, 285, 288, 292, 295, 299,
    303, 307, 311, 315, 319, 323, 327, 331, 336, 340,
    345, 349, 354, 358, 363, 368, 372, 377, 382, 387,
    391, 396, 401, 406, 410, 415, 420, 424, 429, 433,
    438, 442, 447, 451, 455, 459, 463, 467, 471, 474,
    478, 481, 484, 487, 490, 493, 496, 498, 500, 502,
    504, 506, 507, 509, 510, 511, 511, 512, 512, 512,
    512, 512, 511, 511, 510, 509, 508, 506, 505, 503,
    501, 499, 496, 494, 491, 488, 485, 482, 478, 475,
    471, 467, 463, 459, 455, 451, 446, 442, 437, 432,
    428, 423, 418, 413, 409, 404, 399, 394, 389, 384,
    379, 374, 369, 364, 359, 355, 350, 345, 340, 336,
    331, 326, 322, 317, 313, 309, 304, 300, 296, 292,
    288, 284, 280, 276, 273, 269, 266, 262, 259, 256,
    253, 250, 247, 244, 241, 239, 236, 234, 231, 229,
    227, 225, 223, 221, 219, 217, 216, 214, 213, 211,
    210, 208, 207, 206, 205, 204, 203, 202, 201, 200,
    199, 198, 197, 197, 196, 195, 195, 194, 194, 193,
    193, 192, 192, 192, 191, 191, 191, 190, 190, 190,
    190, 190, 189, 189, 189, 189, 189, 189, 189, 189,
    189, 189, 189, 189, 189, 189, 189, 189, 189, 189,
    189, 189, 189, 189, 189, 189, 189, 189, 189, 189,
    189, 189, 189, 189, 189, 189, 189, 189, 189, 189,
    190, 190, 190, 190, 190, 190, 190, 190, 190, 190,
    190, 190, 190, 191, 191, 191, 191, 191, 191, 191,
    191, 191, 191, 191, 191, 191, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    193, 193, 193, 193, 193, 193, 193, 193, 193, 193,
    193, 193, 193, 194, 194, 194, 194, 194, 194, 194,
    194, 194, 194, 194, 194, 194, 194, 195, 195, 195,
    195, 195, 195, 195, 195, 195, 195, 195, 195, 195,
    195, 196, 196, 196, 196, 196, 196, 196, 196, 196
};
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);      // habilita DAC OUT1
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1,    // valor inicial (evita 0V hasta la 1ª IRQ)
                   DAC_ALIGN_12B_R, ecg_60ppm[0]);
  HAL_TIM_Base_Start_IT(&htim2);            // TIM2 genera interrupción a 1 kHz

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_SCK_Pin SPI1_MISO_Pin SPI1_MOSI_Pin */
  GPIO_InitStruct.Pin = SPI1_SCK_Pin|SPI1_MISO_Pin|SPI1_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : I2S3_MCK_Pin I2S3_SCK_Pin I2S3_SD_Pin */
  GPIO_InitStruct.Pin = I2S3_MCK_Pin|I2S3_SCK_Pin|I2S3_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : VBUS_FS_Pin */
  GPIO_InitStruct.Pin = VBUS_FS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_FS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_FS_ID_Pin OTG_FS_DM_Pin OTG_FS_DP_Pin */
  GPIO_InitStruct.Pin = OTG_FS_ID_Pin|OTG_FS_DM_Pin|OTG_FS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Audio_SCL_Pin Audio_SDA_Pin */
  GPIO_InitStruct.Pin = Audio_SCL_Pin|Audio_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {

		switch(n){
			case 0:	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, ecg_30ppm[i]);
					i = (i + 1)%ECG_30_SIZE;
					break;
			case 1:	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, ecg_60ppm[i]);
					i = (i + 1)%ECG_60_SIZE;
					break;
			case 2: HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, ecg_120ppm[i]);
					i = (i + 1)%ECG_120_SIZE;
					break;
			case 3:	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, ecg_180ppm[i]);
					i = (i + 1)%ECG_180_SIZE;
					break;
		}
    }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if (GPIO_Pin == GPIO_PIN_0)
    {
    	i = 0;
    	HAL_GPIO_WritePin(GPIOD, LED_ROJO, 0);
    	HAL_GPIO_WritePin(GPIOD, LED_VERDE, 0);
    	HAL_GPIO_WritePin(GPIOD, LED_NARANJA, 0);
    	HAL_GPIO_WritePin(GPIOD, LED_AZUL, 0);


    	n = (n + 1)%4;

    	switch(n){
    	case 0:	HAL_GPIO_WritePin(GPIOD, LED_ROJO, 1); break;
    	case 1:	HAL_GPIO_WritePin(GPIOD, LED_VERDE, 1); break;
    	case 2:	HAL_GPIO_WritePin(GPIOD, LED_NARANJA, 1); break;
    	case 3:	HAL_GPIO_WritePin(GPIOD, LED_AZUL, 1); break;

    	}
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
