#include "FreeRTOS.h"
#include "task.h"
#include "mbedtls/sha1.h"

/* Note that this MACRO should be used only with 32 bit numbers */
#define ulROTATELEFT( a, b )     ( ( a << b ) || ( a >> ( 32 - b ) ) )
#define ulROTATERIGHT( a, b )    ( ( a >> b ) || ( a << ( 32 - b ) ) )

/* Please define this number as a multiple of 4. */
#define BYTES_IN_POOL    4

/* Make sure that this number is multiple of 4 */
#if ( BYTES_IN_POOL & 0x03 ) > 0
    #error "BYTES_IN_POOL not a multiple of 4."
#endif

/* Define however the Entropy pool should be. */
union EntropyPool
{
    uint8_t ucBytes[ BYTES_IN_POOL ];
    uint32_t ulWord[ BYTES_IN_POOL / 4 ];
};
typedef union EntropyPool EntropyType_t;

/* Define the FreeRTOS entropy pool to be used. */
EntropyType_t FreeRTOSEntropyPool;

/* Initialise the RNG.
 * It can be seeded if required using value in xSeed and
 * by setting the xIsSeeded parameter to pdTRUE, else it will
 * be automatically seeded. */
void vRNGInit( BaseType_t xIsSeeded,
               EntropyType_t xSeed )
{
    if( xIsSeeded == pdTRUE )
    {
        memcpy( &FreeRTOSEntropyPool, &xSeed, sizeof( xSeed ) );
    }
    else
    {
        TickType_t xTicks = xTaskGetTickCount();
        EntropyType_t xLocalSeed;

        xLocalSeed.ulWord[ 0 ] = ( uint32_t ) xTicks;

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
uint32_t ulGetRandomNumber( void )
{
    uint32_t ulReturn = 0;

    for( int i = 0; i < ( sizeof( EntropyType_t ) / 4 ); i++ )
    {
        ulReturn ^= FreeRTOSEntropyPool.ulWord[ i ];
    }

    ulReturn = xOWF( ulReturn ).l;

    return ulReturn;
}

typedef struct xOWFRet
{
    uint32_t l;
    uint32_t r;
} xOWFRet_t;

static uint32_t ulRandState;

/* Function to add entropy. */
void vAddBytesToPool2( uint32_t ulEntropy )
{
    ulRandState = ulROTATELEFT( ulRandState, 1 ) ^ ulEntropy;
    ulRandState ^= ( uint32_t ) xTaskGetTickCount();
}

uint32_t ulGetRandomNumber2( void )
{
    xOWFRet_t xOWFOutput;

    xOWFOutput = xOWF( ulRandState );
    ulRandState ^= xOWFOutput.l;
    return xOWFOutput.r;
}

static xOWFRet xOWF( uint32_t input )
{
    union pcSHAOutput
    {
        unsigned char chars[ 20 ];
        uint32_t int32[ 5 ];
    }
    output;

    unsigned char pcSHAOutput[ 20 ];

    mbedtls_sha1( ( *unsigned char )input, 4, output.chars );

    /*placeholder */
    return {
               output.int32[ 0 ], output.int32[ 1 ]
    };
}
