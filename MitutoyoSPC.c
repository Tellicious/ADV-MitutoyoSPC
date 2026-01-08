/* BEGIN Header */
/**
 ******************************************************************************
 * \file            MitutoyoSPC.c
 * \author          Andrea Vivani
 * \brief           Parser for Mitutoyo SPC protocol
 ******************************************************************************
 * \copyright
 *
 * Copyright 2026 Andrea Vivani
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 ******************************************************************************
 */
/* END Header */

/* Includes ------------------------------------------------------------------*/

#include "MitutoyoSPC.h"
#include <math.h>

/* Macros --------------------------------------------------------------------*/
#define MITUTOYO_SPC_BITS_PER_NIBBLE    4U
#define MITUTOYO_SPC_HEADER_BYTE_1      0U
#define MITUTOYO_SPC_HEADER_BYTE_2      1U
#define MITUTOYO_SPC_HEADER_BYTE_3      2U
#define MITUTOYO_SPC_HEADER_BYTE_4      3U
#define MITUTOYO_SPC_SIGN_BYTE          4U
#define MITUTOYO_SPC_MSD_BYTE           5U
#define MITUTOYO_SPC_LSD_BYTE           10U
#define MITUTOYO_SPC_DECIMAL_POINT_BYTE 11U
#define MITUTOYO_SPC_UNIT_BYTE          12U
#define MITUTOYO_SPC_HEADER_VALUE       0x0FU
#define MITUTOYO_SPC_LOW_BIT_MASK       0x0FU
#define MITUTOYO_SPC_SIGN_BIT_MASK      0x08U

/* Functions -----------------------------------------------------------------*/

MitutoyoSPC_status_t MitutoyoSPC_startReception(MitutoyoSPC_t* const spc) {
    if (spc == NULL) {
        return SPC_STATUS_ERROR;
    }
    spc->_bitIdx = 0;
    spc->_receivedBit = 0;
    spc->_bytesStored = 0;
    for (uint8_t ii = 0; ii < MITUTOYO_SPC_DATA_LENGTH; ii++) {
        spc->data[ii] = 0;
    }
    spc->measurementValue = 0.0f;
    spc->unitOfMeasure = SPC_UNIT_MM;
    return SPC_STATUS_SUCCESS;
}

MitutoyoSPC_status_t MitutoyoSPC_processBit(MitutoyoSPC_t* const spc, const uint8_t bitValue) {
    if ((spc == NULL) || (spc->_bitIdx >= MITUTOYO_SPC_BITS_PER_NIBBLE) || (spc->_bytesStored >= MITUTOYO_SPC_DATA_LENGTH)) {
        return SPC_STATUS_ERROR;
    }
    spc->_receivedBit |= ((bitValue & 0x01U) << spc->_bitIdx);
    spc->_bitIdx++;
    if (spc->_bitIdx == MITUTOYO_SPC_BITS_PER_NIBBLE) {
        /* A full nibble (4 bits) has been received */
        spc->data[spc->_bytesStored] = (spc->_receivedBit & MITUTOYO_SPC_LOW_BIT_MASK); /* Store only lower 4 bits */
        spc->_bitIdx = 0;
        spc->_receivedBit = 0;
        spc->_bytesStored++;
        if (spc->_bytesStored == MITUTOYO_SPC_DATA_LENGTH) {
            return SPC_STATUS_SUCCESS; /* All data received */
        }
    }
    return SPC_STATUS_ONGOING; /* More bits expected */
}

MitutoyoSPC_status_t MitutoyoSPC_processFrame(MitutoyoSPC_t* const spc) {
    if ((spc == NULL) || (spc->_bytesStored != MITUTOYO_SPC_DATA_LENGTH)) {
        return SPC_STATUS_ERROR;
    }

    if (spc->data[MITUTOYO_SPC_HEADER_BYTE_1] != MITUTOYO_SPC_HEADER_VALUE || spc->data[MITUTOYO_SPC_HEADER_BYTE_2] != MITUTOYO_SPC_HEADER_VALUE
        || spc->data[MITUTOYO_SPC_HEADER_BYTE_3] != MITUTOYO_SPC_HEADER_VALUE || spc->data[MITUTOYO_SPC_HEADER_BYTE_4] != MITUTOYO_SPC_HEADER_VALUE) {
        return SPC_STATUS_INVALID_DATA; /* Invalid header */
    }

    float value = 0.0f;
    for (uint8_t ii = MITUTOYO_SPC_MSD_BYTE; ii <= MITUTOYO_SPC_LSD_BYTE; ii++) {
        value = value * 10 + spc->data[ii];
    }
    value /= fpowf(10, spc->data[MITUTOYO_SPC_DECIMAL_POINT_BYTE]);                                            /* Adjust for decimal position */
    spc->measurementValue = (spc->data[MITUTOYO_SPC_SIGN_BYTE] & MITUTOYO_SPC_SIGN_BIT_MASK) ? -value : value; /* Apply sign */
    spc->unitOfMeasure = (spc->data[MITUTOYO_SPC_UNIT_BYTE] == 0U) ? SPC_UNIT_MM : SPC_UNIT_INCH;              /* Set measurement unit */

    return SPC_STATUS_SUCCESS;
}
