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

typedef struct {
	uint8_t messagesCount;
	uint8_t* burstBufferPtr;
	size_t* messageSizePtr;
} ublox_neo6m_DataTypeDef;

ublox_neo6m_DataTypeDef ublox_neo6m_init(UART_HandleTypeDef *huart, UART_HandleTypeDef *log_huart);

#ifdef __cplusplus
}
#endif

#endif /* __UBLOX_NEO6M_H */