/***************************************************************************
 *  <:copyright-BRCM:2016:DUAL/GPL:standard
 *  
 *     Copyright (c) 2016 Broadcom 
 *     All Rights Reserved
 *  
 *  Unless you and Broadcom execute a separate written software license
 *  agreement governing use of this software, this software is licensed
 *  to you under the terms of the GNU General Public License version 2
 *  (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 *  with the following added to such license:
 *  
 *     As a special exception, the copyright holders of this software give
 *     you permission to link this software with independent modules, and
 *     to copy and distribute the resulting executable under terms of your
 *     choice, provided that you also meet, for each linked independent
 *     module, the terms and conditions of the license of that module.
 *     An independent module is a module which is not derived from this
 *     software.  The special exception does not apply to any modifications
 *     of the software.
 *  
 *  Not withstanding the above, under no circumstances may you combine
 *  this software in any way with any other Broadcom software provided
 *  under a license other than the GPL, without Broadcom's express prior
 *  written consent.
 *  
 * :>
 ***************************************************************************/

#if defined(CONFIG_SERIAL_BCM_VIRTCONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/clk.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/sysrq.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/timer.h>


#include <tzioc_drv.h>
#include <vuart.h>
#include <tzioc_common_ring.h>
#include <tzioc_sys_msg.h>
#if IOREMAP_SHARED_MEM
#else
#include <linux/brcmstb/memory_api.h>
#endif
#if defined(CONFIG_BRCM_SB_BUILD)

#include <ipi.h>

#define IS_SANDBOX_MODE()               1
#define NR_VUARTS                       2
#define NR_VUART_FIFO                   2
#define VUART_MEMBASE                   0x0FC40000

#if defined(CONFIG_BRCM_SB_CORE1)
#define IS_CPU0()                       0
#define VUART_RX_FIFO_ID                1
#define VUART_TX_FIFO_ID                0
/* Core 1 IRQ pair */
#else /* CONFIG_BRCM_SB_CORE0 */
#define IS_CPU0()                       1
#define VUART_RX_FIFO_ID                0
#define VUART_TX_FIFO_ID                1
#endif

#else /* defined(CONFIG_BRCM_SB_BUILD) */

#include <bchp_common.h>
#include <bchp_hif_top_ctrl.h>

/* loopback mode */
#define IS_SANDBOX_MODE()               0
#define IS_CPU0()                       1
#define VUART_MEMBASE                   (BCHP_HIF_TOP_CTRL_SCRATCH + \
                                         BCHP_PHYSICAL_OFFSET)
#define VUART_RX_FIFO_ID                1
#define VUART_TX_FIFO_ID                0

#define PORT_BCM_VUART                  200
#define NR_VUARTS                       1
#define NR_VUART_FIFO                   2

#define brcm_ipi_register(a, b, c)      do { } while (0)
#define brcm_ipi_unregister(a)          do { } while (0)
#define brcm_ipi_enable_irq(a)          do { } while (0)
#define brcm_ipi_disable_irq(a)         do { } while (0)
#define bcm_vuart_ipi_rx_callback(a)    do { } while (0)

#define brcm_ipi_send(idx) {                            \
    tasklet_schedule(&plat_vuarts[idx].recv_tasklet);   \
}

#endif /* defined(CONFIG_BRCM_SB_BUILD) */

#define BCM_VUART_DRVNAME               "bcm_vuart"
#define BCM_VUART_TTYNAME               "ttyVX"
#define BCM_VUART_MINOR_START           68
#define PORT_BCM_VUART                  200

#define VUART_IPI_IRQ_START             4
#define VUART_RX_IPI_IRQ(__vport)       (__vport->irq + VUART_RX_FIFO_ID)
#define VUART_TX_IPI_IRQ(__vport)       (__vport->irq + VUART_TX_FIFO_ID)

#define VUART_CLK_FREQ                  16000000
#define VUART_FIFO_SIZE                 128
#define RSET_UART_SIZE                  (NR_VUART_FIFO *                 \
                                         (sizeof(struct vuart_shmfifo) + \
                                          UART_XMIT_SIZE))

#define TTY_MEM_LIMIT                   0x10000



