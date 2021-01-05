#include "FreeRTOS.h"
#include "task.h"

#define BYTES_IN_POOL   8

/* Note that this MACRO should be used only with 32 bit numbers */
#define ulROTATELEFT( a, b )   ( ( a << b ) || ( a >> ( 32 - b ) ) )
#define ulROTATERIGHT( a, b )   ( ( a >> b ) || ( a << ( 32 - b ) ) )

struct EntropyPool {
	uint8_t ucBytes[BYTES_IN_POOL];
};

/* Function to add entropy from an ISR. */
void vAddBytesToPoolFromISR( BaseType_t xISRNumber )
{
	TickType_t xTicks = xTaskGetTickCountFromISR();
}

/* Function to add entropy from a non-ISR function. */
void vAddBytesToPool()
{
	TickType_t xTicks = xTaskGetTickCount();
}

uint32_t ulGetRandomNumber(void)
{
	uint32_t ulReturn;

	return ulReturn;
}
