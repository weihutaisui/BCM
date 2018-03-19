/*
<:copyright-BRCM:2012:DUAL/GPL:standard

   Copyright (c) 2012 Broadcom 
   All Rights Reserved

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>

*/

/** Includes. **/
#include "lib_types.h"
#include "lib_malloc.h"
#include "lib_string.h"
#include "lib_printf.h"

#include "addrspace.h"
#include "cfe_iocb.h"
#include "cfe_ioctl.h"
#include "cfe_device.h"
#include "cfe_devfuncs.h"
#include "cfe_timer.h"

#include "bcm_map.h"
#include "bcm_ethsw.h"

/** Prototypes. **/
static void bcm63xx_ether_probe( cfe_driver_t * drv,    unsigned long probe_a,
                                 unsigned long probe_b, void * probe_ptr );
static int bcm63xx_ether_open(cfe_devctx_t *ctx);
static int bcm63xx_ether_read(cfe_devctx_t *ctx,iocb_buffer_t *buffer);
static int bcm63xx_ether_inpstat(cfe_devctx_t *ctx,iocb_inpstat_t *inpstat);
static int bcm63xx_ether_write(cfe_devctx_t *ctx,iocb_buffer_t *buffer);
static int bcm63xx_ether_ioctl(cfe_devctx_t *ctx,iocb_buffer_t *buffer);
static int bcm63xx_ether_close(cfe_devctx_t *ctx);

extern void _cfe_flushcache(int, uint8_t *, uint8_t *);
#define FLUSH_RANGE(s,l) _cfe_flushcache(CFE_CACHE_FLUSH_RANGE,((uint8_t *) (s)),((uint8_t *) (s))+(l))

static uint32_t rx_cons_idx;
static uint32_t rx_read_idx;


/* Number of RX DMA buffers */
#define NUM_RX_DMA_BUFFERS              32

/* Number of RX DMA buffers */
#define NUM_TX_DMA_BUFFERS              1

/* DMA buffer size for SP_RDMA_BSRS_REG register (2048 bytes) */
#define NET_DMA_SHIFT                   11


/* RX/TX DMA buffer size */
#define NET_DMA_BUFSIZE                 2048 


#define CACHE_ALIGN			64
static unsigned char			*dma_buffer=NULL;


/* Base address of DMA buffers */
#define NET_DMA_BASE                    ((uint64_t)dma_buffer)

/* Address of TX DMA buffer */
#define TX_DMA_BUFADDR                  (NET_DMA_BASE)

/* Address of first RX DMA buffer */
#define RX_DMA_BUFADDR                  (NET_DMA_BASE + (NET_DMA_BUFSIZE*NUM_TX_DMA_BUFFERS))


/* Size of RSB header prepended to RX packets */
#define RSB_SIZE                                8

/* Minimal Ethernet packet length */
#define ENET_ZLEN                               60


#define MAX_PKT_LEN                             (NET_DMA_BUFSIZE - RSB_SIZE)





/** Variables. **/
const static cfe_devdisp_t bcm63xx_ether_dispatch = {
    bcm63xx_ether_open,
    bcm63xx_ether_read,
    bcm63xx_ether_inpstat,
    bcm63xx_ether_write,
    bcm63xx_ether_ioctl,
    bcm63xx_ether_close,
    NULL,
    NULL
};

const cfe_driver_t bcm63xx_enet = {
    "BCM63xx Ethernet",
    "eth",
    CFE_DEV_NETWORK,
    &bcm63xx_ether_dispatch,
    bcm63xx_ether_probe
};

/* Enable System Port RX DMA */
int sp_enable_rdma(void)
{
        volatile uint32_t reg;
        int timeout = 1000;

        // Enable RX DMA and Ring Buffer mode
        SYSPORT_RDMA->SYSTEMPORT_RDMA_CONTROL=0x00030007;

        // Wait for RX DMA to become ready
        do {
                reg = SYSPORT_RDMA->SYSTEMPORT_RDMA_STATUS;
                if (!(reg & 0x3)) {
                        return 0;
                }
                cfe_usleep(10);

        } while (timeout--);

        return -1;
}

/* Enable System Port TX DMA */
int sp_enable_tdma(void)
{
        volatile uint32_t reg;
        int timeout = 1000;

        // Disable ACB and enable TDMA
        SYSPORT_TDMA->SYSTEMPORT_TDMA_CONTROL=
                        1 << 27 | 4 << 22 | 3 << 20 | 1 << 18 | 1 << 17 | 1 << 4 | 3 << 2 | 1 << 0;

        // Wait for TX DMA to become ready
        do {
                reg = SYSPORT_TDMA->SYSTEMPORT_TDMA_STATUS;
                if (!(reg & 0x3)) {
                        return 0;
                }
                cfe_usleep(10);

        } while (timeout--);

        return -1;
}


