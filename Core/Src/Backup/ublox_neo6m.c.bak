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

/*
 * Read data from the $GPGGA (Global Positioning System Fix Data) sentence.
 */
ublox_neo6m_ErrorType ublox_neo6m_readData(ublox_neo6m_ConfigStruct *devConfig, ublox_neo6m_DataStruct *devData) {

	uint8_t sentenceBuffer[NMEA_0183_MAX_MESSAGE_LENGTH];
	uint8_t byteInMessage = 0;
	uint8_t byteRead = 0;

	while (1) {
		// Read last character from the UART buffer and store as the first element of the sentence buffer
		HAL_StatusTypeDef ret = HAL_UART_Receive(devConfig->huartNeo6m, (sentenceBuffer), 1, 10000);
		if (ret == HAL_TIMEOUT) {
			HAL_UART_Transmit(devConfig->huartLogging, "Err, HAL Timeout!", sizeof("Err, HAL Timeout!"),
			HAL_MAX_DELAY);
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

	HAL_UART_Transmit(devConfig->huartLogging, (sentenceBuffer), byteInMessage, HAL_MAX_DELAY);

	return (UBLOX_OK);
}
