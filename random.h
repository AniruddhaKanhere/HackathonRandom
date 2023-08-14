void vRNGInit( BaseType_t xIsSeeded,
               uint64_t xSeed );

void vAddBytesToPoolFromISR( BaseType_t xISRNumber );
void vAddBytesToPool( uint64_t ulEntropy );
uint32_t ulGetRandomNumber( void );
