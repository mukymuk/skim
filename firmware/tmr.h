void tmr_init(void);
uint16_t tmr_getms( void );
void tmr_isr( void );

extern uint16_t s_ms;

#define  tmr_getms_isr() (s_ms)

