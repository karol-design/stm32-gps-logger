/*
 * ublox_neo6m.c
 *
 *  Created on: 21 Jun 2023
 *      Author: wojsl
 */

#include "ublox_neo6m.h"

ublox_neo6m_ErrorType ublox_neo6m_init(ublox_neo6m_ConfigStruct *devConfig){
	return UBLOX_OK;
}

ublox_neo6m_ErrorType ublox_neo6m_readData(ublox_neo6m_ConfigStruct *devConfig, ublox_neo6m_DataStruct *devData){

	uint8_t burstBuffer[NEO_6M_MAX_MESSAGES_PER_BURST][NMEA_0183_MAX_MESSAGE_LENGTH];
	size_t messageSize[NEO_6M_MAX_MESSAGES_PER_BURST];

	uint8_t messageCount = 0;
	uint8_t byteInMessage = 0;
	uint8_t burstBegan = 0;
	uint8_t byteRead = 0;

	while (1) {
		// Read only one character from the UART buffer and store as the first element of the current message array
		HAL_UART_Receive(devConfig->huartNeo6m, (*(burstBuffer + messageCount)), 1, HAL_MAX_DELAY);

		// Test if it is a start character ($)
		if (*(*(burstBuffer + messageCount)) == '$') {
			// New sentence received
			// Read talker ID and sentence type (5 bytes) and store as 5 consecutive elements in the current message array
			HAL_UART_Receive(devConfig->huartNeo6m, (*(burstBuffer + messageCount) + 1), 5, HAL_MAX_DELAY);

			// Test if it is an RMC sentence (which is first in every data burst of NEO-6M)
			uint8_t isSentenceRMC = !(strncmp((*(burstBuffer + messageCount) + 1), "GPRMC", 5));
			if (isSentenceRMC == 1) {
				messageCount = 0; // Current message in a burst
				strncpy(*(burstBuffer + messageCount), "$GPRMC", 6);
				// RMC Sentence decoded
				burstBegan = 1;
			}

			byteInMessage = 6; // Current byte in a message, counting from 0

			do {
				if (byteInMessage >= NMEA_0183_MAX_MESSAGE_LENGTH) {
					break;
				}

				HAL_UART_Receive(devConfig->huartNeo6m, &byteRead, 1, HAL_MAX_DELAY); // Read only one character from the UART buffer
				strncpy((*(burstBuffer + messageCount) + byteInMessage), &byteRead, 1);
				byteInMessage++;
			} while (byteRead != 0x0a); // Continue until Line Feed (LF or 0x0a in ASCI) is encountered

			messageSize[messageCount] = (byteInMessage);

			// Test if it is an GLL sentence (which is last in every data burst of NEO-6M)
			uint8_t isSentenceGLL = !(strncmp((*(burstBuffer + messageCount) + 1), "GPGLL", 5));

			messageCount++;

			// Break if no. of messages limit was reached or last message in a burst was read
			if (messageCount >= NEO_6M_MAX_MESSAGES_PER_BURST || (isSentenceGLL == 1 && burstBegan == 1)) {
				break;
			}
		}
	}

	for (int i = 0; i < messageCount; i++) {
		HAL_UART_Transmit(devConfig->huartLogging, (*(burstBuffer + i)), *(messageSize + i), HAL_MAX_DELAY);
	}

	return UBLOX_OK;
}