#include "global.h"
#include "sx8724.h"
#include <string.h>

// 5.8PSI full scale = 100mV max
// Vref = 1.19/1.22/1.25
// PGA1 = 10
// sensor offset = +-25mV

enum state_t
{
    state_idle,
    state_register_access_write,
    state_register_access_read
};

enum state_i2c_t
{
    state_i2c_idle,
    state_i2c_device_address,
    state_i2c_register_address,
    state_i2c_register_value_read,
    state_i2c_register_value_write,
    state_i2c_restart,
    state_i2c_stop,
    state_i2c_start
};

struct gain_offset_t
{
    uint8_t reserved1 : 4;
    uint8_t pga3_enable : 1;
    uint8_t pga2_enable : 1;
    uint8_t pga1_enable : 1;
    uint8_t adc_enable : 1;
    uint8_t setfs : 2;
    uint8_t pga2_gain : 2;
    uint8_t pga2_offset : 4;
    uint8_t pga1_gain : 1;
    uint8_t pga3_gain : 7;
    uint8_t reserved2 : 1;
    uint8_t pga3_offset : 7;
};
struct channel_t
{
    float                   pga3_gain;
    struct gain_offset_t    gain_offset;

};

#define SX8724_CHANNEL_COUNT    3
static struct channel_t s_channel[SX8724_CHANNEL_COUNT];

static enum state_i2c_t     s_state_i2c;
static enum state_t         s_state;
static uint8_t               s_i2c_addr;


static uint8_t              s_tx_buffer[8];
static uint8_t              s_tx_write_ndx;
static uint8_t              s_tx_read_ndx;
static volatile uint8_t     s_tx_available;
static uint8_t              s_count;

static uint8_t              s_rx_buffer[8];
static uint8_t              s_rx_read_ndx;

static uint16_t             s_stall_count;

static bool s_interrupt_pending = false;

#define TRANSACTION_READ            0x80
#define TRANSACTION_WRITE           0x00
#define TRANSACTION_COUNT_MASK      0x7F

#define SX8724_I2C_READ             0x01
#define SX8724_I2C_WRITE            0x00
#define SX8724_I2C_ADDR             0x90
#define SX8724_I2C_RESET_ADDR       0x00
#define SX8724_REG_RESET            0x06


#define SX8724_REG_RCOSC            0x30
#define SX8724_REG_RCOSC_ENABLE     0x01
#define SX8724_REG_RCOSC_DISABLE    0x00


static inline uint8_t read_byte( void )
{
    uint8_t byte = s_tx_buffer[s_tx_read_ndx++];
    if(s_tx_read_ndx >= ARRAY_SIZE(s_tx_buffer))
        s_tx_read_ndx = 0;
   s_tx_available++;
   return byte;
}
static inline void write_byte( uint8_t byte )
{
    s_tx_buffer[s_tx_write_ndx++] = byte;
    if(s_tx_write_ndx >= ARRAY_SIZE(s_tx_buffer))
        s_tx_write_ndx = 0;
    s_tx_available--;
}

static inline void xfer_data( void )
{
    if( s_count )
    {
        s_count--;
    }
    if( !s_count )
    {
        s_state_i2c =  state_i2c_stop;
    }
}

static void tx( uint8_t reg_addr, bool read, const uint8_t *p_data, uint8_t count )
{
    uint8_t i;
    while( s_tx_available < (count + 2) )
    {
        s_stall_count++;
        sleep();
    }
    PIE3bits.SSP2IE = 0;
    write_byte(reg_addr);
    write_byte( count | (read ? TRANSACTION_READ : TRANSACTION_WRITE) );
    for(i=0;i<count;i++)
    {
        write_byte( p_data[i] );
    }
    if( s_state_i2c == state_i2c_idle )
    {
        SSP2CON2bits.SEN = 1;
        s_state_i2c = state_i2c_device_address;
    }
    PIE3bits.SSP2IE = 1;
}

