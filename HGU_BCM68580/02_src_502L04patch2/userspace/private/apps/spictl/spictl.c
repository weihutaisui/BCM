/*
    <:copyright-BRCM:2011:proprietary:standard
    
       Copyright (c) 2011 Broadcom 
       All Rights Reserved
    
     This program is the proprietary software of Broadcom and/or its
     licensors, and may only be used, duplicated, modified or distributed pursuant
     to the terms and conditions of a separate, written license agreement executed
     between you and Broadcom (an "Authorized License").  Except as set forth in
     an Authorized License, Broadcom grants no license (express or implied), right
     to use, or waiver of any kind with respect to the Software, and Broadcom
     expressly reserves all rights in and to the Software and all intellectual
     property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
     NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
     BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
    
     Except as expressly set forth in the Authorized License,
    
     1. This program, including its structure, sequence and organization,
        constitutes the valuable trade secrets of Broadcom, and you shall use
        all reasonable efforts to protect the confidentiality thereof, and to
        use this information only in connection with your use of Broadcom
        integrated circuit products.
    
     2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
        AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
        WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
        RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
        ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
        FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
        COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
        TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
        PERFORMANCE OF THE SOFTWARE.
    
     3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
        ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
        INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
        WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
        IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
        OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
        SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
        SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
        LIMITED REMEDY.
    :>    

*/


/***************************************************************************
* File Name  : spictl.c
*
* Description: This file contains the test functions for testing a spi slave 
*              test device.(eg. 6368 spi slave device on a 63268).
*
***************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <board.h>
#include "spictl.h"


#define SPICTL_MODE_CMD_END         0
#define SPICTL_MODE_CMD_WRITE       1
#define SPICTL_MODE_CMD_READ        2
#define SPICTL_MODE_CMD_CLEAR_BITS  3
#define SPICTL_MODE_CMD_SET_BITS    4
#define SPICTL_MODE_CMD_DELAY       5

struct spictl_mode_t
{
    uint32_t cmd;
    uint32_t offset;
    uint32_t value;
};

struct spictl_mode_t mode_680x_rgmii[] =
{
    /* cmd              offset             value             */
   {SPICTL_MODE_CMD_SET_BITS,     0x10800004,   0x5         },
   {SPICTL_MODE_CMD_WRITE,        0x10800000,   0x2         },
   {SPICTL_MODE_CMD_CLEAR_BITS,   0x10800808,   0x400000    }, 
   {SPICTL_MODE_CMD_CLEAR_BITS,   0x10800024,   0x40        }, 
   {SPICTL_MODE_CMD_WRITE,        0x104040a8,   0x3         }, 
   {SPICTL_MODE_CMD_WRITE,        0x1080000c,   0x1         }, 
   {SPICTL_MODE_CMD_WRITE,        0x10800018,   0x0         }, 
   {SPICTL_MODE_CMD_END,          0,            0           } 
};

struct spictl_mode_t mode_680x_gphy[] =
{
    /* cmd                        offset        value   */
   {SPICTL_MODE_CMD_WRITE,        0x104040a8,   0x1         }, 
   {SPICTL_MODE_CMD_WRITE,        0x1040431c,   ~(1 << 26)  },
   {SPICTL_MODE_CMD_CLEAR_BITS,   0x10800004,   0xF         },
   {SPICTL_MODE_CMD_DELAY,        0x0,          10000       }, 
   {SPICTL_MODE_CMD_WRITE,        0x1040431c,   0xFFFFFFFF  },
   {SPICTL_MODE_CMD_WRITE,        0x10800000,   0x0         },
   {SPICTL_MODE_CMD_SET_BITS,     0x10800808,   0x400003    }, 
   {SPICTL_MODE_CMD_SET_BITS,     0x10800024,   0x40        }, 
   {SPICTL_MODE_CMD_WRITE,        0x1080000c,   0x0         }, 
   {SPICTL_MODE_CMD_WRITE,        0x10800018,   0x0         }, 
   {SPICTL_MODE_CMD_END,          0,            0           } 
};


