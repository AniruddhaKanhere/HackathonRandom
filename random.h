void vRNGInit( BaseType_t xIsSeeded,
               EntropyType_t xSeed );
void vAddBytesToPoolFromISR( BaseType_t xISRNumber );
void vAddBytesToPool( uint32_t ulEntropy );
uint32_t ulGetRandomNumber( void );
