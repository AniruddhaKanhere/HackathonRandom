#include "FreeRTOS.h"
#include "task.h"
#include "random.h"
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

typedef struct xOWFRet
{
    uint32_t l;
    uint32_t r;
} xOWFRet_t;

/* Define the FreeRTOS entropy pool to be used. */
static uint32_t FreeRTOSEntropyPool;

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
        FreeRTOSEntropyPool = ( uint32_t ) xSeed;
    }
}

/* Function to add entropy from an ISR. */
void vAddBytesToPoolFromISR( BaseType_t xISRNumber )
{
    BaseType_t xLocalISRNumber = xISRNumber;

    FreeRTOSEntropyPool = ulROTATELEFT( FreeRTOSEntropyPool , 1 ) ^ xLocalISRNumber;
    FreeRTOSEntropyPool ^= ( uint32_t ) xTaskGetTickCountFromISR();
}

/* Function to add entropy from a non-ISR function. */
void vAddBytesToPool( uint32_t ulEntropy )
{
    FreeRTOSEntropyPool = ulROTATELEFT( FreeRTOSEntropyPool , 1 ) ^ ulEntropy;
    FreeRTOSEntropyPool ^= ( uint32_t ) xTaskGetTickCount();
}


/* Function to get a random number using the pool. */
uint32_t ulGetRandomNumber( void )
{
    xOWFRet_t xOWFOutput;

    xOWFOutput = xOWF( FreeRTOSEntropyPool );
    FreeRTOSEntropyPool ^= xOWFOutput.l;
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
