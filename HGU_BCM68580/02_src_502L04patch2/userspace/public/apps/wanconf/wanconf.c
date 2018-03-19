/***********************************************************************
 *
 * <:copyright-BRCM:2015:DUAL/GPL:standard
 *
 *    Copyright (c) 2015 Broadcom
 *    All Rights Reserved
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 *
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 *
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 *
 * :>
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>	 /* needed to define getpid() */
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "cms_psp.h"
#include <sys/socket.h>
#include <linux/if.h>

#include <bcmnet.h>
#include "cms_boardioctl.h"
#include <cms_mem.h>
#include <cms_msg.h>
#include <rdpa_types.h>
#include "board.h"
#include "opticaldet.h"
#include <bcm/bcmswapitypes.h>

#include <fcntl.h>
#include <dirent.h>
#include <sched.h>


#define IFNAMESIZ 16


int rdpaCtl_RdpaMwWanConf(void);
int rdpaCtl_time_sync_init(void);

#define wc_log_err(fmt, arg...) fprintf(stderr, "wanconf %s:%d >> " fmt "\n", __FILE__, __LINE__, ##arg);
#define wc_log(fmt, arg...) printf("wanconf: " fmt "\n", ##arg);

#define insmod(d) insmod_param(d, NULL)

#if defined(BRCM_CMS_BUILD)
static CmsRet sendWanOpStateMsg(void *msgHandle, WanConfPhyType phyType, UBOOL8 opState);
#endif /* BRCM_CMS_BUILD */

int insmod_param(char *driver, char *param)
{
    char cmd[128];
    int ret;

    sprintf(cmd, "insmod %s %s", driver, param ? : "");
    ret = system(cmd);
    if (ret)
        wc_log_err("unable to load module: %s\n", cmd);

    return ret;
}

