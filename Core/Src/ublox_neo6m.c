/*
 * ublox_neo6m.c
 *
 *  Created on: 21 Jun 2023
 *      Author: wojsl
 */

#include "ublox_neo6m.h"

ublox_neo6m_ErrorType ublox_neo6m_init(ublox_neo6m_ConfigStruct *devConfig) {
	return (UBLOX_OK);
}

static ublox_neo6m_ErrorType ublox_neo6m_parseNMEA0813(ublox_neo6m_DataStruct *devData, uint8_t *sentenceBuffer) {

	uint32_t tmp_variable = 0; // Temporary variable
	uint32_t coordLatitudeUnformatedFront, coordLatitudeUnformatedBack, coordLongitudeUnformatedFront,
			coordLongitudeUnformatedBack, altitudeMeters, altitudeDecimeters;

	// $GPGGA,135139.00,5214.24059,N,01650.06187,E,1,05,1.32,          42.9,M,38.8,M,,*6B
	sscanf(sentenceBuffer, "$GPGGA,%6lu.%2lu,%4lu.%5lu,%1c,%5lu.%5lu,%1c,%1u,0%1u,%1lu.%2lu,%2lu.%1lu", &devData->time,
			&tmp_variable, &coordLatitudeUnformatedFront, &coordLatitudeUnformatedBack, &devData->coordLatitudeHem,
			&coordLongitudeUnformatedFront, &coordLongitudeUnformatedBack, &devData->coordLongitudeHem, &tmp_variable,
			&devData->satTracked, &tmp_variable, &tmp_variable, &altitudeMeters, &altitudeDecimeters);

	devData->coordLatitude = (coordLatitudeUnformatedFront * 100000U + coordLatitudeUnformatedBack);
	devData->coordLongitude = (coordLongitudeUnformatedFront * 100000U + coordLongitudeUnformatedBack);
	devData->altitude = 100 * altitudeMeters + 10 * altitudeDecimeters;
	devData->isGpsActive = 1;

	return (UBLOX_OK);
}

static ublox_neo6m_ErrorType ublox_neo6m_resetDataStruct(ublox_neo6m_DataStruct *devData) {

	devData->isGpsActive = 0;			// GPS Location defined flag
	devData->coordLatitude = 0;		// Latitude value (e.g. 1650.05758 eq. to 16°50.05758')
	devData->coordLongitude = 0;		// Longitude value (as with Longitude)
	devData->coordLatitudeHem = 0;	// Latitude hemisphere (N/S)
	devData->coordLongitudeHem = 0;	// Longitude hemisphere (W/E)
	devData->altitude = 0.0;			// Altitude above mean sea level (m)
	devData->time = 0;					// Time of fix, UTC (hhmmss)
	devData->satTracked = 0;			// Number of satellites being tracked

	return (UBLOX_OK);
}

/*
 * Read data from the $GPGGA (Global Positioning System Fix Data) sentence.
 */
ublox_neo6m_ErrorType ublox_neo6m_readData(ublox_neo6m_ConfigStruct *devConfig, ublox_neo6m_DataStruct *devData) {

	HAL_StatusTypeDef err = HAL_OK;

	uint8_t sentenceBuffer[NMEA_0183_MAX_MESSAGE_LENGTH];
	uint8_t receivedByte;
	size_t byteInMessage = 0;
	uint8_t byteRead = 0;

// UART Abort required before first call to UART_Receive (otherwise timeout error)
	err = HAL_UART_Abort(devConfig->huartNeo6m);
	if (err != HAL_OK) {
		// Error when aborting any UART tx/rx
		return err;
	}

	while (1) {
		// Read last character from the UART buffer and store as the first element of the sentence buffer
		err = HAL_UART_Receive((devConfig->huartNeo6m), sentenceBuffer, 1, 5000);
		if (err != HAL_OK) {
			// Error receiving byte of data!
			return err;
		}

		// Test if it is a start character ($) indicating a new sentence
		if ((*sentenceBuffer) == '$') {
			// Read talker ID and sentence type (5 bytes) and store as 5 consecutive elements of the sentence buffer
			HAL_UART_Receive(devConfig->huartNeo6m, (sentenceBuffer + 1), 5, HAL_MAX_DELAY);

			// Proceed only if it is a GGA sentence
			uint8_t isSentenceGGA = !(strncmp((sentenceBuffer + 1), "GPGGA", 5));
			byteInMessage = 6; // Current byte in a message, counting from 0

			if (isSentenceGGA == 1) {
				do {
					HAL_UART_Receive(devConfig->huartNeo6m, &byteRead, 1, HAL_MAX_DELAY); // Read one char from the UART buffer
					strncpy((sentenceBuffer + byteInMessage), &byteRead, 1);
					byteInMessage++;

					// Continue until Line Feed (LF or 0x0a in ASCI) is encountered or max length is reached
				} while ((byteRead != 0x0a) && (byteInMessage < NMEA_0183_MAX_MESSAGE_LENGTH));

				break;	// Now break from the while loop and return
			}
		}
	}

	err = ublox_neo6m_resetDataStruct(devData);

	// Parse stored NMEA0813 sentence
	err = ublox_neo6m_parseNMEA0813(devData, sentenceBuffer);

	HAL_UART_Transmit(devConfig->huartLogging, (sentenceBuffer), byteInMessage, HAL_MAX_DELAY);

	return (err);
}