/* vuart mem structure */
struct vuart_mem {
    uint32_t phyTxMem;
    uint32_t phyTxSize;
    uint32_t phyRxMem;
    uint32_t phyRxSize;
    unsigned char *pvmemTx;
    unsigned char *pvmemRx;
};

static struct vuart_mem vuart_mem;
static bool vuart_enabled = false;

/*
 * virtual port shared memory fifo rx/tx buffer
 */
struct vuart_shmfifo {
    struct tzioc_ring_buf   *phdr;
    struct circ_buf         cbuf;
};

/*
 * This is for stored  backpointer in the registred uart port
 */
struct platform_vuart {
    struct uart_port        vuport;
    int                     idx;
    struct tasklet_struct   xmit_tasklet;
    struct tasklet_struct   recv_tasklet;
    struct vuart_shmfifo    *shmfifop[NR_VUART_FIFO];
};

static struct platform_vuart plat_vuarts[NR_VUARTS];
static struct timer_list vuart_timer[NR_VUARTS];

/*
 * return port type in string format
 */
static const char *bcm_vuart_type(struct uart_port *port)
{
    return (port->type == PORT_BCM_VUART) ? BCM_VUART_DRVNAME : NULL;
}

/*
 * serial core request to disable tx ASAP (used for flow control)
 */
static void bcm_vuart_stop_tx(struct uart_port *port)
{
}

/*
 * serial core request to stop rx, called before port shutdown
 */
static void bcm_vuart_stop_rx(struct uart_port *port)
{
}

/*
 * ring buffer offset wrap arround condition
 */
static uint32_t offset_wrap(
    struct tzioc_ring_buf *pRing,
    uint32_t ulOffset,
    uint32_t ulInc)
{
    ulOffset += ulInc;
    if (ulOffset >= pRing->ulBuffOffset + pRing->ulBuffSize) {
        ulOffset -= pRing->ulBuffSize;
    }
    return ulOffset;
}

/*
 * read all chars from fifo and send them to local/remote core
 */
static void bcm_vuart_do_rx(struct uart_port *port)
{
#ifdef VUART_USE_TTY
    struct tty_port *tty = 0;
#endif
    struct tzioc_ring_buf *hdr = 0;
    unsigned int max_count = 0, i = 0;
    struct platform_vuart *vuart = (struct platform_vuart *)port->private_data;
    unsigned char *rdPtr = NULL;

    hdr = (struct tzioc_ring_buf *)vuart->shmfifop[VUART_RX_FIFO_ID]->phdr;
    if (NULL == hdr) {
        LOGE("Error Uninitialized FIFO buffer");
        return;
    }

    /* calculate number of bytes of data available */
    if (hdr->ulWrOffset >= hdr->ulRdOffset)
        max_count = hdr->ulWrOffset - hdr->ulRdOffset;
    else
        max_count = hdr->ulBuffSize - (hdr->ulRdOffset - hdr->ulWrOffset);

    if (0 == max_count)
        return;

    rdPtr = hdr->ulRdOffset - vuart_mem.phyRxMem + vuart_mem.pvmemRx;

#ifdef VUART_USE_TTY
    tty = &(port->state->port);

    max_count = max_count > TTY_MEM_LIMIT ? TTY_MEM_LIMIT : max_count;
    /*
     * if there is no room in the tty buffer we will come back again
     */
    if (tty_buffer_request_room((struct tty_port *)tty, max_count) < max_count) {
        tty_flip_buffer_push((struct tty_port *)tty);
        return;
    }

    i = max_count;

    while(i) {
        unsigned char c;
        char flag;

        c = *rdPtr;
        rdPtr++;
        if ((vuart_mem.pvmemRx + vuart_mem.phyRxSize) == rdPtr) {
            rdPtr = hdr->ulBuffOffset - vuart_mem.phyRxMem + vuart_mem.pvmemRx;
        }
        port->icount.rx++;
        flag = TTY_NORMAL;
        c &= 0xff;
        i--;

        if (uart_handle_sysrq_char(port, c))
            continue;

        tty_insert_flip_char((struct tty_port *)tty, c, flag);
    }

    tty_flip_buffer_push((struct tty_port *)tty);

#else
    i = max_count;
    while(i) {
        printk("%c", *rdPtr);
        rdPtr++;
        if ((vuart_mem.pvmemRx + vuart_mem.phyRxSize) == rdPtr) {
            rdPtr = hdr->ulBuffOffset - vuart_mem.phyRxMem + vuart_mem.pvmemRx;
        }
        i--;
    }
#endif

    hdr->ulRdOffset = offset_wrap(hdr, hdr->ulRdOffset, max_count);
}