#if defined(BRCM_CMS_BUILD)
CmsRet smd_start_app(UINT32 wordData)
{
    void *msgBuf;
    CmsRet ret;
    CmsMsgHeader *msg;
    void *msgHandle;
    int pid;

    ret = cmsMsg_initWithFlags(EID_WANCONF, 0, &msgHandle);
    if (ret)
    {
        wc_log_err("message init failed ret=%d\n", ret);
        return ret;
    }

    msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
    if (msgBuf == NULL)
    {
        wc_log_err("message allocation failed\n");
        return CMSRET_INTERNAL_ERROR;
    }

    msg = (CmsMsgHeader *)msgBuf;
    msg->src = EID_WANCONF;
    msg->dst = EID_SMD;
    msg->flags_event = FALSE;
    msg->type = CMS_MSG_START_APP;
    msg->wordData = wordData;
    msg->dataLength = 0;

    pid = (int)cmsMsg_sendAndGetReply(msgHandle, msg);
    if (pid == CMS_INVALID_PID)
    {
        wc_log_err("Failed to start app\n");
        return CMSRET_INTERNAL_ERROR;
    }

    if (wordData == EID_EPON_APP)
    {
        ret = sendWanOpStateMsg(msgHandle, WANCONF_PHY_TYPE_EPON, TRUE);
    }
    else if (wordData == EID_OMCID)
    {
        ret = sendWanOpStateMsg(msgHandle, WANCONF_PHY_TYPE_GPON, TRUE);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
    cmsMsg_cleanup(&msgHandle);

    return ret;
}
#endif /* BRCM_CMS_BUILD */

int load_gpon_modules(rdpa_wan_type wan_type)
{
    int rc = 0;

    if (wan_type == rdpa_wan_gpon)
    {
        rc = insmod("/lib/modules/"KERNELVER"/extra/gponstack.ko");
    }
    else if (wan_type == rdpa_wan_xgpon)
    {
        rc = insmod("/lib/modules/"KERNELVER"/extra/ngponstack.ko");
    }

    rc = insmod("/lib/modules/"KERNELVER"/extra/bcmgpon.ko");

    rc = rc ? : system("cat /dev/rgs_logger &");
#if defined(BRCM_CMS_BUILD)
    if (!rc && (smd_start_app(EID_OMCID)) != CMSRET_SUCCESS)
    {
        wc_log_err("Failed to start omcid app\n");
        return -1;
    }
#endif /* BRCM_CMS_BUILD */

    return rc;
}

static int getPidByName(const char *name)
{
    DIR *dir;
    FILE *fp;
    struct dirent *dent;
    UBOOL8 found=FALSE;
    long pid;
    int  rc, p, i;
    int rval = CMS_INVALID_PID;
    char filename[BUFLEN_256];
    char processName[BUFLEN_256];
    char *endptr;

    if (NULL == (dir = opendir("/proc"))) {
        wc_log_err("could not open /proc\n");
        return rval;
    }

    while (!found && (dent = readdir(dir)) != NULL) {
        /*
         * Each process has its own directory under /proc, the name of the
         * directory is the pid number.
         */
        if (dent->d_type != DT_DIR)
            continue;

        pid = strtol(dent->d_name, &endptr, 10);
        if (ERANGE != errno && endptr != dent->d_name) {
            snprintf(filename, sizeof(filename), "/proc/%ld/stat", pid);
            if ((fp = fopen(filename, "r")) == NULL) {
                wc_log_err("could not open %s\n", filename);
            } else {
                /* Get the process name, format: 913 (consoled) */
                memset(processName, 0, sizeof(processName));
                rc = fscanf(fp, "%d (%s", &p, processName);
                fclose(fp);

                if (rc >= 2) {
                    i = strlen(processName);
                    if (i > 0) {
                        /* strip out the trailing ) character */
                        if (processName[i-1] == ')')
                            processName[i-1] = 0;
                    }
                }

                if (!strncmp(processName, name,strlen(name))) {
                    rval = pid;
                    found = TRUE;
                }
            }
        }
    }

    closedir(dir);

    return rval;
}

int load_epon_modules(int launch_app)
{
#define KTHREAD_NAME "EponMPCP"
    int pid;
    int rc = insmod_param("/lib/modules/"KERNELVER"/extra/bcmepon.ko", "epon_usr_init=1");
    struct sched_param sp = { .sched_priority = 10 };

    if(launch_app == 0)
        return rc;

#if defined(BRCM_CMS_BUILD)
    if (!rc && (smd_start_app(EID_EPON_APP) != CMSRET_SUCCESS))
    {
        wc_log_err("Failed to start eponapp\n");
        return -1;
    }
#endif /* BRCM_CMS_BUILD */

    pid = getPidByName(KTHREAD_NAME);
    if (pid > 0)
    {
        if (sched_setscheduler(pid, SCHED_RR, &sp) == -1)
        {
            wc_log_err("failed to set kthread %s with scheduler RR\n", KTHREAD_NAME);
            return -1;
        }
    }
    else
    {
        wc_log_err("unable to find pid for kthread %s\n", KTHREAD_NAME);
    }

    return rc;
}

#ifndef CONFIG_BCM963158
static int detect_wan_type(rdpa_wan_type *wan_type, int *epon2g)
{
    int fd, ret, val;

    fd = open("/dev/opticaldetect", O_RDWR);
    if (fd < 0)
    {
        wc_log_err("%s: %s\n", "/dev/opt..", strerror(errno));
        return -EINVAL;
    }

    /* block till Optical Signal is detected */
    wc_log("block till Optical Signal is detected...\n");
    ret = ioctl(fd, OPTICALDET_IOCTL_SD, &val);
    if (ret)
        wc_log_err("ioctl failed, Errno[%s] ret=%d\n", strerror(errno), ret);

    wc_log("Optical Signal is detected!\n");

    /* got a signal. Define the type of the signal : GPON, EPON, AE */
    ret = ioctl(fd, OPTICALDET_IOCTL_DETECT, &val);
    if (ret)
        wc_log_err("ioctl failed, Errno[%s] ret=%d\n", strerror(errno), ret);

    close(fd);

    *wan_type = val & RDPA_WAN_MASK;
    *epon2g = val & EPON2G;
	
    return 0;
}

char *wan_type_str(rdpa_wan_type wan_type)
{
    switch (wan_type)
    {
    case rdpa_wan_gpon:
        return RDPA_WAN_TYPE_VALUE_GPON;
    case rdpa_wan_epon:
        return RDPA_WAN_TYPE_VALUE_EPON;
    case rdpa_wan_gbe: /* Detection code will return rdpa_wan_gbe for AE */
        return RDPA_WAN_TYPE_VALUE_GBE;
    case rdpa_wan_xgpon:
        return RDPA_WAN_TYPE_VALUE_XGPON1;
    case rdpa_wan_xepon:
        return RDPA_WAN_TYPE_VALUE_EPON;
    default:
        return NULL;
    }
}

static rdpa_wan_type detect_and_set_scratchpad(void)
{
    char *wanStr = NULL;
    int epon2g;
    rdpa_wan_type wan_type;

    while (!wanStr)
    {
        if (!detect_wan_type(&wan_type, &epon2g))
            wanStr = wan_type_str(wan_type);
        if (!wanStr)
            wc_log_err("Failed to autodetect, retrying\n");
    }

    /* Set detected WAN in scratchpad */

    wc_log("cmsPsp_set wanStr = %s\n", wanStr);
    if (cmsPsp_set(RDPA_WAN_TYPE_PSP_KEY, wanStr, strlen(wanStr)))
    {
        wc_log_err("cmsPsp_set failed to set new "RDPA_WAN_TYPE_PSP_KEY" = %s\n", wanStr);
        return -1;
    }

    if (wan_type == rdpa_wan_gbe) /* Detect rdpa_wan_gbe is always AE! */
    {
        char *emac_str = "EMAC5";

        if (cmsPsp_set(RDPA_WAN_OEMAC_PSP_KEY, emac_str, strlen(emac_str)))
            return -1;
    }
    else if (wan_type == rdpa_wan_epon)
    {
        char *epon_speed = epon2g ? "Turbo" : "Normal";
        char wan_rate[PSP_BUFLEN_16] ={};

        sprintf(wan_rate, "%s%s", epon2g ? RDPA_WAN_RATE_2_5G : RDPA_WAN_RATE_1G, RDPA_WAN_RATE_1G);

        if (cmsPsp_set(RDPA_EPON_SPEED_PSP_KEY, epon_speed, strlen(epon_speed)))
            return -1;

        if (cmsPsp_set(RDPA_WAN_RATE_PSP_KEY, wan_rate, strlen(wan_rate)))
            return -1;
    }

    return wan_type;
}
#endif

int create_bcmenet_vport(char *ifname, int op, rdpa_if port)
{
    struct ifreq ifr;
    int err, skfd;
    struct ethctl_data ethctl;

    memset(&ethctl, 0x0, sizeof(struct ethctl_data));
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket open error\n");
        return -1;
    }

    strcpy(ifr.ifr_name, "bcmsw");
    if ((err = ioctl(skfd, SIOCGIFINDEX, &ifr)) < 0 )
    {
        printf("bcmsw interface does not exist");
        goto exit;
    }

    ethctl.op = op;
    ethctl.val = port;
    ifr.ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, &ifr);
    if (err < 0 )
        printf("Error %d bcmenet gbe wan port\n", err);

    strcpy(ifname, ethctl.ifname);