void sx8724_reset( void )
{
    // resets the SX8724
    uint8_t data = SX8724_REG_RESET;
    s_i2c_addr = SX8724_I2C_RESET_ADDR;
    tx(SX8724_REG_RESET,TRANSACTION_WRITE,NULL,0);
    memset(s_channel,0,sizeof(s_channel));
    for(uint8_t i=0;i<ARRAY_SIZE(s_channel);i++)
    {
        struct channel_t * p = &s_channel[i];
        p->pga3_gain = 1.0;
        p->gain_offset.pga3_gain = 12;
    }
}
void sx8724_init( void )
{
    PIE3bits.SSP2IE = 0;
    TRISDbits.RD0 = 1;      // interrupt line from adc (pin isn't interrupt capable though...)
    TRISDbits.TRISD5 = 1;   // SDA
    SSP2CON1 = 0b00111000;  // i2c mode
    SSP2ADD  = 19;          // bitrate = FOSC/(4*(SSP2ADD+1)) <= 400kHz
    s_tx_available = ARRAY_SIZE(s_tx_buffer);
    s_tx_write_ndx = 0;
    s_tx_read_ndx = 0;
    s_count = 0;
    PIR3bits.SSP2IF = 0;
    PIE3bits.SSP2IE = 1;
    sx8724_reset();
}

bool sx8724_isr( void )
{
    // polling scheme to simulate interrupt
    if( s_interrupt_pending && PORTDbits.RD0 )
    {
        s_interrupt_pending = false;
    }
    return s_interrupt_pending;
}

static void ack_error( void )
{
    // i2c ack errors are fatal
    while(1);
}

void sx8724_start( void )
{
    uint8_t data = SX8724_REG_RCOSC_ENABLE;
    tx(SX8724_REG_RCOSC,TRANSACTION_WRITE,&data, sizeof(data));

}

#define PGA3_MAX_GAIN   10.58
#define PGA2_MAX_GAIN   10.0
#define PGA1_MAX_GAIN   10.0

struct pga2_offset_t
{
    uint8_t reg_value;
    float   offset;
};

float sx8724_gain( uint8_t channel, float gain )
{
   
    uint8_t i = 0;
    float out_gain = 1.0, original_gain = gain;
    struct gain_offset_t *p_channel = (struct gain_offset_t*)&s_channel[channel].gain_offset;

    // clamp maximum gain
    if( gain > (PGA3_MAX_GAIN*PGA2_MAX_GAIN*PGA1_MAX_GAIN) )
        gain = (PGA3_MAX_GAIN*PGA2_MAX_GAIN*PGA1_MAX_GAIN);
    
    if( gain > PGA2_MAX_GAIN*PGA3_MAX_GAIN )
    {
        // pga1 is required
        p_channel->pga1_enable = 1;
        p_channel->pga1_gain = 1; // G = 10.0 only
        out_gain = 10.0;
        gain /= 10.0;
    }
    else
    {
        // pga1 is not required
        p_channel->pga1_enable = 0;
        p_channel->pga1_gain = 0;   // G = 1.0
    }
    if( gain > PGA3_MAX_GAIN )
    {
        // pga2 is required
        static const float pga2_gains[3] = { 2.0, 5.0, 10.0 }; // list of gains pga2 is capable of
        uint8_t pga2_gain_ndx = 0;
        float pga2_min_gain =  gain / PGA3_MAX_GAIN;   // minimum gain required from pga2
        p_channel->pga2_enable = 1;
        while( pga2_min_gain > pga2_gains[pga2_gain_ndx] )
            pga2_gain_ndx++;
        p_channel->pga2_gain = pga2_gain_ndx+1;
        gain /= pga2_gains[pga2_gain_ndx];
        out_gain *= pga2_gains[pga2_gain_ndx];
    }
    else
    {
        // pga2 is not required
        p_channel->pga2_enable = 0;
        p_channel->pga2_gain = 0; // G = 1.0
    }
   
    if( gain != 1.0 || p_channel->pga1_enable==0 && p_channel->pga2_enable==0)
    {
        // pga3 is required if gain isn't unity or if both of the other pga's are
        // not enabled.  At least one pga must be enabled in order to satisfy
        // adc input impedance requirements.
        p_channel->pga3_enable = 1;
        s_channel[channel].pga3_gain = round(out_gain*12.0)/12.0;
        p_channel->pga3_gain = ((uint8_t)s_channel[channel].pga3_gain)*12;
        out_gain *= s_channel[channel].pga3_gain;
    }
    else
    {
        // pga3 isn't required
        p_channel->pga3_enable = 0;
        p_channel->pga3_gain = 12;  // G = 1.0
    }
    // return the system gain coef for this channel
    return original_gain / out_gain;
}

