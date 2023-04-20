#ifndef QLCD_H
#define QLCD_H

#include "stm32f4xx_hal.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "usdelay.h"

#define QLCD_RETURN_HOME 	 0x02
#define QLCD_ENTRY_MODE_SET  0x04
#define QLCD_DISPLAY_CONTROL 0x08
#define QLCD_CURSOR_SHIFT 	 0x10
#define QLCD_FUNCTION_SET 	 0x20
#define QLCD_SET_CGRAM_ADDR  0x40
#define QLCD_SET_DDRAM_ADDR  0x80
#define QLCD_ENTRY_SH        0x01
#define QLCD_ENTRY_ID        0x02
#define QLCD_DISPLAY_B       0x01
#define QLCD_DISPLAY_C       0x02
#define QLCD_DISPLAY_D       0x04
#define QLCD_SHIFT_RL        0x04
#define QLCD_SHIFT_SC        0x08
#define QLCD_FUNCTION_F      0x04
#define QLCD_FUNCTION_N      0x08
#define QLCD_FUNCTION_DL     0x10

void QLCD_Init(GPIO_TypeDef* PORT, uint16_t RS, uint16_t E, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7);
void QLCD_Enable();
void QLCD_Write(uint8_t byte);
void QLCD_SendCommand(uint8_t cmd);
void QLCD_SendData(uint8_t data);
void QLCD_WriteCommand(uint8_t nibble);
void QLCD_CursorPosition(uint8_t row, uint8_t col);
void SetRowNumber(int n);
void QLCD_CursorOff();
void QLCD_CursorOn();
void QLCD_Clear();
void QLCD_BlinkOn();
void QLCD_BlinkOff();
void QLCD_DisplayOff();
void QLCD_DisplayOn();
void QLCD_ShiftToRight(uint8_t n);
void QLCD_ShiftToLeft(uint8_t n);
void QLCD_PrintChr(char ch);
void QLCD_PrintStr(char text[]);
void QLCD_PrintInt(int number);
void QLCD_Printf(const char *format, ...);

#endif
