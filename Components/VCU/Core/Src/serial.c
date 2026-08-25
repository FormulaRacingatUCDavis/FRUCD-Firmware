#include "serial.h"

void Serial_Init(Serial_t* serial, UART_HandleTypeDef* h_uart, uint8_t* buf, uint8_t len)
{
	serial->h_uart = h_uart;
	serial->buf = buf;
	serial->len = len;
	serial->index = 0;
}

HAL_StatusTypeDef Serial_StartListening(Serial_t* serial)
{
	return HAL_UART_Receive_DMA(serial->h_uart, serial->buf, serial->len);
}

uint32_t Serial_BytesAvailable(Serial_t* serial)
{
	uint32_t dma_index = (serial->len - serial->h_uart->hdmarx->Instance->NDTR);

	if(dma_index >= serial->index)
	{
		return dma_index - serial->index;
	}
	else
	{
		return (dma_index + serial->len) - serial->index;
	}
}

// Must ensure bytes are available using Serial_BytesAvailable before calling this function
uint8_t Serial_GetByte(Serial_t* serial)
{
	uint8_t byte = serial->buf[serial->index];
	serial->index++;
	if(serial->index >= serial->len) serial->index = 0;
	return byte;
}

HAL_StatusTypeDef Serial_SendBytes(Serial_t* serial, const uint8_t* bytes, uint16_t len, uint32_t timeout_ms)
{
	return HAL_UART_Transmit(serial->h_uart, bytes, len, timeout_ms);
}