float sx8724_offset( uint8_t channel, float offset )
{
    // sx8724_gain() should be called before this function
    // because offsets depend on gain settings
    struct gain_offset_t *p_channel = (struct gain_offset_t*)&s_channel[channel].gain_offset;
    float pga3_gain = s_channel[channel].pga3_gain;
    
    uint8_t pga_offset;

    
    float pga2_offset =  round(5.0*offset/pga3_gain);
    float pga3_offset = round((offset-pga2_offset)/12.0);

    if( p_channel->pga2_enable==0 && pga2_offset != 0.0 )
    {
        // turn pga2 on and set gain to 1 if we only need its
        // contribution to offset
        p_channel->pga2_enable = 1;
        p_channel->pga2_gain = 0;
    }
    pga_offset = (uint8_t)(5.0*pga2_offset/pga3_gain);
    p_channel->pga2_offset = ((~(pga_offset & 0x07) + 1) & 0x0F);  // 2's compliment to sign+magnitude format
    pga_offset = (uint8_t)(pga3_offset/12);
    p_channel->pga3_offset = ((~(pga_offset & 0x3F) + 1) & 0x7F);  // 2's compliment to sign+magnitude format
    // return the system offset coef for this channel
    return offset - (pga2_offset * pga3_gain + pga3_offset);
}

void sx8724_i2c_isr( void )
{
    if( PIR3bits.SSP2IF )
    {
        if( SSP2CON2bits.ACKSTAT )
        {
            ack_error();
        }
        else
        {
            switch( s_state_i2c )
            {
                case state_i2c_device_address:
                {
                    SSP2BUF = s_i2c_addr | SX8724_I2C_WRITE;
                    s_state_i2c = state_i2c_register_address;
                    s_i2c_addr = SX8724_I2C_ADDR;
                    break;
                }
                case state_i2c_register_address:
                {
                    SSP2BUF = read_byte();
                    uint8_t count = read_byte();
                    s_count = count & TRANSACTION_COUNT_MASK;
                    if( s_count )
                    {
                        if( (count & TRANSACTION_COUNT_MASK)  == TRANSACTION_WRITE )
                        {
                            s_state_i2c = state_i2c_register_value_write;
                        }
                        else
                        {
                            s_state_i2c = state_i2c_restart;
                            SSP2CON2bits.SEN = 1;   // start
                        }
                    }
                    else
                    {
                        s_state_i2c = state_i2c_stop;
                    }
                    break;
                }
                case state_i2c_restart:
                {
                    SSP2BUF = SX8724_I2C_ADDR | SX8724_I2C_READ;
                    s_state_i2c = state_i2c_register_value_read;
                    s_rx_read_ndx = 0;
                    break;
                }
                case state_i2c_register_value_read:
                {
                    s_rx_buffer[s_rx_read_ndx++] = SSP2BUF;
                    xfer_data();
                    break;
                    
                }
                case state_i2c_register_value_write:
                {
                    SSP2BUF = read_byte();
                    xfer_data();
                    break;
                }
                case state_i2c_stop:
                {
                    SSP2CON2bits.PEN = 1;
                    s_state_i2c = state_i2c_start;
                    if( s_tx_available < ARRAY_SIZE(s_tx_buffer))
                    {
                        s_state_i2c = state_i2c_start;
                    }
                    else
                    {
                        s_state_i2c = state_i2c_idle;
                    }
                    break;
                }
                case state_i2c_start:
                {
                    SSP2CON2bits.SEN = 1;
                    s_state_i2c = state_i2c_device_address;
                    break;
                }
            }
        }
        PIR3bits.SSP2IF = 0;
    }
}