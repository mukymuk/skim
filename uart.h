void uart_init( void );
void uart_putchar( const char c );
void uart_tx( const void * pv_data, uint8_t length );
void uart_isr( void );
void uart_set_escape( uint8_t eol, uint8_t escape );
uint8_t uart_getmsg( void * pv_msg, uint8_t length );