int load(int fd, char *buf, int len, uint32_t offset, int dev)
{
    BOARD_IOCTL_PARMS ioctlParms;

    memset(&ioctlParms, 0, sizeof(ioctlParms));
    ioctlParms.result = dev;
    ioctlParms.offset = offset;
    ioctlParms.buf = buf;
    ioctlParms.strLen = len;
    if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_WRITE_BUF, &ioctlParms) < 0)
    {
        return -1;
    }
    return 0;
}

int spiread(int fd, uint32_t *result, int len, uint32_t offset, int dev)
{
    BOARD_IOCTL_PARMS ioctlParms;
    uint32_t buf=0;

    memset(&ioctlParms, 0, sizeof(ioctlParms));
    ioctlParms.offset = offset;
    ioctlParms.strLen = len;
    ioctlParms.result = dev;
    ioctlParms.buf = (char *)&buf;

    *result = 0;

    if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_READ, &ioctlParms) < 0)
    {
        return -1;
    } 

    if (ioctlParms.strLen == 1)
    {
        *result = *(ioctlParms.buf);
    }
    else if (ioctlParms.strLen == 2)
    {
        *result = *(uint16_t *)(ioctlParms.buf);
    }
    else if (ioctlParms.strLen == 4)
    {
        *result = *(uint32_t *)(ioctlParms.buf);
    }
    else
    {
        printf("Wrong unitsize. unitsize can be 1, 2 or 4 only\n");
        return -1;
    }
    return 0;
}

int spiwrite(int fd, uint32_t offset, uint32_t value, int size, int dev)
{    
    BOARD_IOCTL_PARMS ioctlParms;

    memset(&ioctlParms, 0, sizeof(ioctlParms));
    ioctlParms.offset = offset;
    ioctlParms.result = value;
    ioctlParms.strLen = size;
    ioctlParms.buf = (char *)dev; 
     
    if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_WRITE, &ioctlParms) < 0)
    {
        return -1;
    }  
    return 0;
}

int spi_clear_bits(int fd, uint32_t offset, uint32_t bits_to_clear, int size, int dev)
{
    int ret = 0;
    uint32_t data;
    
    ret = spiread(fd, &data, size, offset, dev);

    if (ret != 0)
        return(ret);

    data &= ~bits_to_clear;

    ret = spiwrite(fd, offset, data, size, dev);

    return(ret);
}

int spi_set_bits(int fd, uint32_t offset, uint32_t bits_to_set, int size, int dev)
{
    int ret = 0;
    uint32_t data;
    
    ret = spiread(fd, &data, size, offset, dev);

    if (ret != 0)
        return(ret);

    data |= bits_to_set;

    ret = spiwrite(fd, offset, data, size, dev);

    return(ret);
}

void dumpReg(int fd, uint32_t offset, uint32_t len, uint32_t devId)
{
    uint32_t result;

    if (spiread(fd, &result, len, offset, devId) < 0)
    {
         printf("spi slave device read failed.\n");
         return;
    }    

    printf("BMOCA_INIT: Read of %08x = %08x\n", offset, result);
}


void spictl_print_mode_help(void)
{
    printf("spictl mode <mode number> [device ID]\n");
    printf("        mode: 1 = 680x configured for RGMII\n");
    printf("              2 = 680x configured for GPHY\n");
    printf("   device ID: Optional, used to distinguish between\n");
    printf("              multiple MoCA interfaces (i.e. BHR)\n");
}

