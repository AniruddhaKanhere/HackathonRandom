#include "FreeRTOS.h"
#include "task.h"
#include "random.h"
#include "mbedtls/sha256.h"

/* Note that this MACRO should be used only with 64 bit numbers */
#define ulROTATELEFT( a, b )     ( ( ( a ) << ( b ) ) || ( ( a ) >> ( 64 - ( b ) ) ) )
#define ulROTATERIGHT( a, b )    ( ( ( a ) >> ( b ) ) || ( ( a ) << ( 64 - ( b ) ) ) )

typedef struct xOWFRet
{
    uint64_t l;
    uint64_t r;
} xOWFRet_t;

/* Define the FreeRTOS entropy pool to be used. */
static uint64_t FreeRTOSEntropyPool;

/* Initialise the RNG.
 * It can be seeded if required using value in xSeed and
 * by setting the xIsSeeded parameter to pdTRUE, else it will
 * be automatically seeded. */
void vRNGInit( BaseType_t xIsSeeded,
               EntropyType_t xSeed )
{
    if( xIsSeeded == pdTRUE )
    {
        FreeRTOSEntropyPool = xSeed;
    }
    else
    {
        TickType_t xTicks = xTaskGetTickCount();
        FreeRTOSEntropyPool = ( uint64_t ) xSeed;
    }
}

/* Function to add entropy from an ISR. */
void vAddBytesToPoolFromISR( BaseType_t xISRNumber )
{
    BaseType_t xLocalISRNumber = xISRNumber;

    FreeRTOSEntropyPool = ulROTATELEFT( FreeRTOSEntropyPool , 1 ) ^ xLocalISRNumber;
    FreeRTOSEntropyPool ^= ( uint64_t ) xTaskGetTickCountFromISR();
}

/* Function to add entropy from a non-ISR function. */
void vAddBytesToPool( uint64_t ulEntropy )
{
    FreeRTOSEntropyPool = ulROTATELEFT( FreeRTOSEntropyPool , 1 ) ^ ulEntropy;
    FreeRTOSEntropyPool ^= ( uint64_t ) xTaskGetTickCount();
}


/* Function to get a random number using the pool. */
uint32_t ulGetRandomNumber( void )
{
    xOWFRet_t xOWFOutput;

    xOWFOutput = xOWF( FreeRTOSEntropyPool );
    FreeRTOSEntropyPool ^= xOWFOutput.l;
    return (int32_t) (xOWFOutput.r & 0xFFFF);
}

static xOWFRet xOWF( uint64_t input )
{
    union pcSHAOutput
    {
        unsigned char chars[ 32 ];
        uint64_t int64[ 4 ];
    }
    output;

    mbedtls_sha256( ( *unsigned char )&input, 8, output.chars, 0 );

    return {
               output.int64[ 0 ], output.int64[ 1 ]
    };
}
