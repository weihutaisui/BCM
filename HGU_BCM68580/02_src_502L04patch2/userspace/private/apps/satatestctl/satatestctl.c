/*
 * <:copyright-BRCM:2014:proprietary:standard
 * 
 *    Copyright (c) 2014 Broadcom 
 *    All Rights Reserved
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :> 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "board.h"
#include "bcm_sata_test.h"

#define ENUM_TO_STR(x) #x

const char *sata_test_mode_name[] =
{
    ENUM_TO_STR(SATA_TESTMODE_START),
    ENUM_TO_STR(SATA_TESTMODE_CONFIGURATION),
    ENUM_TO_STR(SATA_TESTMODE_PHY_TSG_MOI),
    ENUM_TO_STR(SATA_TESTMODE_RX),
    ENUM_TO_STR(SATA_TESTMODE_RW_PHY_REGS),
    ENUM_TO_STR(SATA_TESTMODE_STOP),
    ENUM_TO_STR(SATA_TESTMODE_MAX)
};

const char *sata_phy_tsg_moi_test_name[] =
{
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_UNUSED),
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_LFTP),
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_LBP),
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_HFTP),
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_MFTP),
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_PRBS),
    ENUM_TO_STR(SATA_PHY_TSG_MOI_TEST_MAX)
};


const char *sata_rx_test_name[] =
{
    ENUM_TO_STR(SATA_RX_TEST_UNUSED),
    ENUM_TO_STR(SATA_RX_TEST_BIST_L_GEN1),
    ENUM_TO_STR(SATA_RX_TEST_BIST_L_GEN2),
    ENUM_TO_STR(SATA_RX_TEST_BIST_L_GEN3),
    ENUM_TO_STR(SATA_RX_TEST_MAX)
}; 

const char *sata_mode[]=
{
    "unknown",
    "SATA_GEN1i",
    "SATA_GEN1m",
    "SATA_GEN2i",
    "SATA_GEN2m",
    "SATA_GEN3i",
    "unknown"
};

int ioctl_fd;
struct sata_test_params g_test_params;

int usage_testmode(void)
{
    int mode, i;
    char cmd_line[160];
    int ret;
retry:
    printf("SATA MAIN TEST Menu: Select the test mode\n");
    for(i=1; i< SATA_TESTMODE_MAX;i++)
    {
        printf("\t\%d: %s\n",i , sata_test_mode_name[i]);
    }
    printf("Enter your choice\n");

    fgets(cmd_line, sizeof(cmd_line),stdin);
    ret = sscanf(cmd_line, "%d", &mode);
    if((ret == 0) || (ret == EOF))   
    {
        printf("Bad input format...Retry\n");
        goto retry;
    }

    if((mode < 1) ||( mode >= SATA_TESTMODE_MAX))
    {
        printf("Invalid Test Mode =%d\n",mode);
        printf("Try again\n");
        goto retry;
    }
    return mode;
}

void display_testmode_configuration(void)
{
    printf("***CURRENT CONFIG: MODE= %s, SSC= %s  TxAmp= %dmV***\n",
            g_test_params.sata_mode < SATA_MODE_MAX? sata_mode[g_test_params.sata_mode]:"unknown",
g_test_params.ssc_enabled ? "Enabled":"Disabled", g_test_params.tx_amp_mvolts
     );
}



void set_testmode_configuration(struct sata_test_params *test_params_p)
{
    char cmd_line[160];
    uint32_t param_type;
    uint32_t param_value;
    int ret,i;

retry:
    display_testmode_configuration();
    printf("\tSelect parameter to change\n");

    printf("\t\t 0:Return to Main Menu \n");
    printf("\t\t %d: SSC (Enable/Disable) \n", SATA_TEST_CONFIG_SSC);
    printf("\t\t %d: SATA MODE(Gen1/2/3)\n", SATA_TEST_CONFIG_SATAMODE);
    printf("\t\t %d: SATA TX Amplitude\n", SATA_TEST_CONFIG_TXAMP);
    printf("\tEnter Your Choice\n");
    printf("\t");

    fgets(cmd_line, sizeof(cmd_line),stdin);
    ret = sscanf(cmd_line, "%d", &param_type);

    if((ret == 0) || (ret == EOF))   
    {
        printf("Bad input format...Retry\n");
        goto retry;
    }

    if(param_type == 0)
        return;

    test_params_p->param_type = param_type;

    switch(param_type)
    {
        case SATA_TEST_CONFIG_SSC:
            printf("\t\t\t SATA SSC 1:enable 0:disable\n");
            printf("\t\t\t Enter Your Choice\n");
            printf("\t\t\t ");
            fgets(cmd_line, sizeof(cmd_line),stdin);
            ret = sscanf(cmd_line, "%d", &param_value);
            if((ret == 0) || (ret == EOF) || (param_value >1))   
            {
                printf("Bad input format...Retry\n");
                goto retry;
            }

            test_params_p->param1 = param_value;
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;

        case SATA_TEST_CONFIG_SATAMODE:

            printf("\t\t\t SATA MODE:\n");
            for(i=SATA_MODE_GEN1i; i<SATA_MODE_MAX; i++)
            {
                printf("\t\t\t\t %d:%s\n",i, sata_mode[i]);
            }
            printf("\t\t\t Enter Your Choice\n");
            printf("\t\t\t ");
            fgets(cmd_line, sizeof(cmd_line),stdin);
            ret = sscanf(cmd_line, "%d", &param_value);
            if((ret == 0) || (ret == EOF))   
            {
                printf("Bad input format...Retry\n");
                goto retry;
            }
            test_params_p->param1 = param_value;
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;

        case SATA_TEST_CONFIG_TXAMP:
            printf("\t\t\t TX Amplitude:Choose a value from 220mV to 1176mV\n");
            printf("\t\t\t Enter Your Choice\n");
            printf("\t\t\t ");
            fgets(cmd_line, sizeof(cmd_line),stdin);
            ret = sscanf(cmd_line, "%d", &param_value);
            if((ret == 0) || (ret == EOF))   
            {
                printf("Bad input format...Retry\n");
                goto retry;
            }
            
            if((param_value <220) || (param_value >1176))
            {
                printf("Invalid input select a value between 220 and 1176 ...Retry\n");
                goto retry;
            }

            test_params_p->param1 = param_value;
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;

        default:
            printf("\t\t Unsupported test parameter\n");
    }

    goto retry;

}

void run_testmode_rx(struct sata_test_params *test_params_p)
{
    int i, test_type;
    char cmd_line[160];
    int ret;

retry:
    printf("\tSATA RX TEST Menu: Select the test type\n");
    printf("\t\t 0:Return to Main Menu \n");

    for(i=SATA_RX_TEST_BIST_L_GEN1; i< SATA_RX_TEST_MAX;i++)
    {
        printf("\t\t %d: %s\n",i , sata_rx_test_name[i]);
    }
    printf("\tEnter Your Choice\n");
    printf("\t");

    fgets(cmd_line, sizeof(cmd_line),stdin);
    ret = sscanf(cmd_line, "%d", &test_type);

    if((ret == 0) || (ret == EOF))   
    {
        printf("Bad input format...Retry\n");
        goto retry;
    }

    if(test_type == 0)
        return;

    if((test_type < SATA_RX_TEST_BIST_L_GEN1) ||( test_type >= SATA_RX_TEST_MAX))
    {
        printf("Invalid Test Type =%d\n", test_type);
        printf("Try again\n");
        goto retry;
    }

    test_params_p->test_type = test_type;

    switch(test_type)
    {
        case SATA_RX_TEST_BIST_L_GEN1:
        case SATA_RX_TEST_BIST_L_GEN2:
        case SATA_RX_TEST_BIST_L_GEN3:
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;

        default:
            printf("Test %s not supported yet\n",  sata_rx_test_name[test_type]);
    }

    printf(">>>>Running Test %s: %s \n", sata_test_mode_name[test_params_p->test_mode], sata_rx_test_name[test_type]);
    printf("\nHit Enter to run different test\n");
    fgets(cmd_line, sizeof(cmd_line),stdin);
    goto retry;
}

void run_testmode_phy_tsg_moi(struct sata_test_params *test_params_p)
{
    int i, test_type;
    char cmd_line[160];
    int ret;
    unsigned int pattern;

retry:
    printf("\tSATA PHY_TSG_MOI TEST Menu: Select the test type\n");

    printf("\t\t 0:Return to Main Menu \n");

    for(i=SATA_PHY_TSG_MOI_TEST_LFTP; i< SATA_PHY_TSG_MOI_TEST_MAX;i++)
    {
        printf("\t\t %d: %s\n",i , sata_phy_tsg_moi_test_name[i]);
    }
    printf("\tEnter Your Choice\n");
    printf("\t");

    fgets(cmd_line, sizeof(cmd_line),stdin);
    ret = sscanf(cmd_line, "%d", &test_type);

    if((ret == 0) || (ret == EOF))   
    {
        printf("Bad input format...Retry\n");
        goto retry;
    }

    if(test_type == 0)
        return;

    if((test_type < SATA_PHY_TSG_MOI_TEST_LFTP) ||( test_type >= SATA_PHY_TSG_MOI_TEST_MAX))
    {
        printf("Invalid Test Type =%d\n", test_type);
        printf("Try again\n");
        goto retry;
    }

    test_params_p->test_type = test_type;

    switch(test_type)
    {
        case SATA_PHY_TSG_MOI_TEST_LFTP:
        case SATA_PHY_TSG_MOI_TEST_LBP:
        case SATA_PHY_TSG_MOI_TEST_HFTP:
        case SATA_PHY_TSG_MOI_TEST_MFTP:
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;
        case SATA_PHY_TSG_MOI_TEST_PRBS:

    retry2:
            printf("\t\tChoose the Pattern:\n");
            printf("\t\t1: 0x88 ; 2: 0x99 ; 3: 0xaa ; 4:0xbb \n");
            printf("\t\t");
            fgets(cmd_line, sizeof(cmd_line),stdin);
            ret = sscanf(cmd_line, "%d", &pattern);

            if((ret == 0) || (ret == EOF))
            {
                printf("Bad input format...Retry\n");
                goto retry2;
            }

            test_params_p->param1 = pattern;
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;

        default:
            printf("Test %s not supported yet\n",  sata_phy_tsg_moi_test_name[test_type]);
    }

    printf(">>>>Running Test %s: %s \n", sata_test_mode_name[test_params_p->test_mode], sata_phy_tsg_moi_test_name[test_type]);
    display_testmode_configuration();
    printf("\nHit Enter to run different test\n");
    fgets(cmd_line, sizeof(cmd_line),stdin);
    goto retry;
} 

void run_testmode_rw_phy_regs(struct sata_test_params *test_params_p)
{
    int access_type;
    char cmd_line[160];
    int ret;
    uint32_t mdio_bank, reg_num, value;

retry:
    printf("\tSATA Read/Write PHY registers : Select access type\n");

    printf("\t\t 0:Return to Main Menu \n");

        printf("\t\t 1: READ  PHY Register\n");
        printf("\t\t 2: WRITE PHY Register\n");
    printf("\tEnter Your Choice\n");

    fgets(cmd_line, sizeof(cmd_line),stdin);
    ret = sscanf(cmd_line, "%d", &access_type);

    if((ret == 0) || (ret == EOF))   
    {
        printf("Bad input format...Retry\n");
        goto retry;
    }

    if(access_type == 0)
        return;

    if((access_type < 1) ||( access_type >2))
    {
        printf("Invalid choice  =%d\n", access_type);
        printf("Try again\n");
        goto retry;
    }

    test_params_p->test_type = access_type;

    switch(access_type)
    {
        case SATA_PHY_REG_READ:
retry2:
            printf("Enter <REG_BANK> <REG_NUM> \n");
            fgets(cmd_line, sizeof(cmd_line),stdin);
            ret = sscanf(cmd_line, "%x %x", &mdio_bank, &reg_num);

            if((ret == 0) || (ret == EOF))   
            {
                printf("Bad input format...Retry\n");
                goto retry2;
            }
            test_params_p->param1 = mdio_bank;
            test_params_p->param2 = reg_num;

            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;
        case SATA_PHY_REG_WRITE:
retry3:
            printf("Enter <REG__BANK> <REG_NUM> <VALUE> \n");
            fgets(cmd_line, sizeof(cmd_line),stdin);
            ret = sscanf(cmd_line, "%x %x %x", &mdio_bank, &reg_num, &value);

            if((ret == 0) || (ret == EOF))   
            {
                printf("Bad input format...Retry\n");
                goto retry3;
            }
            test_params_p->param1 = mdio_bank;
            test_params_p->param2 = reg_num;
            test_params_p->param3 = value;
            ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, test_params_p);
            break;


        default:
            printf("Error Unknow command\n");
    }
    goto retry;
} 

int main_testmode(void)
{

    //printf(">>>Entering SATA testmode:\n");

    /* relenquish the SATA controller from kernel by
     * removing SATA/AHCI drivers
     */
    //system("sh /etc/init.d/disk.sh suspend");
    //sleep(2);


    /* Initilaize SATA contorller for test mode */
    g_test_params.test_mode = SATA_TESTMODE_START;
    ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, &g_test_params);

    sleep(1);

    do{
        g_test_params.test_mode = usage_testmode();

        switch(g_test_params.test_mode)
        {
            case SATA_TESTMODE_CONFIGURATION:
                set_testmode_configuration(&g_test_params);
                break;

            case SATA_TESTMODE_PHY_TSG_MOI:
                run_testmode_phy_tsg_moi(&g_test_params);
                break;

            case SATA_TESTMODE_RX:
                run_testmode_rx(&g_test_params);
                break;

            case SATA_TESTMODE_RW_PHY_REGS:
                run_testmode_rw_phy_regs(&g_test_params);
                break;

            case SATA_TESTMODE_STOP:
                ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, &g_test_params);
                break;

            default:
                printf("test_mode:%s not supported yet\n",sata_test_mode_name[g_test_params.test_mode]);
        }
    } while(g_test_params.test_mode != SATA_TESTMODE_STOP);
    
    return 0;
}