int spictl_handle_mode(int fd, int argc, char ** argv)
{
    uint32_t devId = 0;
    uint32_t mode;
    struct spictl_mode_t * p_mode_table;
    
    if ((argc < 3) || (argc > 4))
    {
        spictl_print_mode_help();
        return(0);
    }


    sscanf(argv[2], "%d", &mode);   
    if (argc == 4)
    {
       sscanf(argv[3], "%d", &devId);   
    }

    switch (mode)
    {
        case 1:
            p_mode_table = &mode_680x_rgmii[0];
            break;
        case 2:
            p_mode_table = &mode_680x_gphy[0];
            break;
        default:
            printf("ERROR! Unknown mode\n");
            spictl_print_mode_help();
            return(-1);
    }

    while (p_mode_table->cmd != SPICTL_MODE_CMD_END)
    {
        switch(p_mode_table->cmd)
        {
            case SPICTL_MODE_CMD_WRITE:
                printf("   write     0x%x to 0x%x\n", p_mode_table->offset, p_mode_table->value);
                spiwrite( fd, p_mode_table->offset, p_mode_table->value, sizeof(uint32_t), devId);
                break;
 
            case SPICTL_MODE_CMD_READ:
                spiread( fd, &p_mode_table->value, sizeof(uint32_t), p_mode_table->offset, devId);
                printf("   read      0x%x == 0x%x\n", p_mode_table->offset, p_mode_table->value);
                break;

            case SPICTL_MODE_CMD_CLEAR_BITS:
                printf("   clearbits 0x%x &= ~0x%x\n", p_mode_table->offset, p_mode_table->value);
                spi_clear_bits(fd, p_mode_table->offset, p_mode_table->value, sizeof(uint32_t), devId);
                break;

            case SPICTL_MODE_CMD_SET_BITS:
                printf("   setbits   0x%x |= 0x%x\n", p_mode_table->offset, p_mode_table->value);
                spi_set_bits(fd, p_mode_table->offset, p_mode_table->value, sizeof(uint32_t), devId);
                break;

            case SPICTL_MODE_CMD_DELAY:
                usleep(p_mode_table->value);
                break;

            default:
                printf("ERROR! Unknown command. Exiting!\n");
                return(-2);
        }

        p_mode_table++;
    }

    return(0);
    
}