exit:
    close(skfd);
    return err;
}

int get_enet_wan_port(char *ifname, unsigned int len)
{
    struct ifreq ifr;
    int err, skfd;
    struct ethswctl_data ethswctl;

    memset(&ethswctl, 0x0, sizeof(struct ethswctl_data));
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket open error\n");
        return -1;
    }

    strcpy(ifr.ifr_name, "bcmsw");
    if ((err = ioctl(skfd, SIOCGIFINDEX, &ifr)) < 0 )
    {
        printf("bcmsw interface does not exist");
        goto exit;
    }

    ethswctl.up_len.uptr = ifname;
    ethswctl.up_len.len = len;
    ifr.ifr_data = (void *)&ethswctl;
    err = ioctl(skfd, SIOCGWANPORT, &ifr);
    if (err < 0 )
        printf("Error %d bcmenet gbe wan port\n", err);

exit:
    close(skfd);
    return err;
}

#if defined(BRCM_CMS_BUILD)
void gbeSendPostMdmMsg(void *msgHandle)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(MdmPostActNodeInfo)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;
    MdmPostActNodeInfo *msgBody = (MdmPostActNodeInfo *)(msgHdr + 1);

    msgHdr->dst = EID_SSK;
    msgHdr->src = EID_WANCONF;
    msgHdr->type = CMS_MSG_MDM_POST_ACTIVATING;
    msgHdr->flags_event = 1;
    msgHdr->dataLength = sizeof(MdmPostActNodeInfo);
    msgBody->subType = MDM_POST_ACT_TYPE_FILTER;

    // Attempt to send CMS response message & test result.
    cmsReturn = cmsMsg_send(msgHandle, msgHdr);
    if (cmsReturn != CMSRET_SUCCESS)
    {
        // Log error.
        wc_log_err("Send message failure, cmsResult: %d", cmsReturn);
    }
    else
    {
        wc_log_err("Sent Wanconf App Indication to SSK");
    }
}