/*
 * fill tx fifo with chars to send, stop when fifo is about to be full
 * or when all chars have been sent.
 */
static void bcm_vuart_do_tx(struct uart_port *port)
{
    struct tzioc_ring_buf *hdr = 0;
    struct circ_buf *xmit;
    unsigned int max_count = 0, i = 0;
    unsigned char c = 0;
    struct platform_vuart *vuart = (struct platform_vuart *)port->private_data;
    unsigned char *wrPtr = NULL;

    hdr = (struct tzioc_ring_buf *)vuart->shmfifop[VUART_TX_FIFO_ID]->phdr;

    if (NULL == hdr) {
        LOGE("Error Uninitialized FIFO buffer");
        return;
    }

    /* calculate free space available */
    /* minimum 1 byte of unwritten space has to be reserved */
    if (hdr->ulWrOffset >= hdr->ulRdOffset)
        max_count = hdr->ulBuffSize - (hdr->ulWrOffset - hdr->ulRdOffset) - 1;
    else
        max_count = (hdr->ulRdOffset - hdr->ulWrOffset) - 1;

    wrPtr = hdr->ulWrOffset - vuart_mem.phyTxMem + vuart_mem.pvmemTx;

    xmit = &port->state->xmit;
    if (uart_circ_empty(xmit))
        return;

    i = max_count;

    while (i--) {

        c = xmit->buf[xmit->tail];
        xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
        port->icount.tx++;

        *wrPtr = c;
        wrPtr++;

        if (uart_circ_empty(xmit))
            break;

        if((vuart_mem.pvmemTx + vuart_mem.phyTxSize) == wrPtr)
            wrPtr = hdr->ulBuffOffset - vuart_mem.phyTxMem + vuart_mem.pvmemTx;
    }

    hdr->ulWrOffset = offset_wrap(hdr, hdr->ulWrOffset, (max_count - i));
    return;
}

/*
 * Transmit tasklet
 */
static void bcm_vuart_tasklet_tx(unsigned long data)
{
    struct uart_port *port = (struct uart_port *) data;

    spin_lock(&port->lock);
    bcm_vuart_do_tx(port);
    spin_unlock(&port->lock);
    return;
}


/*
 * Receive tasklet
 */
static void bcm_vuart_tasklet_rx(unsigned long data)
{
    struct uart_port *port = (struct uart_port *) data;

    spin_lock(&port->lock);
    bcm_vuart_do_rx(port);
    spin_unlock(&port->lock);
    return;
}

#if defined(CONFIG_BRCM_SB_BUILD)
static int bcm_vuart_ipi_rx_callback(int idx, void *dev)
{
    struct uart_port *port = (struct uart_port *)dev;

    spin_lock(&port->lock);
    bcm_vuart_do_rx(port);
    spin_unlock(&port->lock);
    return 0;
}
#endif

/*
 * vuart_timer callback function
 */
static void scheduleReceiver(unsigned long index)
{
    unsigned long volatile nextTimeout = 0;
    brcm_ipi_send(index);
    nextTimeout = jiffies + 1 * HZ; /* 1 second from now*/
    mod_timer(&vuart_timer[index], nextTimeout);
}

/*
 * enable rx & tx operation on uart
 */
static void bcm_vuart_enable(struct uart_port *port)
{
    /*
     * register the IPI receive isr and enable the interrupt
     */
    brcm_ipi_register(VUART_RX_IPI_IRQ(port), bcm_vuart_ipi_rx_callback, port);
    brcm_ipi_enable_irq(VUART_RX_IPI_IRQ(port));
}

/*
 * disable rx & tx operation on uart
 */
static void bcm_vuart_disable(struct uart_port *port)
{
    /* unregister IPI  callback */
    brcm_ipi_disable_irq(VUART_RX_IPI_IRQ(port));
    brcm_ipi_unregister(VUART_RX_IPI_IRQ(port));
}

/*
 * clear all unread data in rx fifo and unsent data in tx fifo
 */
static void bcm_vuart_flush(struct uart_port *port)
{
    brcm_ipi_send(VUART_TX_IPI_IRQ(port));
}