static void reset_sp(void)
{
    SYSPORT_UMAC->SYSTEMPORT_UMAC_CMD=0;
    cfe_usleep(1000);
    SYSPORT_RDMA->SYSTEMPORT_RDMA_CONTROL=0;
    cfe_usleep(1000);
    //RXCHK_CONTROL=0;
    SYSPORT_TDMA->SYSTEMPORT_TDMA_CONTROL=0;
    cfe_usleep(1000);
    SYSPORT_TPC->SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL=1;
    SYSPORT_TPC->SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL=1;
    cfe_usleep(1000);
    SYSPORT_TPC->SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL=0;
    SYSPORT_TPC->SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL=0;
    cfe_usleep(1000);
    SYSPORT_TDMA->SYSTEMPORT_TDMA_STATUS = 0x2; // initialize the Link List RAM. 
    cfe_usleep(10);
}

static int sp_init(void)
{

   dma_buffer=KMALLOC(NET_DMA_BUFSIZE *(NUM_TX_DMA_BUFFERS+NUM_RX_DMA_BUFFERS), CACHE_ALIGN);

   if(dma_buffer == NULL)
   {
        printf("PANIC: dma buffer allocation failed\n");
        return -1;
   }
   memset(dma_buffer, 0x00, NET_DMA_BUFSIZE *(NUM_TX_DMA_BUFFERS+NUM_RX_DMA_BUFFERS));
   FLUSH_RANGE(dma_buffer, NET_DMA_BUFSIZE *(NUM_TX_DMA_BUFFERS+NUM_RX_DMA_BUFFERS));

   reset_sp();

   rx_cons_idx = 0;
   rx_read_idx = 0;

   // SYSTEMPORT_RBUF_RBUF_CONTROL 4b_align=0, rsb_en=1
   SYSPORT_RBUF->SYSTEMPORT_RBUF_RBUF_CONTROL=0x00e0c081;

   // SYSTEMPORT_RBUF_RBUF_PACKET_READY_THRESHOLD: pkt_rdy_threshold=0x7f
   SYSPORT_RBUF->SYSTEMPORT_RBUF_RBUF_PACKET_READY_THRESHOLD=0x7f;

   // Initialize RX DMA consumer index
   SYSPORT_RDMA->SYSTEMPORT_RDMA_CONSUMER_INDEX=0x0;

   // Set number of RX DMA buffers and RX DMA buffer size
   SYSPORT_RDMA->SYSTEMPORT_RDMA_BSRS=(NUM_RX_DMA_BUFFERS << 16) | (NET_DMA_SHIFT << 0);

   // Set start address of RX DMA buffers
   SYSPORT_RDMA->SYSTEMPORT_RDMA_START_ADDRESS_LOW=K1_TO_PHYS(RX_DMA_BUFADDR);

   // Enable TDMA q0 ("ring0")
   SYSPORT_TDMA->SYSTEMPORT_TDMA_DESC[0].SYSTEMPORT_TDMA_DESC_RING_XX_HEAD_TAIL_PTR=1 << 25;

   // SYSTEMPORT_TDMA_CONTROL
   SYSPORT_TDMA->SYSTEMPORT_TDMA_CONTROL=0x0136001e;

   // SYSTEMPORT_TDMA_DESC_RING_00_MAX_HYST_THRESHOLD:
   // locram_max_threshold=1 (max. q0 size)
   SYSPORT_TDMA->SYSTEMPORT_TDMA_DESC[0].SYSTEMPORT_TDMA_DESC_RING_XX_MAX_HYST_THRESHOLD=0x1;

   // SYSTEMPORT_TDMA_TIER_1_ARBITER_0_QUEUE_ENABLE: arb_enable=1
   SYSPORT_TDMA->SYSTEMPORT_TDMA_TIER_1_ARBITER_0_QUEUE_ENABLE=0x1;

   // Enable RX DMA
   if (sp_enable_rdma() < 0) {
           // Print SPIF for System Port Init Failed
           
           return -1;
   }

   // Enable TX DMA
   if (sp_enable_tdma() < 0) {
           // Print SPIF for System Port Init Failed
           
           return -1;
   }

   // SYSTEMPORT_UMAC_CMD:  tx_pause_ignore=1, eth_speed='b10 (1G),
   // rx_ena=1, tx_ena=1, others=default
   SYSPORT_UMAC->SYSTEMPORT_UMAC_CMD=0x108000db;

   // Give SP some time to initialize
   cfe_usleep(100);

    bcm_ethsw_open();
   

   return 0;
}



/** Functions. **/
static void bcm63xx_ether_probe( cfe_driver_t * drv,    unsigned long probe_a,
                                unsigned long probe_b, void * probe_ptr )
{
    char descr[100];


    sp_init();

    sprintf(descr, "%s eth %d", drv->drv_description, probe_a);
    cfe_attach(drv, NULL, NULL, descr);


}

static int bcm63xx_ether_open(cfe_devctx_t *ctx)
{
return 0;
}

