#ifndef SENSOR_H_
#define SENSOR_H_

#define HCLK            ( (long)0x0 )
#define RING_OSC_HCLK   ( (long)0x1 )
#define PLL_REFCLK      ( (long)0x2 )
#define SYN_DDRO        ( (long)0x3 )
#define INV_DDRO        ( (long)0xC )
#define OXIDE_CLK       ( (long)0x16 )
#define PMOS_CLK        ( (long)0x17 )
#define NMOS_CLK        ( (long)0x18 )
#define TEMP_CLK        ( (long)0x19 )

// init_sensor: enable ddro_done interrupt
extern void init_sensor(void);
extern long sensor_read(long type, long id);

#endif