/*
 * serial core request to initialize uart and start rx operation
 */
static int bcm_vuart_startup(struct uart_port *port)
{
    bcm_vuart_flush(port);
    bcm_vuart_enable(port);
    return 0;
}

/*
 * serial core request to flush & disable uart
 */
static void bcm_vuart_shutdown(struct uart_port *port)
{

    bcm_vuart_disable(port);
}

/*
 * serial core request to claim uart iomem
 */
static int bcm_vuart_request_port(struct uart_port *port)
{
    return 0;
}

/*
 * serial core request to release uart iomem
 */
static void bcm_vuart_release_port(struct uart_port *port)
{

}

/*
 * serial core request to do any port required autoconfiguration
 */
static void bcm_vuart_config_port(struct uart_port *port, int flags)
{
    if (flags & UART_CONFIG_TYPE) {
        if (bcm_vuart_request_port(port))
            return;
        port->type = PORT_BCM_VUART;
    }
}

/*
 * serial core request to (re)enable tx
 */
static void bcm_vuart_start_tx(struct uart_port *port)
{
    struct platform_vuart *vuart =
        (struct platform_vuart *)port->private_data;

    tasklet_schedule(&vuart->xmit_tasklet);
}

/*
 * serial core request to check if uart tx fifo is empty
 */
static unsigned int bcm_vuart_tx_empty(struct uart_port *port)
{
    return uart_circ_empty(&port->state->xmit) ? TIOCSER_TEMT : 0;
}

/*
 * serial core request to set RTS and DTR pin state and loopback mode
 */
static void bcm_vuart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{

}

/*
 * serial core request to return RI, CTS, DCD and DSR pin state
 */
static unsigned int bcm_vuart_get_mctrl(struct uart_port *port)
{
    return TIOCM_CAR | TIOCM_DSR | TIOCM_CTS;
}

/*
 * serial core request to enable modem status interrupt reporting
 */
static void bcm_vuart_enable_ms(struct uart_port *port)
{
}

/*
 * serial core request to start/stop emitting break char
 */
static void bcm_vuart_break_ctl(struct uart_port *port, int ctl)
{
}


/*
 * serail core request for termios settings
 */
static void bcm_vuart_set_termios(struct uart_port *port,
    struct ktermios *new, struct ktermios *old)
{
}

/*
 * serial core request to check that port information in serinfo are
 * suitable
 */
static int bcm_vuart_verify_port(struct uart_port *port,
    struct serial_struct *serinfo)
{
    return 0;
}

/* serial core callbacks */
static struct uart_ops bcm_vuart_ops = {
    .tx_empty       = bcm_vuart_tx_empty,
    .start_tx       = bcm_vuart_start_tx,
    .startup        = bcm_vuart_startup,
    .shutdown       = bcm_vuart_shutdown,
    .type           = bcm_vuart_type,
    .release_port   = bcm_vuart_release_port,
    .request_port   = bcm_vuart_request_port,
    .config_port    = bcm_vuart_config_port,
    .get_mctrl      = bcm_vuart_get_mctrl,
    .set_mctrl      = bcm_vuart_set_mctrl,
    .stop_tx        = bcm_vuart_stop_tx,
    .stop_rx        = bcm_vuart_stop_rx,
    .enable_ms      = bcm_vuart_enable_ms,
    .break_ctl      = bcm_vuart_break_ctl,
    .set_termios    = bcm_vuart_set_termios,
    .verify_port    = bcm_vuart_verify_port,
};


#if defined(CONFIG_SERIAL_BCM_VIRTCONSOLE)
static inline void wait_for_xmitr(struct circ_buf *shmxmit, int con_idx)
{
    unsigned int tmout;
    int ipi_sent = 0;
    /* Wait up to 10ms for the character(s) to be sent */
    tmout = 10000;

    while (--tmout) {
        /* we need at least 2 free char space */
        if (uart_circ_chars_free(shmxmit) > 1)
            break;
        udelay(1);
        brcm_ipi_send(VUART_IPI_IRQ_START + con_idx + VUART_TX_FIFO_ID);
        ipi_sent = 1;
    }

    if (!ipi_sent)
        brcm_ipi_send(VUART_IPI_IRQ_START + con_idx + VUART_TX_FIFO_ID);
}