void usage(void)
{
    printf("\nSATA test utility ::: Usage:\n\n");
    printf("satatestctl testmode                                : use to run test patterns\n\n");  
    printf("satatestctl phy_reg_rd <reg_bank> <reg_num>         : read a PHY register\n");  
    printf("\tEx: satatestctl phy_reg_rd 0x1a0 0x1\n\n");
    printf("satatestctl phy_reg_wr <reg_bank> <reg_num> <value> : write to PHY register\n");  
    printf("\tEx: satatestctl phy_reg_wr 0x1a0 0x1 0x3\n\n");
    printf("Note1: Before running testmode make sure you remove linux SATA modules(ata/ahci\n");
    printf("       to avoid interference, you can use /etc/init.d/disk.sh suspend\n");

    printf("Note2: phy_reg_rd & phy_reg_wr should be used only when linux SATA modules are present\n");
}

int main (int argc, char **argv)
{
    char *cmd;
    unsigned long reg_bank;
    unsigned long reg_num;
    unsigned long reg_value;

    /* open fd for ioctls */
    if ((ioctl_fd = open (BOARD_DEVICE_NAME, O_RDWR)) < 0)
    {
        printf ("Can't open /dev/brcmboard ");
        return -1;
    }

    if(argc < 2)
    {
        usage();
        goto out;
    }
    cmd =argv[1];


    if(!strcmp("phy_reg_rd",cmd))
    {
        if(argc != 4)
        {
            usage();
            goto out;
        }

        /*TODO: need to add checks to detect illegal values for reg_bank &reg_num*/
        reg_bank = strtoul(argv[2], NULL, 16);
        reg_num = strtoul(argv[3], NULL, 16);

        g_test_params.test_mode = SATA_TESTMODE_RW_PHY_REGS;
        g_test_params.test_type = SATA_PHY_REG_READ;
        g_test_params.param1 = reg_bank;
        g_test_params.param2 = reg_num;

        ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, &g_test_params);
    }
    else if(!strcmp("phy_reg_wr",cmd)) 
    {
        if(argc != 5)
        {
            usage();
            goto out;
        }

        /*TODO: need to add checks to detect illegal values for reg_bank, reg_num & value*/

        reg_bank = strtoul(argv[2], NULL, 16);
        reg_num = strtoul(argv[3], NULL, 16);
        reg_value = strtoul(argv[4], NULL, 16);

        g_test_params.test_mode = SATA_TESTMODE_RW_PHY_REGS;
        g_test_params.test_type = SATA_PHY_REG_WRITE;
        g_test_params.param1 = reg_bank;
        g_test_params.param2 = reg_num;
        g_test_params.param3 = reg_value;

        ioctl(ioctl_fd, BOARD_IOCTL_SATA_TEST, &g_test_params);
    }
    else if(!strcmp("testmode",cmd))
    {
        main_testmode();
    }
    else
    {
        printf("unsupported option\n");
        usage();
        goto out;
    }

out:
    close(ioctl_fd);
    return (0);
}
