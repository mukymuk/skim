
bool sx8724_isr( void );
void sx8724_init( void );
void sx8724_i2c_isr( void );
void sx8724_gain_offset( uint8_t channel, float * p_gain, float *p_offset );