/*
 * helper function to put the characters into the xmit fifo
 */
static void  bcm_vconsole_putchars(int con_idx, const char *c,
    struct vuart_shmfifo *shmfifop, unsigned int count)
{
    struct circ_buf *shmxmit;
    int i;

    /*
     * Write to the shared tx circular buffer
     */
    shmxmit = &shmfifop->cbuf;

    for (i = 0; i < count; i++, c++) {
        wait_for_xmitr(shmxmit, con_idx);
        /*
         * Put data into virtual uart shared memory fifo
         */
        shmxmit->buf[shmxmit->head] = *c;
        shmxmit->head = (shmxmit->head + 1) & (UART_XMIT_SIZE - 1);
        mb();

        if (*c == '\n') {
            shmxmit->buf[shmxmit->head] = '\r';
            shmxmit->head = (shmxmit->head + 1) &
                (UART_XMIT_SIZE - 1);
            mb();
        }
    }
}

/*
 * console core request to output given string
 * we assume that there is room in the buffer
 * at this point
 */
static struct vuart_shmfifo *early_shmfifop __initdata;
static void __init bcm_early_vconsole_write(struct console *con, const char *s,
    unsigned int count)
{
    bcm_vconsole_putchars(con->index, s, early_shmfifop, count);
}

static struct console bcm_early_vconsole __initdata = {
    .name	= BCM_VUART_TTYNAME,
    .write	= bcm_early_vconsole_write,
    .flags	= CON_PRINTBUFFER | CON_BOOT,
    .index	= -1
};

static int bcm_early_vconsole_initialized __initdata;

static int __init bcm_setup_early_vconsole(char *cmdline)
{
    char *s;
    unsigned long mapbase;

    if (bcm_early_vconsole_initialized)
        return 0;
    bcm_early_vconsole_initialized = 1;

    bcm_early_vconsole.index = 0;

    s = strstr(cmdline, "console=ttyVX");
    if (s) {
        /*
         *  for a virtaul console:
         *      console=ttyVX0,115200n8
         */
        s = strchr(cmdline, 'X') + 1;
        if ((*s >= '0') && (*s <= '9'))
            bcm_early_vconsole.index = *s - '0';
    }

    /*
     * tx buffer init for the  assinged console shared buffer
     */
    mapbase = VUART_MEMBASE + (bcm_early_vconsole.index * RSET_UART_SIZE);
    mapbase += (sizeof(struct vuart_shmfifo) + UART_XMIT_SIZE)
        * VUART_TX_FIFO_ID;

    early_shmfifop = (struct vuart_shmfifo *) phys_to_virt(mapbase);

    /*
     * clear the tx cicular buffer
     */
    uart_circ_clear(&early_shmfifop->cbuf);

    LOGI("Registering [%s%d]",
        bcm_early_vconsole.name,
        bcm_early_vconsole.index);

    register_console(&bcm_early_vconsole);

    return 0;
}

/* if ttyVX is enabled from commandline,  we use it as early console and do an
 * early registration
 */
early_param("earlycon", bcm_setup_early_vconsole);

/*
 * console core request to output given string
 */
static void bcm_vconsole_write(struct console *co, const char *s,
    unsigned int count)
{
    struct uart_port *port;
    unsigned long flags;
    int locked;
    struct circ_buf *shmxmit;
    struct vuart_shmfifo *shmfifop;

    port = &plat_vuarts[co->index].vuport;

    local_irq_save(flags);

    if (port->sysrq)
        locked = 0;
    else
        locked = spin_trylock(&port->lock);

    /*
     * Write to the shared tx circular buffer
     */
    shmfifop = plat_vuarts[co->index].shmfifop[VUART_TX_FIFO_ID];
    shmxmit = &shmfifop->cbuf;

    bcm_vconsole_putchars(co->index, s, shmfifop, count);

    wait_for_xmitr(shmxmit, co->index);

    if (locked)
        spin_unlock(&port->lock);

    local_irq_restore(flags);
}

/*
 * console core request to setup given console, find matching uart
 * port and setup it.
 */
