void uart_init( void );
void uart_putchar( const char c );
void uart_tx_buf( const void * pv_data, uint8_t length );
void uart_tx( uint8_t data );
void uart_isr( void );
uint8_t uart_rx( void * pv_msg, uint8_t max_length );