static int sendEnablePort(char *if_name)
{
    char buf[sizeof(CmsMsgHeader) + IFNAMESIZ]={0};
    CmsMsgHeader *msg=(CmsMsgHeader *) buf;
    char *msg_ifname = (char *)(msg+1);
    CmsRet ret;
    void *msgHandle;

    if (strlen(if_name) > IFNAMESIZ -1)
        return -1;

    ret = cmsMsg_initWithFlags(EID_WANCONF, 0, &msgHandle);
    if (ret)
        return ret;

    msg->type = CMS_MSG_WAN_PORT_ENABLE;
    msg->src = EID_WANCONF;
    msg->dst = EID_SSK;
    msg->flags_event = 1;
    msg->dataLength = IFNAMESIZ;

    strcpy(msg_ifname, if_name);

    if ((ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS)
    {
        wc_log_err("could not send out CMS_MSG_WAN_PORT_ENABLE, ret=%d", ret);
        return -1;
    }
    gbeSendPostMdmMsg(msgHandle);

    cmsMsg_cleanup(&msgHandle);

    return 0;
}

static CmsRet sendWanOpStateMsg(void *msgHandle, WanConfPhyType phyType, UBOOL8 opState)
{
    CmsRet ret = CMSRET_SUCCESS;
    char buf[sizeof(CmsMsgHeader) + sizeof(WanConfPhyOpStateMsgBody)] = {0};
    CmsMsgHeader *msg = (CmsMsgHeader*)buf;
    WanConfPhyOpStateMsgBody *info;

    msg->type = CMS_MSG_WAN_PORT_SET_OPSTATE;
    msg->src = EID_WANCONF;
    msg->dst = EID_SSK;
    msg->flags_request = 1;
    msg->flags_response = 0;
    msg->flags_event = 0;
    msg->dataLength = sizeof(WanConfPhyOpStateMsgBody);
    msg->wordData = 0;

    info = (WanConfPhyOpStateMsgBody*)&(buf[sizeof(CmsMsgHeader)]);
    info->phyType = phyType;
    info->opState = opState;

    /* Wait for reply to serialize the processing. */
    ret = cmsMsg_sendAndGetReply(msgHandle, msg);
    if (ret != CMSRET_SUCCESS)
    {
        wc_log_err("cmsMsg_sendAndGetReply(CMS_MSG_WAN_PORT_SET_OPSTATE) failed, ret=%d", ret);
    }

    return ret;
}
#endif

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
    pid_t childPid = fork();
    char buf[16];
    int rc, launch_app = 1;
    rdpa_wan_type wan_type = rdpa_wan_none;
#ifndef CONFIG_BCM963158
    int count;
#endif
    /* BP_BOARD_ID_LEN(16) defined in boardparms.h */
    char boardid[16];

    if (childPid < 0) /* Failed to fork */
        return -1;

    if (childPid != 0) /* Father always exists */
        return 0;

#ifdef CONFIG_BCM963158
    wan_type = rdpa_wan_gpon;
#else
    /* read a scratchpad */
    memset(buf, 0, sizeof(buf));
    count = cmsPsp_get(RDPA_WAN_TYPE_PSP_KEY, buf, sizeof(buf));
    if (count <= 0)
    {
        wc_log_err("unexpected data in %s, len=%d", RDPA_WAN_TYPE_PSP_KEY, count);
        return -1;
    }

    wc_log("cmsPsp_get: rdpaWanType=%s\n", buf);

    if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_GPON))
        wan_type = rdpa_wan_gpon;
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_EPON))
    {
        char eponBuf[16];
#define RATE_STR_LEN 2
#define PSP_RATE_STR_LEN 4

        wan_type = rdpa_wan_epon;
        /* read a scratchpad */
        memset(eponBuf, 0, sizeof(eponBuf));
        count = cmsPsp_get(RDPA_WAN_RATE_PSP_KEY, eponBuf, sizeof(eponBuf));
        if ((count >= PSP_RATE_STR_LEN) &&
            (!strncasecmp(&buf[RATE_STR_LEN], RDPA_WAN_RATE_10G, RATE_STR_LEN)))
        {
            // open xepon when with 10G upstream rate
            wan_type = rdpa_wan_xepon;
        }
        launch_app = 1;
    }
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_AE))
        wan_type = rdpa_wan_gbe;
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_GBE))
    {
        count = cmsPsp_get(RDPA_WAN_OEMAC_PSP_KEY, buf, sizeof(buf));
        if (count <= 0)
        {
            wc_log_err("unexpected data in %s, len=%d", RDPA_WAN_OEMAC_PSP_KEY, count);
            return -1;
        }
        buf[count] = '\0';
        if (!strcasecmp(buf, "EPONMAC"))
        {
            wan_type = rdpa_wan_xepon;
            launch_app = 0;
        }
        else
            wan_type = rdpa_wan_gbe;
    }
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_XGPON1) || !strcasecmp(buf, RDPA_WAN_TYPE_VALUE_NGPON2) || !strcasecmp(buf, RDPA_WAN_TYPE_VALUE_XGS))
        wan_type = rdpa_wan_xgpon;
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_AUTO))
        wan_type = detect_and_set_scratchpad();
    else
    {
        wc_log_err("Unknown wan set in scratchpad %s = %s\n", RDPA_WAN_TYPE_PSP_KEY, buf);
        return -1;
    }