static int bcm_vconsole_setup(struct console *co, char *options)
{
    struct uart_port *port;
    int baud = 9600;
    int bits = 8;
    int parity = 'n';
    int flow = 'n';

    if (co->index < 0 || co->index >= NR_VUARTS)
        return -EINVAL;
    port = &plat_vuarts[co->index].vuport;
    if (!port->membase)
        return -ENODEV;
    if (options)
        uart_parse_options(options, &baud, &parity, &bits, &flow);

    return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver bcm_vuart_driver;

static struct console bcm_vconsole = {
    .name           = BCM_VUART_TTYNAME,
    .write          = bcm_vconsole_write,
    .device         = uart_console_device,
    .setup          = bcm_vconsole_setup,
    .flags          = CON_PRINTBUFFER | CON_ANYTIME,
    .index          = -1,
    .data           = &bcm_vuart_driver,
};

static int __init bcm_vconsole_init(void)
{
    register_console(&bcm_vconsole);
    return 0;
}

console_initcall(bcm_vconsole_init);

#define BCM_VCONSOLE   (&bcm_vconsole)
#else
#define BCM_VCONSOLE   NULL
#endif

static struct uart_driver bcm_vuart_driver = {
    .owner          = THIS_MODULE,
    .driver_name    = BCM_VUART_DRVNAME,
    .dev_name       = BCM_VUART_TTYNAME,
    .major          = TTY_MAJOR,
    .minor          = BCM_VUART_MINOR_START,
    .nr             = NR_VUARTS,
    .cons           = BCM_VCONSOLE,
};

/*
 * platform driver probe/remove callback
 */
static int bcm_vuart_probe(struct platform_device *pdev)
{
    int ret, i, j;
    struct uart_port *port;
    struct platform_vuart *puart;
    struct vuart_shmfifo *shmfifop;
    unsigned long mapbase;

    if (pdev->id < 0 || pdev->id >= NR_VUARTS)
        return -EINVAL;

    for (j = 0; j < NR_VUARTS; j++) {
        puart = &plat_vuarts[j];
        port = &puart->vuport;
        memset(port, 0, sizeof(*port));
        port->iotype = UPIO_MEM;
        //port->irq = j + VUART_IPI_IRQ_START;
        port->uartclk = VUART_CLK_FREQ;
        port->fifosize = VUART_FIFO_SIZE;
        port->ops = &bcm_vuart_ops;
        port->flags = UPF_BOOT_AUTOCONF;
        port->type = PORT_BCM_VUART;
        port->dev = &pdev->dev;
        port->line = pdev->id + j;
        port->private_data = puart;
        puart->idx = j;
        /*
         * Initialize the tx/rx uart fifo structure
         * we use the shared address memory pointed by the
         * port->membase for calculating the fifo address
         * when using a sandboxed CPU core
         *
         *   +---vport0----+<-- vuart shared memory base
         *   + shmfifo0    +
         *   +-------------+
         *   +    buf0     +
         *   +-------------+
         *   + shmfifo1    +
         *   +-------------+
         *   +    buf1     +
         *   +-------------+
         *   +---vport1----+
         *   + shmfifo0    +
         *   +-------------+
         *   +    buf0     +
         *   +-------------+
         *   + shmfifo1    +
         *   +-------------+
         *   +    buf1     +
         *   +-------------+
         *   ...............
         *   ....vportn.....
         */
        if (IS_SANDBOX_MODE()) {
            mapbase = VUART_MEMBASE + (j * RSET_UART_SIZE);
            port->mapbase = mapbase;
        }
        else {
            /* loopback mode */
            port->mapbase = (uint32_t)vuart_mem.pvmemTx;
        }

        for (i = 0; i < NR_VUART_FIFO; i++) {
            /*
             * return the shared buffer pointer in the
             * platform_uart for the current port
             */
            if (IS_SANDBOX_MODE()) {
                shmfifop = (struct vuart_shmfifo *)phys_to_virt(port->mapbase);
                shmfifop += (i * (RSET_UART_SIZE / NR_VUART_FIFO));
                shmfifop->cbuf.buf =
                    (char *)shmfifop + sizeof(struct vuart_shmfifo);
            }
            else {
                shmfifop = (struct vuart_shmfifo *)
                    kzalloc(sizeof(struct vuart_shmfifo), GFP_KERNEL);
                shmfifop->phdr = (struct tzioc_ring_buf *)
                    ((i == VUART_RX_FIFO_ID) ?
                     (void *)vuart_mem.pvmemRx :
                     (void *)vuart_mem.pvmemTx);
                shmfifop->cbuf.buf =
                    (char *)shmfifop->phdr + sizeof(struct tzioc_ring_buf);
            }

            if (IS_CPU0() && i == VUART_TX_FIFO_ID)
                uart_circ_clear(&shmfifop->cbuf);

            puart->shmfifop[i] = shmfifop;
        }

        tasklet_init(&puart->xmit_tasklet, bcm_vuart_tasklet_tx,
            (unsigned long)puart);
        tasklet_init(&puart->recv_tasklet, bcm_vuart_tasklet_rx,
            (unsigned long)puart);

        /* Configure kernel timer */
        init_timer_on_stack(&vuart_timer[j]);
        vuart_timer[j].function = scheduleReceiver;
        vuart_timer[j].expires = jiffies + 1 * HZ;
        vuart_timer[j].data = j;
        add_timer(&vuart_timer[j]); /* 10 seconds from now */

        /*
         * add this port now
         */
        ret = uart_add_one_port(&bcm_vuart_driver, port);

        if (ret) {
            port->membase = 0;
            if (!IS_SANDBOX_MODE()) {
                for (i = 0; i < NR_VUART_FIFO; i++) {
                    kfree(puart->shmfifop[i]);
                    puart->shmfifop[i] = 0;
                }
            }
            return ret;
        }

#ifdef VUART_USE_TTY
        tty_buffer_set_limit((struct tty_port *)(&(port->state->port)),
            TTY_MEM_LIMIT);
#endif
    }

    /*
     * set platform virtual uart structure context
     * as platform driver data
     */
    platform_set_drvdata(pdev, plat_vuarts);

    return 0;
}

static int bcm_vuart_remove(struct platform_device *pdev)
{
    int    i;
    struct platform_vuart *vuarts;

    vuarts = platform_get_drvdata(pdev);

    /* for all virtaul  ports */
    for (i = 0; i < NR_VUARTS; i++) {
        tasklet_kill(&vuarts[i].xmit_tasklet);
        tasklet_kill(&vuarts[i].recv_tasklet);
        del_timer(&vuart_timer[i]);
        uart_remove_one_port(&bcm_vuart_driver, &vuarts[i].vuport);
        /* mark port as free */
        vuarts[i].vuport.membase = 0;
    }

    platform_set_drvdata(pdev, NULL);
    return 0;
}

/* Suspend power management event */
static int bcm_vuart_suspend(struct platform_device *pdev, pm_message_t state)
{
    int i;
    struct platform_vuart *vuarts;

    vuarts = platform_get_drvdata(pdev);
    for (i = 0; i < NR_VUARTS; i++)
        uart_suspend_port(&bcm_vuart_driver, &vuarts[i].vuport);
    return 0;
}

/* Resume after suspend */
static int bcm_vuart_resume(struct platform_device *pdev)
{
    int i;
    struct platform_vuart *vuarts;

    vuarts = platform_get_drvdata(pdev);

    /* for all virtaul  ports */
    for (i = 0; i < NR_VUARTS; i++)
        uart_resume_port(&bcm_vuart_driver, &vuarts[i].vuport);
    return 0;
}

/*
 * platform driver
 */
static struct platform_driver bcm_vuart_platform_driver = {
    .probe          = bcm_vuart_probe,
    .remove         = bcm_vuart_remove,
    .resume         = bcm_vuart_resume,
    .suspend        = bcm_vuart_suspend,
    .driver         = {
        .owner      = THIS_MODULE,
        .name       = BCM_VUART_DRVNAME,
    },
};

static void bcm_vuart_device_release(struct device *pdev)
{
}

struct platform_device bcm_vuart_platform_device = {
    .name           = BCM_VUART_DRVNAME,
    .id             = 0,
    .dev            = {
        .release    = bcm_vuart_device_release,
    },
};

int bcm_vuart_init(void)
{
    int ret = 0;

    if (vuart_enabled)
        return 0;

    LOGI("Astra vuart shared memory:\n\tRx buf 0x%x 0x%x\n\tTx buf 0x%x 0x%x",
        (unsigned int)vuart_mem.phyRxMem, (unsigned int)vuart_mem.phyRxSize,
        (unsigned int)vuart_mem.phyTxMem, (unsigned int)vuart_mem.phyTxSize);

    /* map shared memory */
#if IOREMAP_SHARED_MEM
    vuart_mem.pvmemRx = (unsigned char *)
        ioremap_nocache(vuart_mem.phyRxMem, vuart_mem.phyRxSize);
    vuart_mem.pvmemTx = (unsigned char *)
        ioremap_nocache(vuart_mem.phyTxMem, vuart_mem.phyTxSize);
#else
    vuart_mem.pvmemRx = (unsigned char *)
        _tzioc_paddr2vaddr(vuart_mem.phyRxMem);
    vuart_mem.pvmemTx = (unsigned char *)
        _tzioc_paddr2vaddr(vuart_mem.phyTxMem);
#endif

    if (!vuart_mem.pvmemRx || !vuart_mem.pvmemTx) {
        LOGE("Failed to map vuart memory");
        ret = -ENODEV;
        return ret;
    }

    ret = uart_register_driver(&bcm_vuart_driver);
    if (ret)
        return ret;

    ret = platform_device_register(&bcm_vuart_platform_device);
    if (ret) {
        uart_unregister_driver(&bcm_vuart_driver);
        return ret;
    }
    ret = platform_driver_register(&bcm_vuart_platform_driver);
    if (ret) {
        uart_unregister_driver(&bcm_vuart_driver);
        platform_device_unregister(&bcm_vuart_platform_device);
        return ret;
    }

    LOGI("Astra vuart is enabled");
    vuart_enabled = true;
    return 0;
}

void bcm_vuart_exit(void)
{
    if (!vuart_enabled)
        return;

    if (vuart_mem.pvmemRx) {
#if IOREMAP_SHARED_MEM
        iounmap(vuart_mem.pvmemRx);
#endif
        vuart_mem.pvmemRx = 0;
    }

    if (vuart_mem.pvmemTx) {
#if IOREMAP_SHARED_MEM
        iounmap(vuart_mem.pvmemTx);
#endif
        vuart_mem.pvmemTx = 0;
    }

    platform_device_unregister(&bcm_vuart_platform_device);
    platform_driver_unregister(&bcm_vuart_platform_driver);
    uart_unregister_driver(&bcm_vuart_driver);

    LOGI("Astra vuart is disabled");
    vuart_enabled = false;
}

int bcm_vuart_msg_proc(struct tzioc_msg_hdr *pHdr)
{
    int err = 0;

    struct sys_msg_vuart_on_cmd *pCmd;
    LOGD("Astra vuart msg processing");

    switch (pHdr->ucType) {

    case SYS_MSG_VUART_ON:
        if (pHdr->ucOrig != TZIOC_CLIENT_ID_SYS ||
            pHdr->ulLen == 0) {
            LOGE("Invalid vuart on cmd received");
            err = -EINVAL;
            break;
        }

        if (vuart_enabled) {
            LOGE("Ignoring vuart on cmd, vuart is already on");
            err = -EINVAL;
            break;
        }

        pCmd = (struct sys_msg_vuart_on_cmd *)TZIOC_MSG_PAYLOAD(pHdr);

        if (!(pCmd->rxFifoPaddr &&
              pCmd->rxFifoSize &&
              pCmd->txFifoPaddr &&
              pCmd->txFifoSize)) {
            err = -EINVAL;
            break;
        }

        vuart_mem.phyTxMem  = pCmd->rxFifoPaddr;
        vuart_mem.phyTxSize = pCmd->rxFifoSize;
        vuart_mem.phyRxMem  = pCmd->txFifoPaddr;
        vuart_mem.phyRxSize = pCmd->txFifoSize;

        if (bcm_vuart_init()) {
            LOGE("Failed to init vuart driver");
            err = -EINVAL;
            break;
        }
        break;

    case SYS_MSG_DOWN:
    case SYS_MSG_VUART_OFF:
        if (pHdr->ucOrig != TZIOC_CLIENT_ID_SYS ||
            pHdr->ulLen != 0) {
            LOGE("Invalid vuart off cmd received");
            err = -EINVAL;
            break;
        }

        bcm_vuart_exit();
        break;

    default:
        LOGW("Unknown vuart msg %d", pHdr->ucType);
        err = -ENOENT;
    }
    return err;
}
