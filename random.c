#include "FreeRTOS.h"
#include "task.h"

/* Please define this number as a multiple of 4. */
#define BYTES_IN_POOL   4

/* Make sure that this number is multiple of 4 */
#if ( BYTES_IN_POOL & 0x03 ) > 0
    #error "BYTES_IN_POOL not a multiple of 4."
#endif

/* Note that this MACRO should be used only with 32 bit numbers */
#define ulROTATELEFT( a, b )   ( ( a << b ) || ( a >> ( 32 - b ) ) )
#define ulROTATERIGHT( a, b )   ( ( a >> b ) || ( a << ( 32 - b ) ) )

/* Define however the Entropy pool should be. */
union EntropyPool {
	uint8_t ucBytes[BYTES_IN_POOL];
	uint32_t ulWord[ BYTES_IN_POOL / 4 ];
};
typedef union EntropyPool EntropyType_t;

/* Define the FreeRTOS entropy pool to be used. */
EntropyType_t  FreeRTOSEntropyPool;

uint32_t ulSHA1( uint32_t input )
{
	uint32_t ulReturn = input;

	return ulReturn;
}

/* Initialise the RNG.
 * It can be seeded if required using value in xSeed and
 * by setting the xIsSeeded parameter to pdTRUE, else it will
 * be automatically seeded. */
void vRNGInit( BaseType_t xIsSeeded, EntropyType_t xSeed )
{
	if( xIsSeeded == pdTRUE )
	{
	    memcpy( &FreeRTOSEntropyPool, &xSeed, sizeof( xSeed ) );
	}
	else
	{
		TickType_t xTicks = xTaskGetTickCount();
		EntropyType_t xLocalSeed;

		xLocalSeed.ulWord[0] = ulSHA1( ( uint32_t ) xTicks );

		memcpy( &FreeRTOSEntropyPool, &xLocalSeed, sizeof( xSeed ) );
	}
}

/* Function to add entropy from an ISR. */
void vAddBytesToPoolFromISR( BaseType_t xISRNumber )
{
	TickType_t xTicks = xTaskGetTickCountFromISR();
	BaseType_t xLocalISRNumber = xISRNumber;

	uint32_t ulNumber = ( ( uint32_t ) xTicks ^ ( uint32_t ) xLocalISRNumber );


}

/* Function to add entropy from a non-ISR function. */
void vAddBytesToPool()
{
	TickType_t xTicks = xTaskGetTickCount();
}

/* Function to get a random number from the SHA1 of the pool. */
uint32_t ulGetRandomNumber(void)
{
	uint32_t ulReturn = 0;

	for( int i =0; i < ( sizeof( EntropyType_t ) / 4 ); i++ )
		ulReturn ^= FreeRTOSEntropyPool.ulWord[i];

	ulReturn = ulSHA1( ulReturn );

	return ulReturn;
}