int main(int argc, char **argv)
{
    BOARD_IOCTL_PARMS ioctlParms;
    uint32_t buf;
    
    int fd = 0;

    memset(&ioctlParms, 0, sizeof(ioctlParms));
    
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    fd = open("/dev/brcmboard", O_RDWR);
    if (fd == -1)
    {
        printf("Could not open /dev/brcmboard\n");
        return -1;
    }
    
    if ( !strcmp(argv[1], "init") )
    {
        if (argc == 3)
        {
            sscanf(argv[2], "%d", &ioctlParms.result);   
        }

        if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_INIT, &ioctlParms) < 0)
        {
            printf("spi slave device init failed.\n");
            return -1;
        }
    }
    else if ( !strcmp(argv[1], "read") )
    {
        uint32_t result;
        uint32_t offset;
        int len;
        int dev=0;

        if ((argc != 4) && (argc != 5))
        {
            printUsage();
            return -1;
        }
        
        sscanf(argv[2], "%x", &offset);
        sscanf(argv[3], "%d", &len);   

        if (argc == 5)
        {
            sscanf(argv[4], "%d", &dev);   
        }

        printf("received addr = 0x%x, unitsize=%d, device=%d\n", offset, len, dev);

        if (spiread(fd, &result, len, offset, dev) < 0)
        {
            printf("spi slave device read failed.\n");
            return -1;
        }   

        printf("Read returned 0x%x = 0x%x\n", offset, result);
    }
    else if ( !strcmp(argv[1], "write") )
    {    
        uint32_t offset;
        uint32_t value;
        int len;
        int dev=0;

        if ((argc != 5) && (argc != 6))
        {
            printUsage();
            return -1;
        }
        
        sscanf(argv[2], "%x", &offset);
        sscanf(argv[3], "%x", &value);    
        sscanf(argv[4], "%d", &len);    

        if (argc == 6)
        {
           sscanf(argv[5], "%d", &dev);   
        }

        if (spiwrite(fd, offset, value, len, dev) < 0)
        {
            printf("spi write failed.\n");
            return -1;
        }  
        else
        {         
            printf("Write successful\n");
        }      
    }
    else if ( !strcmp(argv[1], "setbits") )
    {    
         
         if ((argc != 5) && (argc != 6))
         {
             printUsage();
             return -1;
         }
         
         sscanf(argv[2], "%x", (unsigned int *)&ioctlParms.offset);
         sscanf(argv[3], "%x", (unsigned int *)&ioctlParms.result);    
         sscanf(argv[4], "%d", (int *)&ioctlParms.strLen);    

         if (argc == 6)
         {
            sscanf(argv[5], "%d", (int *)&ioctlParms.buf);   
         }

         
         if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_SET_BITS, &ioctlParms) < 0)
         {
             printf("spi slave device set bits failed.\n");
             return -1;
         }  
    }
    else if ( !strcmp(argv[1], "clearbits") )
    {    
         
         if ((argc != 5) && (argc != 6))
         {
             printUsage();
             return -1;
         }
         
         sscanf(argv[2], "%x", &ioctlParms.offset);
         sscanf(argv[3], "%x", &ioctlParms.result);    
         sscanf(argv[4], "%d", &ioctlParms.strLen);    

         if (argc == 6)
         {
            sscanf(argv[5], "%d", (int *)&ioctlParms.buf);   
         }

         
         if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_CLEAR_BITS, &ioctlParms) < 0)
         {
             printf("spi slave device clear bits failed.\n");
             return -1;
         }  
    }
    else if ((!strcmp(argv[1], "loadflash"))
          || (!strcmp(argv[1], "loadflashswap")))
    {
        FILE *fp;
        int i;
        int dev=0;
        static unsigned char buf[264];
        uint32_t result;
        uint32_t offset;
        int co;
        struct stat fstats;
        uint32_t nbytes = 0;
        int swap = !strcmp(argv[1], "loadflashswap");

        fp = fopen(argv[2], "rb");
        if (!fp)
        {
           printf("Unable to open file: %s\n", argv[2]);
           return -1;
        }
        stat(argv[2], &fstats);

        sscanf(argv[3], "%x", &offset);
        if (argc == 5)
            sscanf(argv[4], "%d", &dev);

        /* word at end of buf signals tx of next block */
        buf[256]=0xaa;
        buf[257]=0xaa;
        buf[258]=0xaa;
        buf[259]=0xaa;

        while(!feof(fp))
        {
            memset(buf, 0, 256); // clear data portion to 0
            for (i=0;i<64;i++)
            {
                char a,b,c,d;
                if (0 == fread(&buf[i*4], 4, 1, fp))
                   break;
                if (swap)
                {
                   a=buf[i*4];
                   b=buf[i*4+1];
                   c=buf[i*4+2];
                   d=buf[i*4+3];
                   buf[i*4]=d;
                   buf[i*4+1]=c;
                   buf[i*4+2]=b;
                   buf[i*4+3]=a;
                }
            } 
            if (i==0)
                break;
            nbytes += 256;

            printf("\r[");
            for(i=0;i<50;i++)
               if (nbytes*50/fstats.st_size > i)
                  printf("#");
               else
                  printf(".");
            printf("] %d%% : OFFSET=0x%08x", (int)(nbytes*100/fstats.st_size), (int)offset);

            buf[260] = (offset >>24) & 0xff;
            buf[261] = (offset >>16) & 0xff;
            buf[262] = (offset >>8) & 0xff;
            buf[263] = (offset) & 0xff;
            if (load(fd, (char *)buf, 264, 0x10090000, dev) < 0) //LEAP SHARED
            {
                printf("\nspi slave device write failed. (LEAP shared))\n");
                fclose(fp);
                return -1;
            }

            // trigger interrupt on leap
            if (spiwrite(fd, 0x100b0204, (1<<8), 4, dev) < 0) // leap_l2_status_set, HAB
            {
                printf("\nspi slave device write failed. (trigger interrupt)\n");
                fclose(fp);
                return -1;
            }

            // wait for block to be written 
            co=100000;
            while (co--)
            {
                if (spiread(fd, &result, 4, 0x10090100, dev) < 0)
                {
                    printf("\nspi slave device read failed. (HAB)\n");
                    fclose(fp);
                    return -1;
                }
                if (result != 0xAAAAAAAA)
                   break;
            }
            if (co<=0)
            {
                printf("\nTimeout wating for LEAP (is LEAP running?)\n");
                fclose(fp);
                return -1;
            } 
            offset += 256;

        }
        printf("\n");
    }
    else if (!strcmp(argv[1], "load"))
    {
        FILE *fp;
        char buf[256];
        int i;
        uint32_t offset;
        int dev;

        if ((argc != 4) && (argc != 5))
        {
            printUsage();
            return -1;
        }

        dev = 0;
        if (argc == 5)
            sscanf(argv[4], "%d", &dev);
                
        fp = fopen(argv[3], "rb");
        if (!fp)
        {
            fprintf(stderr, "Could not open file\n");
            return -1;
        }

        sscanf(argv[2], "%x", &offset);
        while (!feof(fp))
        {
            for (i=0;i<64;i++)
            {
                char a,b,c,d;
                if (0 == fread(&buf[i*4], 4, 1, fp))
                   break;
                a=buf[i*4];
                b=buf[i*4+1];
                c=buf[i*4+2];
                d=buf[i*4+3];
                buf[i*4]=d;
                buf[i*4+1]=c;
                buf[i*4+2]=b;
                buf[i*4+3]=a;
            }
            if (i == 0)
                break;
            i*=4;
        
            if (load(fd, buf, i, offset, dev) < 0)
            {
                printf("spi slave device write failed.\n");
                fclose(fp);
                return -1;
            }
            offset += i;
        }
    }
    else if (!strcmp(argv[1], "save"))
    {
        int len;
        FILE *fp;

        if ((argc != 5) && (argc != 6))
        {
            printUsage();
            return -1;
        }

         sscanf(argv[2], "%x", &ioctlParms.offset);
         sscanf(argv[3], "%d", &ioctlParms.strLen);   


        if (argc == 6)
            sscanf(argv[5], "%d", &ioctlParms.result);
                
        fp = fopen(argv[4], "wb");
        if (!fp)
        {
            fprintf(stderr, "Could not open file\n");
            return -1;
        }

        sscanf(argv[2], "%x", &ioctlParms.offset);
        sscanf(argv[3], "%d", &len);

        ioctlParms.buf = (char *)&buf;
        
        ioctlParms.strLen = 4;

        while (len > 0)
        {
            if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_READ, &ioctlParms) < 0)
            {
                printf("spi slave device read failed.\n");
                fclose(fp);
                return -1;
            }
            if (1 != fwrite(ioctlParms.buf, (len>=4?4:len), 1, fp))
            {
                printf("Unable to write to file\n");
                fclose(fp);
                return -1;
            }

	    ioctlParms.offset += 4;
            len -= 4;
        }
        fclose(fp);
    }
    else if (!strcmp(argv[1], "mode"))
    {
        spictl_handle_mode(fd, argc, argv);
    }
    else
    {
        printf("Command not supported\n");
    }

    close(fd);
    return 0;
    
}

