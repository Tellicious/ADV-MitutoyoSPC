/* BEGIN Header */
/**
 ******************************************************************************
 * \file            MitutoyoSPC.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MITUTOYO_SPC_H__
#define __MITUTOYO_SPC_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Macros --------------------------------------------------------------------*/

#define MITUTOYO_SPC_DATA_LENGTH 13U /* Number of 4-bit digits in SPC data frame */

/* Typedefs ------------------------------------------------------------------*/

/*
*  Return status
*/
typedef enum {
    SPC_STATUS_SUCCESS = 0,
    SPC_STATUS_ONGOING,
    SPC_STATUS_INVALID_DATA,
    SPC_STATUS_ERROR,
} MitutoyoSPC_status_t;

/*
*  Measurement unit
*/
typedef enum {
    SPC_UNIT_MM = 0,
    SPC_UNIT_INCH,
} MitutoyoSPC_unit_t;

/*
* Mitutoyo SPC data structure
*/
typedef struct {
    uint8_t data[MITUTOYO_SPC_DATA_LENGTH]; /* Array of 4-bit digits */
    float measurementValue;                 /* Calculated measurement value */
    MitutoyoSPC_unit_t unitOfMeasure;       /* Measurement unit */

    uint8_t _bitIdx, _receivedBit, _bytesStored;
} MitutoyoSPC_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Initialize data struct to prepare for new data from Mitutoyo SPC protocol
 *
 * \param[in]       spc: pointer to the MitutoyoSPC_t structure
 *
 * \return          return MITUTOYO_SPC_STATUS_SUCCESS if struct initialized correctly, MITUTOYO_SPC_STATUS_ERROR if empty pointer
 */
MitutoyoSPC_status_t MitutoyoSPC_startReception(MitutoyoSPC_t* const spc);

/**
 * \brief           Process a single bit received from Mitutoyo SPC protocol
 *
 * \param[in]       spc: pointer to the MitutoyoSPC_t structure
 * \param[in]       bitValue: bit value to process
 *
 * \return          return MITUTOYO_SPC_STATUS_SUCCESS if an entire frame is received, MITUTOYO_SPC_STATUS_ONGOING if more data expected, MITUTOYO_SPC_STATUS_ERROR if error in data
 */
MitutoyoSPC_status_t MitutoyoSPC_processBit(MitutoyoSPC_t* const spc, const uint8_t bitValue);

/**
 * \brief           Process the entire frame received from Mitutoyo SPC protocol
 *
 * \param[in]       spc: pointer to the MitutoyoSPC_t structure
 *
 * \return          return MITUTOYO_SPC_STATUS_SUCCESS if all data valid, MITUTOYO_SPC_STATUS_INVALID_DATA if invalid data received, MITUTOYO_SPC_STATUS_ERROR if error in data
 */
MitutoyoSPC_status_t MitutoyoSPC_processFrame(MitutoyoSPC_t* const spc);

/**
 * \brief           Get the parsed measurement value
 * \param[in]       spc: pointer to the MitutoyoSPC_t structure
 * \return          measurement value
 */
#define MitutoyoSPC_getMeasurementValue(spc) ((spc)->measurementValue)

/**
 * \brief           Get the parsed measurement unit
 * \param[in]       spc: pointer to the MitutoyoSPC_t structure
 * \return          measurement unit
 */
#define MitutoyoSPC_getUnitOfMeasure(spc)    ((spc)->unitOfMeasure)

#ifdef __cplusplus
}
#endif

#endif /* __MITUTOYO_SPC_H__ */
