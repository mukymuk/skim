void uart_init( void );
void uart_putchar( const char c );
void uart_tx( const void * pv_data, uint8_t length );
void uart_isr( void );