#endif

    rc = rdpaCtl_RdpaMwWanConf();
    if (rc)
    {
        wc_log("Failed to call rdpa_mw ioctl (rc=%d)\n", rc);
        return rc;
    }

    switch (wan_type)
    {
        case rdpa_wan_gpon:
        case rdpa_wan_xgpon:
            rc = load_gpon_modules(wan_type);
            break;
        case rdpa_wan_epon:
        case rdpa_wan_xepon:
            rc = load_epon_modules(launch_app);
            /*epon ae mode*/
            if(launch_app == 0)
            {
                char ifname[IFNAMESIZ];
                ifname[0] = '\0';

                rc = create_bcmenet_vport(ifname, ETHCREATEEPONAEVPORT, rdpa_wan_type_to_if(wan_type));
#if defined(BRCM_CMS_BUILD)
                rc = rc ? : sendEnablePort(ifname);
#endif /* BRCM_CMS_BUILD */
            }
            break;
        case rdpa_wan_gbe:
            {
                char ifname[IFNAMESIZ];
                ifname[0] = '\0';

                /* Get the enet wan port if already configured in rdpa */
                rc = get_enet_wan_port(ifname, IFNAMESIZ);
                if (rc)
                    goto exit;

                if (ifname[0] == '\0')
                    rc = create_bcmenet_vport(ifname, ETHINITWAN, rdpa_wan_type_to_if(wan_type));

#if defined(BRCM_CMS_BUILD)
                rc = rc ? : sendEnablePort(ifname);
#endif /* BRCM_CMS_BUILD */
                break;
            }
        default:
            wc_log_err("Unsupported wanconf type set in scratchpad %s = %s\n", RDPA_WAN_TYPE_PSP_KEY, buf);
            return -1;
    }

    if ((rc = devCtl_boardIoctl(BOARD_IOCTL_GET_ID, 0, boardid,
                    sizeof(boardid), 0, NULL)) != CMSRET_SUCCESS)
    {
         wc_log_err("Could not get Board Id");
         goto exit;
    }
    else if (strcmp(boardid,"963158REF2") == 0)
    {
         /* Workaround for a REF2 hardware issue */
         /* Set rx_pmd_dp_invert bit(RX PMD Datapath Invert Control) in SerDes */
         if (system("serdesctrl reg 0xd0d3 0x1") == -1)
         {
             wc_log_err("Failed to execute command - serdesctrl reg 0xd0d3 0x1");
         }
         else
         {
             printf("REF2 SerDes Workaround applied successfully\n");
         }
    }


exit:
    if (rc)
    {
        wc_log("wanconf was not loaded successfully (rc=%d)\n", rc);
        return rc;
    }

    rc = rdpaCtl_time_sync_init();
    if (rc)
    {
        wc_log("Failed to call rdpaCtl_time_sync_init ioctl (rc=%d)\n", rc);
        return rc;
    }

    return 0;
}

