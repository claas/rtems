/*
 *  DMV177 SONIC Configuration Information
 *
 *  References:
 *
 *  1) SVME/DMV-171 Single Board Computer Documentation Package, #805905,
 *     DY 4 Systems Inc., Kanata, Ontario, September, 1996.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/rtems_bsdnet.h>
#include <libchip/sonic.h>

void dmv177_sonic_write_register(
  void       *base,
  uint32_t    regno,
  uint32_t    value
)
{
  volatile uint32_t   *p = base;

#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Write 0x%04x to %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  p[regno] = value;
}

uint32_t   dmv177_sonic_read_register(
  void       *base,
  uint32_t    regno
)
{
  volatile uint32_t   *p = base;
  uint32_t             value;

  value = p[regno];
#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Read 0x%04x from %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  return value;
}

/*
 * Default sizes of transmit and receive descriptor areas
 */
#define RDA_COUNT     20 /* 20 */
#define TDA_COUNT     20 /* 10 */

/*
 * Default device configuration register values
 * Conservative, generic values.
 * DCR:
 *      No extended bus mode
 *      Unlatched bus retry
 *      Programmable outputs unused
 *      Asynchronous bus mode
 *      User definable pins unused
 *      No wait states (access time controlled by DTACK*)
 *      32-bit DMA
 *      Empty/Fill DMA mode
 *      Maximum Transmit/Receive FIFO
 * DC2:
 *      Extended programmable outputs unused
 *      Normal HOLD request
 *      Packet compress output unused
 *      No reject on CAM match
 */
#define SONIC_DCR \
   (DCR_DW32 | DCR_WAIT0 | DCR_PO0 | DCR_PO1  | DCR_RFT24 | DCR_TFT28)
#ifndef SONIC_DCR
# define SONIC_DCR (DCR_DW32 | DCR_TFT28)
#endif
#ifndef SONIC_DC2
# define SONIC_DC2 (0)
#endif

/*
 * Default location of device registers
 */
#ifndef SONIC_BASE_ADDRESS
# define SONIC_BASE_ADDRESS 0xF3000000
# warning "Using default SONIC_BASE_ADDRESS."
#endif

/*
 * Default interrupt vector
 */
#ifndef SONIC_VECTOR
# define SONIC_VECTOR 1
# warning "Using default SONIC_VECTOR."
#endif

sonic_configuration_t dmv177_sonic_configuration = {
  SONIC_BASE_ADDRESS,        /* base address */ 
  SONIC_VECTOR,              /* vector number */ 
  SONIC_DCR,                 /* DCR register value */
  SONIC_DC2,                 /* DC2 register value */
  TDA_COUNT,                 /* number of transmit descriptors */
  RDA_COUNT,                 /* number of receive descriptors */
  dmv177_sonic_write_register,
  dmv177_sonic_read_register
};

int rtems_dmv177_sonic_driver_attach(struct rtems_bsdnet_ifconfig *config)
{
  return rtems_sonic_driver_attach( config, &dmv177_sonic_configuration );
  
}