int sp_poll(uint32_t timeout_ms)
{
        int i = 0;
        uint32_t p_index, c_index;

        // SYSTEMPORT_RDMA_PRODUCER_INDEX...
        p_index = SYSPORT_RDMA->SYSTEMPORT_RDMA_PRODUCER_INDEX & 0xffff;
        c_index = SYSPORT_RDMA->SYSTEMPORT_RDMA_CONSUMER_INDEX & 0xffff;

        while (p_index == c_index) {

                if (i++ > timeout_ms)
                        return -1;

                cfe_usleep(1000);

                p_index = SYSPORT_RDMA->SYSTEMPORT_RDMA_PRODUCER_INDEX & 0xffff;
                c_index = SYSPORT_RDMA->SYSTEMPORT_RDMA_CONSUMER_INDEX & 0xffff;
        }

        return 0;
}


static int bcm63xx_ether_read(cfe_devctx_t *ctx,iocb_buffer_t *buffer)
{

        int rc = 0;
        const uint8_t *bufaddr;

        if(RX_DMA_BUFADDR == NULL) {
                return -1;
        }

	if(sp_poll(1) == 0)
	{

		// Get RX DMA buffer address
		bufaddr = (uint8_t *)cache_to_uncache(RX_DMA_BUFADDR) + rx_read_idx * NET_DMA_BUFSIZE;

		// Get packet length from RSB.
		buffer->buf_retlen = (*((uint32_t *)(bufaddr + 4)) & 0xfffc0000) >> 18;
		buffer->buf_retlen -= RSB_SIZE;

		if (buffer->buf_retlen < ENET_ZLEN || buffer->buf_retlen > MAX_PKT_LEN) {
			rc = -1;
			buffer->buf_retlen = 0;
			goto out;
		}

		memcpy(buffer->buf_ptr, bufaddr + RSB_SIZE, buffer->buf_retlen);

	out:
		// Advance RX read index
		rx_read_idx++;
		rx_read_idx &= (NUM_RX_DMA_BUFFERS - 1);

		// Advance RX consumer index in HW
		rx_cons_idx++;
		rx_cons_idx &= 0xffff;
		SYSPORT_RDMA->SYSTEMPORT_RDMA_CONSUMER_INDEX=rx_cons_idx;
	}
	else
		rc=-1;

        return rc;


}

static int bcm63xx_ether_inpstat(cfe_devctx_t *ctx,iocb_inpstat_t *inpstat)
{
return -1; 
}

static int bcm63xx_ether_write(cfe_devctx_t *ctx,iocb_buffer_t *buffer)
{

        int timeout = 1000;
        uint32_t p_index, c_index;
        uint32_t txdesc_hi, txdesc_lo;

        if (buffer->buf_length > MAX_PKT_LEN) {
                return -1;
        }

        if(TX_DMA_BUFADDR == NULL) {
                return -1;
        }

        memcpy((void *)cache_to_uncache(TX_DMA_BUFADDR), buffer->buf_ptr, buffer->buf_length);

        // set up txdesc record: eop=1, sop=1, append_crc=1
        txdesc_hi = (buffer->buf_length << 18) | (3 << 16) | (1 << 11);
        txdesc_lo = (uint32_t)K1_TO_PHYS(TX_DMA_BUFADDR);


	__asm__ __volatile__ ("dsb    sy");

        // write tx desc to hw
        // SYSTEMPORT_TDMA_DESCRIPTOR_00_WRITE_PORT_LO
        SYSPORT_TDMA->SYSTEMPORT_TDMA_DESCRIPTOR_WRITE_PORT[0].SYSTEMPORT_TDMA_DESCRIPTOR_XX_WRITE_PORT_LO=txdesc_lo;

        // SYSTEMPORT_TDMA_DESCRIPTOR_00_WRITE_PORT_HI
        SYSPORT_TDMA->SYSTEMPORT_TDMA_DESCRIPTOR_WRITE_PORT[0].SYSTEMPORT_TDMA_DESCRIPTOR_XX_WRITE_PORT_HI=txdesc_hi;

        // wait until the packet is fully DMA'ed and stored into TBUF
        do {
                // SYSTEMPORT_TDMA_DESC_RING_00_PRODUCER_CONSUMER_INDEX
                p_index = SYSPORT_TDMA->SYSTEMPORT_TDMA_DESC[0].SYSTEMPORT_TDMA_DESC_RING_XX_PRODUCER_CONSUMER_INDEX;
                c_index = (p_index >> 16);
                p_index &= 0xffff;
                c_index &= 0xffff;

        } while ((p_index != c_index) & timeout--);

        if (timeout == 0) {
                return -1;
        }

        return 0;

}

static int bcm63xx_ether_ioctl(cfe_devctx_t *ctx,iocb_buffer_t *buffer)
{
	//TODO:
return -1; 
}

static int bcm63xx_ether_close(cfe_devctx_t *ctx)
{
return 0; 
}



