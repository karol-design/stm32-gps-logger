/**
 ******************************************************************************
 * @file           : ublox_neo6m.h
 * @brief          : Header for ublox_neo6m.c file.
 *                   This file contains the u-blox neo6m driver defines.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UBLOX_NEO6M_H
#define __UBLOX_NEO6M_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32l4xx_hal.h"

#define NMEA_0183_MAX_MESSAGE_LENGTH 82	// Messages have a maximum length of 82 characters
#define NEO_6M_MAX_MESSAGES_PER_BURST 9	// NEO-6M sends 9 messages per burst at most

/**
 * @brief  u-blox NEO-6M Driver Status structure definition
 */
typedef enum {
	UBLOX_OK = 0x00,
	UBLOX_ERROR = 0x01
} ublox_neo6m_ErrorType;

/**
 * @brief  u-blox NEO-6M Driver Configuration structure definition
 */
typedef struct {
	UART_HandleTypeDef *huartNeo6m;		// UART used to communicate with NEO-6M
	UART_HandleTypeDef *huartLogging;	// UART used for logging messages
} ublox_neo6m_ConfigStruct;

typedef struct {
	uint8_t isGpsActive;	// GPS Location defined flag
	int32_t coordLatitude;	// Latitude value (e.g. 1650.05758 eq. to 16°50.05758')
	int32_t coordLongitude;	// Longitude value (as with Longitude)
	char coordLatitudeHem;	// Latitude hemisphere (N/S)
	char coordLongitudeHem;	// Longitude hemisphere (W/E)
	float altitude;			// Altitude above mean sea level (m)
	uint32_t time;			// Time of fix, UTC (hhmmss)
	uint8_t satTracked;		// Number of satellites being tracked
} ublox_neo6m_DataStruct;

ublox_neo6m_ErrorType ublox_neo6m_init(ublox_neo6m_ConfigStruct *devConfig);
ublox_neo6m_ErrorType ublox_neo6m_readData(ublox_neo6m_ConfigStruct *devConfig, ublox_neo6m_DataStruct *devData);

#ifdef __cplusplus
}
#endif

#endif /* __UBLOX_NEO6M_H */
