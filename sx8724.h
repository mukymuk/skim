
bool sx8724_isr( void );
void sx8724_init( void );
void sx8724_i2c_isr( void );
float sx8724_offset( uint8_t channel, float offset );
float sx8724_gain( uint8_t channel, float gain );
