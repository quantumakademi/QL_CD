#include "QL_CD.h"

/*
  Quantum Lojik (info@quantumlojik.com | www.quantumlojik.com)
  Hitachi/HD44780 ve uyumlu chip'e sahip
  16*2 LCD'ler için geliştirilmiştir.
  v1.0 Nisan 2023
  Aykut Taşdelen
*/

static GPIO_TypeDef* port;

static uint16_t PIN_RS, PIN_E, D4_PIN, D5_PIN, D6_PIN, D7_PIN;

static uint8_t dc = 0x0F, fs = 0x38;  // display control ve function set

// E pin'ini önce 1'e, sonra 0'a çeker
void QLCD_Enable()
{
	HAL_GPIO_WritePin(port, PIN_E, 1);
	Delay_US(10);
	HAL_GPIO_WritePin(port, PIN_E, 0);
	Delay_US(60);
}

// RS pin'ini 0'a çeker ve komutu command reg'e yazar
void QLCD_WriteCommand(uint8_t nibble)
{
	uint8_t LSB_nibble = nibble & 0xF;

	HAL_GPIO_WritePin(port, PIN_RS, 0);

	HAL_GPIO_WritePin(port, D4_PIN, (GPIO_PinState) (LSB_nibble & 0x1));
	HAL_GPIO_WritePin(port, D5_PIN, (GPIO_PinState) (LSB_nibble & 0x2));
	HAL_GPIO_WritePin(port, D6_PIN, (GPIO_PinState) (LSB_nibble & 0x4));
	HAL_GPIO_WritePin(port, D7_PIN, (GPIO_PinState) (LSB_nibble & 0x8));

	QLCD_Enable();
}


/* 4 bit mod'da parametresine gönderilen 8 bit'lik veriyi
 * iki nibble'a bölüp yüksek ve düşük anlamlı kısımları
 * port pinlerine gönderir
*/
void QLCD_Write(uint8_t byte)
{
	uint8_t LSB_nibble = byte & 0xF;
	uint8_t MSB_nibble = (byte >> 4) & 0xF;

	HAL_GPIO_WritePin(port, D4_PIN, (GPIO_PinState) (MSB_nibble & 0x1));
	HAL_GPIO_WritePin(port, D5_PIN, (GPIO_PinState) (MSB_nibble & 0x2));
	HAL_GPIO_WritePin(port, D6_PIN, (GPIO_PinState) (MSB_nibble & 0x4));
	HAL_GPIO_WritePin(port, D7_PIN, (GPIO_PinState) (MSB_nibble & 0x8));

	QLCD_Enable();

	HAL_GPIO_WritePin(port, D4_PIN, (GPIO_PinState) (LSB_nibble & 0x1));
	HAL_GPIO_WritePin(port, D5_PIN, (GPIO_PinState) (LSB_nibble & 0x2));
	HAL_GPIO_WritePin(port, D6_PIN, (GPIO_PinState) (LSB_nibble & 0x4));
	HAL_GPIO_WritePin(port, D7_PIN, (GPIO_PinState) (LSB_nibble & 0x8));

	QLCD_Enable();
}

// Komut gönderir
void QLCD_SendCommand(uint8_t command)
{
	HAL_GPIO_WritePin(port, PIN_RS, 0);
	QLCD_Write(command);
}

// Veri gönderir
void QLCD_SendData(uint8_t data)
{
	HAL_GPIO_WritePin(port, PIN_RS, 1);
	QLCD_Write(data);
}

/*
 * Port ve pin ayarlamalarını gerçekleştirir.
   1. parametresi: Hangi port'un kullanılacağını belirler (Örn: GPIOA)
   2. parametresi: RS ucunun bağlandığı pin no (Örn: GPIO_PIN_0)
   3. parametresi: E ucunun bağlandığı pin no (Örn: GPIO_PIN_1)
   4, 5, 6, 7. parametreler: Data pinlerinin bağlandığı pin numaralarıdır (Örn: GPIO_PIN_5)
*/
void QLCD_Init(GPIO_TypeDef* PORT, uint16_t RS, uint16_t E, uint16_t D4, uint16_t D5, uint16_t D6, uint16_t D7)
{
	port = PORT;
	PIN_RS = RS;
	PIN_E = E;
	D4_PIN = D4;
	D5_PIN = D5;
	D6_PIN = D6;
	D7_PIN = D7;

	Timer_Config();

	fs = 0x28;

	HAL_Delay(20);

	for (int i = 1; i <= 3; ++i)
	{
		QLCD_WriteCommand(0x3);
		HAL_Delay(5);
	}

	QLCD_WriteCommand(0x2);
	HAL_Delay(5);

	QLCD_DisplayOn();
	QLCD_Clear();
}

// Cursor'ı (row 1 | 2, col 1 | 16) olmak üzere konumlar
void QLCD_CursorPosition(uint8_t row, uint8_t col)
{
	uint8_t tmp = (col - 1) & 0x0F;

	if (row == 1)
	{
		tmp |= (0x80);
		QLCD_SendCommand(tmp);
	}
	if (row == 2)
	{
		tmp |= (0xc0);
		QLCD_SendCommand(tmp);
	}
}

// Satır adedini belirler
void QLCD_SetRowNumber(int n)
{
	if (n == 1)
	{
		fs &= ~(0x08);
	}
	if (n == 2)
	{
		fs |= ~(0x08);
	}
	QLCD_SendCommand(fs);
}

// Alt tire görünmez
void QLCD_CursorOff()
{
	dc &= ~(0x02);
	QLCD_SendCommand(dc);
}

// Alt tire görünür
void QLCD_CursorOn()
{
	dc |= (0x02);
	QLCD_SendCommand(dc);
}

// Ekranı temizler
void QLCD_Clear()
{
	QLCD_SendCommand(0x01);
	HAL_Delay(3);
}

// Kare görünmez
void QLCD_BlinkOff()
{
	dc &= ~(0x01);
	QLCD_SendCommand(dc);
}

// Kare görünür + blink yapar
void QLCD_BlinkOn()
{
	dc |= 0x01;
	QLCD_SendCommand(dc);
}

// Ekranı kapatır
void QLCD_DisplayOff()
{
	dc &= ~(0x04);
	QLCD_SendCommand(dc);
}

// Ekranı açar
void QLCD_DisplayOn()
{
	dc |= (0x04);
	QLCD_SendCommand(dc);
}

// Satırı n adet sağa kaydırır
void QLCD_ShiftToRight(uint8_t n)
{
	for (uint8_t i = 0; i < n; i++)
	{
		QLCD_SendCommand(0x1c);
	}
}

// Satırı n adet sola kaydırır
void QLCD_ShiftToLeft(uint8_t n)
{
	for (uint8_t i = 0; i < n; i++)
	{
		QLCD_SendCommand(0x18);
	}
}

// Karakter gösterir
void QLCD_PrintChr(char ch)
{
	if (ch != '\0')
	{
		QLCD_SendData((uint8_t) ch);
	}
}


// String gösterir
void QLCD_PrintStr(char text[])
{
	for (uint8_t i = 0; i <= 15; i++)
	{
		if (text[i] != '\0')
		{
			QLCD_SendData((uint8_t) text[i]);
		}
		else
		{
			return;
		}
	}
}

// Tamsayı gösterir
void QLCD_PrintInt(int number)
{
	char buf[16];
	sprintf(buf, "%d", number);
	QLCD_PrintStr(buf);
}


// Formatlı text gösterir
void QLCD_Printf(const char *format, ...)
{
	char buf[16];

	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	QLCD_PrintStr(buf);
}

