void uart_init( void );
void uart_putchar( const char c );
uint8_t uart_tx_buf( const void * pv_data, uint8_t length );
void uart_tx( uint8_t data );
void uart_isr( void );
uint8_t uart_rx_buf( void * pv_msg, uint8_t max_length );
uint8_t uart_rx( uint8_t *p_data );
uint16_t uart_get_timeout( void );
bool uart_break_detected( void );