void printUsage(void)
{
    printf("Usage: \n");
    printf("spictl init\n");        
    printf("spictl read <addr> <unitsize> [device ID]\n");
    printf("spictl write <addr> <data> <unitsize> [device ID]\n");
    printf("spictl setbits <addr> <data> <unitsize> [device ID]\n");
    printf("spictl clearbits <addr> <data> <unitsize> [device ID]\n");
    printf("spictl ddrinit <chip>\n");    
    printf("spictl loadflash <file> <offset> [device ID]\n");
    printf("spictl load <addr> <file> [device ID]\n");
    printf("spictl save <addr> <len> <file> [device ID]\n");
    printf("spictl mode <mode number> [device ID]\n");
}

void executeCommandSequence(int fd, BpCmdElem *prSeq)
{    
    BOARD_IOCTL_PARMS ioctlParms;
    uint32_t buf;

    memset(&ioctlParms, 0, sizeof(ioctlParms));
    
    while (prSeq[0].command != CMD_END)
    {
        switch(prSeq[0].command)
        {
            case CMD_READ:   ioctlParms.offset = prSeq[0].addr;
                             ioctlParms.strLen = 4;  
                             ioctlParms.buf = (char *)&buf;
                             if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_READ, &ioctlParms) < 0)
                             {
                                 printf("spi slave device read failed.\n");
                                 return;
                             }   
                             break;
    
            case CMD_WRITE:  
                             ioctlParms.offset = prSeq[0].addr;
                             ioctlParms.result = prSeq[0].value;
                             ioctlParms.strLen = 4;     
                             if (ioctl(fd, BOARD_IOCTL_SPI_SLAVE_WRITE, &ioctlParms) < 0)
                             {
                                 printf("spi slave device write failed.\n");
                                 return;
                             }                                
                             break;
    
            case CMD_WAIT:   usleep(prSeq[0].addr * 1000); /* Sleep in ms */
                             break;     
                             
            case CMD_END:    break;                                    
        }
    
        prSeq++;
    }
}

