/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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
 *
 ************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <net/if.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "os_defs.h"

#include <simio_def_common.h>
#include "simio_user_kernel.h"
#include "simcard_ctl_api.h"

/*
 * Macros
 */

#define SIMCARD_IOCTL_FILE_NAME "/dev/bcm_sim_card"
#define SIMCARD_MAX_FD 10

//#define CC_SIMCARD_CTL_DEBUG

#ifdef CC_SIMCARD_CTL_DEBUG
#define simcard_debug(fmt, arg...) {printf(">>> %s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg); fflush(stdout);}
#define simcard_error(fmt, arg...) {printf("ERROR[%s.%u]: " fmt, __FUNCTION__, __LINE__, ##arg); fflush(stdout);}

static inline void dump_data(uint8_t *data, size_t size)
{
    int i, j, k=0;
    printf("Dump data(%s.%u) %d bytes: \n", __FUNCTION__, __LINE__, size);

    for (i = 0, j = 0; i < size; i++, j++)
    {
        j %=16;
        if (j == 0)
            printf("%d : ", k++);
        
        printf("0x%02x%s", data[i], j == 15 ? "\n" : " ");
    }

    printf("\n====== End of data ========\n");
}
#else
#define simcard_debug(fmt, arg...)
#define simcard_error(fmt, arg...)
#define dump_data(a, b)
#endif

typedef struct simio_app_ctx_s {
    SIMIO_ID_t sim_id;
    bcm_simio_app_cbs_t cbs;
    void *app_ctx;
    struct simio_app_ctx_s *next;
} simio_app_ctx_t;

typedef struct simio_file_ctx_s {
    uint16_t filename;
    char *pin1;
    char *pin2;
    struct simio_file_ctx_s *next;
} simio_file_ctx_t;

typedef struct simio_fd_ctx_s {
    simio_app_ctx_t *owner;
    simio_file_ctx_t *head;
    simio_file_ctx_t **tail;
} simio_fd_ctx_t;

typedef struct simio_active_fd_s {
    int index;
    uint8_t response[256];
    size_t response_size;
    char *ptr_pin1;
    char *ptr_pin2;
} simio_active_fd_t;

typedef struct simio_lib_ctx_s {
    uint32_t ref_cnt;
    simio_app_ctx_t *app_list[SIMIO_ID_1];
    simio_fd_ctx_t fd[SIMCARD_MAX_FD];
    simio_active_fd_t current_fd[SIMIO_ID_1];
} simio_lib_ctx_t;

static simio_lib_ctx_t *simio_lib_ctx = NULL;

#define SIM_CMD_SELECT          0xa0, 0xa4, 0x00, 0x00, 0x02
#define SIM_CMD_GET_RESPONSE    0xa0, 0xc0, 0x00, 0x00, 0x00
#define SIM_CMD_READ            0xa0, 0xb0, 0x00, 0x00, 0x00
#define SIM_CMD_WRITE           0xa0, 0xd0, 0x00, 0x00, 0x00
#define SIM_CMD_UPDATE          0xa0, 0xd6, 0x00, 0x00, 0x00
#define SIM_CMD_VERIFY_CHV      0xa0, 0x20, 0x00, 0x00, 0x08
#define SIM_CHV1_OFFSET         13
#define SIM_CHV1_FLAG           0x80
#define SIM_FILE_TYPE_OFFSET    6
#define SIM_FILE_TYPE_MF        0x1
#define SIM_FILE_TYPE_DF        0x2
#define SIM_FILE_TYPE_EF        0x4
#define SIM_FILE_ACCESS_COND_OFFSET 8
#define SIM_FILE_ACCESS_MASK    0x0F
#define SIM_FILE_UPDATE_COND(a) ((a) & SIM_FILE_ACCESS_MASK)
#define SIM_FILE_READ_COND(a)   (((a) >> 4) & SIM_FILE_ACCESS_MASK)
enum 
{ 
    SIM_FILE_ACCESS_ALW = 0x0,
    SIM_FILE_ACCESS_CHV1 = 0x1,
    SIM_FILE_ACCESS_CHV2 = 0x2,
    SIM_FILE_ACCESS_RFU = 0x3,
    SIM_FILE_ACCESS_ADM = 0x4,
    SIM_FILE_ACCESS_ADM2 = 0xE,
    SIM_FILE_ACCESS_NEVER = 0xF
};
/*
 * Static functions
 */

static inline int __simcard_ioctl(int code, uint32_t ctx)
{
    int fd, ret = 0;

    fd = open(SIMCARD_IOCTL_FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        simcard_error("%s: %s\n", SIMCARD_IOCTL_FILE_NAME, strerror(errno));
        return -EINVAL;
    }

    ret = ioctl(fd, code, ctx);
    if (ret)
        simcard_error("ioctl failed, Errno[%s] ret=%d\n", strerror(errno), ret);

    close(fd);
    return ret;
}

static inline int simcard_select_file(SIMIO_ID_t sim_id, unsigned short file_id,
    uint8_t *respond_buf, size_t *respond_len)
{
    int ret;
    unsigned char resp[3] = {};
    unsigned char cmd[7] = { SIM_CMD_SELECT };
    unsigned char get_resp[5] = { SIM_CMD_GET_RESPONSE };
    size_t len, rlen;
    simio_ioctl_arg_t arg = {};

    if (!respond_buf || !respond_len)
    {
        simcard_error("Both respond_buf and respond len pointers should be provided\n");
        return -EINVAL;
    }

    arg.sim_id = sim_id;

    simcard_debug("SIM %d Select File %04x", sim_id,  file_id);

    cmd[5] = file_id >> 8;
    cmd[6] = file_id & 0xFF;

    memcpy(arg.data.io.data, cmd, sizeof(cmd));
    arg.data.io.tx_len = sizeof(cmd);

    ret = __simcard_ioctl(SIMIO_WRITE, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Write command failed %d \n", arg.ret);
        return arg.ret;
    }

    len = arg.data.io.rx_len;
    if (len != 2)
    {
        simcard_error("Unexpected respond length %d (should be 2)\n", len);
        return -EINVAL;
    }
    
    memcpy(resp, arg.data.io.data, len);

    simcard_debug("Respond Dump: 0x%02x 0x%02x\n", resp[0], resp[1]);
    
    if (resp[0] == 0x98 && resp[1] == 0x04)
    {
        simcard_error("The Pin was not set\n");
        return -EPERM;
    }

    if (resp[0] == 0x6e)
    {
        simcard_error("Used CLA not supported\n");
        return -ENOTSUP;
    }

    if (resp[0] != 0x6c && resp[0]!= 0x9f && resp[0] != 0x61)
    {
        simcard_error("Unexpected response 0x%02x ( should be one of the "
            "following 0x6c, 0x9f, 0x61)\n", resp[0]);
        return -EINVAL;
    }
    
    get_resp[4] = resp[1];
    simcard_debug("Trying to get response %d bytes long\n", resp[1]);

    memcpy(arg.data.io.data, get_resp, sizeof(get_resp));
    arg.data.io.tx_len = sizeof(get_resp);
    arg.data.io.rx_len = 0;

    ret = __simcard_ioctl(SIMIO_READ, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Read command failed %d \n", arg.ret);
        return arg.ret;
    }

    rlen = arg.data.io.rx_len;
    *respond_len = rlen;
    memcpy(respond_buf, arg.data.io.data, rlen);

    return 0;
}

static inline int simcard_set_pin(SIMIO_ID_t sim_id, int chv_id, const char *pin)
{
    int ret;
    uint8_t cmd[5] = { SIM_CMD_VERIFY_CHV };
    simio_ioctl_arg_t arg = {};

    simcard_debug("SIM %d setting PIN\n", sim_id);

    if (!pin)
    {
        simcard_error("No PIN specified\n");
        return -EINVAL;
    }
    
    if (chv_id != SIM_FILE_ACCESS_CHV1 && chv_id != SIM_FILE_ACCESS_CHV2)
    {
        simcard_error("Only PIN1 and PIN2 is supported\n");
        return -EINVAL;
    }

    if (strlen(pin) > 8)
    {
        simcard_error("PIN is too long\n");
        return -EINVAL;
    }

    cmd[3] = chv_id;

    arg.sim_id = sim_id;

    memset(arg.data.io.data, 0xff, 13);
    memcpy(arg.data.io.data, cmd, 5);
    memcpy(&arg.data.io.data[5], pin, strlen(pin));
    
    ret = __simcard_ioctl(SIMIO_WRITE, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Write command failed %d \n", arg.ret);
        return arg.ret;
    }

	if (arg.data.io.rx_len != 2 || arg.data.io.data[0] != 0x90 || arg.data.io.data[1] != 0x00)
    {
		simcard_error("PIN set and verification failed\n");
		return -EINVAL;
	}

    simcard_debug("PIN set and verified successfully\n");

    return 0;
}

static inline int simcard_is_pin_needed(uint8_t *hdr, size_t hlen)
{
    if (hdr[SIM_FILE_TYPE_OFFSET] != SIM_FILE_TYPE_EF) /* This logic is only for MF and DF files */
    {
        if((hlen > SIM_CHV1_OFFSET) && 
            !(hdr[SIM_CHV1_OFFSET] & SIM_CHV1_FLAG))
        {
            return 1;
        }
    }
    return 0;
}

static int simcard_set_fd_active(int index)
{
    simio_fd_ctx_t *fd = NULL;
    simio_file_ctx_t *fl = NULL;
    SIMIO_ID_t sim_id;
    int rt;
    uint8_t respond[256] = {};
    size_t respond_size = sizeof(respond);
    int pin_id;

    fd = &(simio_lib_ctx->fd[index]);
    if (!fd->owner)
    {
        simcard_error("Using not allocated FD\n");
        return -EINVAL;
    }

    sim_id = fd->owner->sim_id;

    if (simio_lib_ctx->current_fd[sim_id].index == index)
        return 0;

    fl = fd->head;

    while (fl)
    {
        respond_size = sizeof(respond);
        rt = simcard_select_file(sim_id, fl->filename, respond, &respond_size);
        if (rt)
        {
            simcard_error("Failed to selet 0x%04x file with %d error\n", fl->filename, rt);
            goto Error;
        }

        pin_id = simcard_is_pin_needed(respond, respond_size);
        if (pin_id)
        {
            rt = simcard_set_pin(sim_id, pin_id, fl->pin1); /* For MF and DF only PIN1 is acceptable */
            if (rt)
            {
                simcard_error("Failed to set pin %d for 0x%04x file with %d error\n", pin_id, fl->filename, rt);
                goto Error;
            }
        }

        simio_lib_ctx->current_fd[sim_id].ptr_pin1 = fl->pin1;
        simio_lib_ctx->current_fd[sim_id].ptr_pin2 = fl->pin2;
        fl = fl->next;
    }

    simio_lib_ctx->current_fd[sim_id].index = index;
    simio_lib_ctx->current_fd[sim_id].response_size = respond_size;
    memcpy(simio_lib_ctx->current_fd[sim_id].response, respond, respond_size);


    return 0;

Error:

    simio_lib_ctx->current_fd[sim_id].index = -1;
    simio_lib_ctx->current_fd[sim_id].response_size = 0;
    return rt;
}

static inline int simcard_read_chunk(SIMIO_ID_t sim_id, uint8_t *buf,
    size_t len, size_t offset, uint8_t *SW1, uint8_t *SW2)
{
    int ret;
    uint8_t cmd[5] = { SIM_CMD_READ };
    simio_ioctl_arg_t arg = {};

    simcard_debug("SIM %d read %u bytes from offset %u\n", sim_id, len, offset);

    if (len > SIM_CARD_MAX_CHUNK_SIZE)
    {
        simcard_error("The chunk size is too big %u (%u)\n", len, SIM_CARD_MAX_CHUNK_SIZE);
        return -EINVAL;
    }

    cmd[2] = (offset >> 8) & 0xFF;
    cmd[3] = offset & 0xFF;
    cmd[4] = len;
    
    arg.data.io.tx_len = sizeof(cmd);
    memcpy(arg.data.io.data, cmd, sizeof(cmd));

    ret = __simcard_ioctl(SIMIO_READ, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Read command failed %d \n", arg.ret);
        return arg.ret;
    }

    arg.data.io.rx_len -=2;

    memcpy(buf, arg.data.io.data, arg.data.io.rx_len);
    *SW1 = arg.data.io.data[arg.data.io.rx_len];
    *SW2 = arg.data.io.data[arg.data.io.rx_len + 1];

    return arg.data.io.rx_len;
}

static inline int simcard_write_chunk(SIMIO_ID_t sim_id, uint8_t *buf, size_t len, size_t offset,
    uint8_t *SW1, uint8_t *SW2)
{
    int ret;
    uint8_t cmd[5] = { SIM_CMD_WRITE };
    simio_ioctl_arg_t arg = {};

    simcard_debug("SIM %d wrtite %u bytes to offset %u\n", sim_id, len, offset);

    if (len > SIM_CARD_MAX_CHUNK_SIZE)
    {
        simcard_error("The chunk size is too big %u (%u)\n", len, SIM_CARD_MAX_CHUNK_SIZE);
        return -EINVAL;
    }

    cmd[2] = (offset >> 8) & 0xFF;
    cmd[3] = offset & 0xFF;
    cmd[4] = len;
    
    arg.data.io.tx_len = sizeof(cmd) + len;
    memcpy(arg.data.io.data, cmd, sizeof(cmd));
    memcpy(arg.data.io.data+sizeof(cmd), buf, len);

    ret = __simcard_ioctl(SIMIO_WRITE, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Read command failed %d \n", arg.ret);
        return arg.ret;
    }
    
    *SW1 = arg.data.io.data[0];
    *SW2 = arg.data.io.data[1];

    return 0;
}

static inline int sw_to_os(uint8_t SW1, uint8_t SW2)
{
    int rt = 0;
    if (SW1 == 0x65)
    {
        simcard_error("Memory Failure\n");
        rt = -EIO;
        goto Exit;
    }
    else if (SW1 == 0x67 || SW1 == 0x6C)
    {
        simcard_error("Wrong len is specified\n");
        rt = -EINVAL;
        goto Exit;
    }
    else if (SW1 == 0x6B)
    {
        simcard_error("The offset is outside the limit\n");
        rt = -EINVAL;
        goto Exit;
    }
    else if (SW1 == 0x69)
    {
        if (SW2 == 0x81)
        {
            simcard_error("Command incompatible with file structure\n");
            rt = -EINVAL;
        }
        else if (SW2 == 0x82)
        {
            simcard_error("Security status not satisfied\n");
            rt = -EACCES;
        }
        else if (SW2 == 0x86)
        {
            simcard_error("Command not allowed\n");
            rt = -EINVAL;    
        }
        goto Exit;
    }
    else if (SW1 == 0x6A)
    {
        if (SW2 == 0x81)
        {
            simcard_error("Command not supported\n");
            rt = -ENOTSUP;
        }
        else if (SW2 == 0x82)
        {
            simcard_error("File not found\n");
            rt = -ENODEV;
        }
        goto Exit;
    }

Exit:
    return rt;
}

/*
 * Public functions
 */

/*******************************************************************************
 *
 * Function: bcm_simio_lib_init
 *
 * Description: initialize the library internal structures
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/

int bcm_simio_lib_init(void)
{
    if (simio_lib_ctx)
    {
        simio_lib_ctx->ref_cnt++;
        return 0;
    }

    simio_lib_ctx = calloc(1, sizeof(simio_lib_ctx_t));
    if (!simio_lib_ctx)
    {
        simcard_error("Failed to allocate internal context\n");
        return -ENOMEM;
    }

    simio_lib_ctx->ref_cnt = 1;
    simio_lib_ctx->current_fd[0].index = -1;
    simio_lib_ctx->current_fd[1].index = -1;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_uninit
 *
 * Description: uninitialize the library internal structures. 
 *
 * Return value: None 
 *******************************************************************************/
void bcm_simio_lib_uninit(void)
{
    if (!simio_lib_ctx)
    {
        simcard_error("duplicate uninit - library already cleared\n");
        return;
    }

    simio_lib_ctx->ref_cnt--;

    if (simio_lib_ctx->ref_cnt == 0)
    {
        free(simio_lib_ctx);
        simio_lib_ctx = NULL;
    }
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_register_app
 *
 * Parameters:
 *            sim_id  - (IN) sim controller ID.
 *            app_ctx - (IN) pointer to application context.
 *            cbs     - (IN) pointer to application callbacks struct.
 *            lib_ctx - (OUT) pointer to lib context.
 *
 * Description: register application with app_ctx on controller
 *             sim_id with cbs callbacks and will return the private context in lib_ctx. 
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_register_app(SIMIO_ID_t sim_id, void* app_ctx, bcm_simio_app_cbs_t *cbs, void **lib_ctx)
{
    simio_app_ctx_t *new;

    if(!simio_lib_ctx)
    {
        simcard_error("Library is not initialized yet\n");
        return -ENODEV;

    }

    new = calloc(1, sizeof(simio_app_ctx_t));
    if (!new)
    {
        simcard_error("Failed to allocate internal application context\n");
        return -ENOMEM;
    }

    new->sim_id = sim_id;
    new->app_ctx = app_ctx;
    memcpy(&new->cbs, cbs, sizeof(bcm_simio_app_cbs_t));

    new->next = simio_lib_ctx->app_list[sim_id];
    simio_lib_ctx->app_list[sim_id] = new;

    *lib_ctx = new;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_unregister_app
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *
 * Description: Unregister current application.
 *
 * Return value: None
 *
 *******************************************************************************/
void bcm_simio_lib_unregister_app(void *lib_ctx)
{
    SIMIO_ID_t sim_id;
    simio_app_ctx_t *app_ctx = (simio_app_ctx_t *)lib_ctx;
    simio_app_ctx_t **iterr;

    if(!simio_lib_ctx)
    {
        simcard_error("Library is not initialized yet\n");
        return;
    }

    if (!app_ctx)
    {
        simcard_error("Lib context is NULL\n");
        return;
    }

    sim_id = app_ctx->sim_id;

    if (simio_lib_ctx->app_list[sim_id] == NULL)
    {
        simcard_error("Lib context is garbage\n");
        return;
    }

    for (iterr = &(simio_lib_ctx->app_list[sim_id]); *iterr; iterr=&((*iterr)->next))
    {
        if (*iterr == app_ctx)
        {
            *iterr = app_ctx->next;
            break;
        }
    }

    free(app_ctx);
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_activate
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            size -    (IN)/(OUT) size of preallocated buffer, size of respond
 *            data -    (OUT) pointer to preallocated buffer.
 *
 * Description: This API will activate simcard and return the card respond.
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_activate(void *lib_ctx, size_t *size, void *data)
{
    simio_ioctl_arg_t arg = {};
    int ret;
    simio_app_ctx_t *ctx = (simio_app_ctx_t *)lib_ctx;

    if (*size < SIM_CARD_MAX_BUFFER_SIZE && data)
    {
        simcard_error("Buffer size is too small shoud be at least %d \n", SIM_CARD_MAX_BUFFER_SIZE);
        return -EINVAL;
    }

    arg.sim_id = ctx->sim_id;
    arg.data.active.len = *size;
    
    ret = __simcard_ioctl(SIMIO_ACTIVATE, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Activate failed %d \n", arg.ret);
        return arg.ret;
    }

    *size = arg.data.active.len;

    if (data)
        memcpy(data, arg.data.active.data, *size);
    
    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_is_online
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            data -    (OUT) pointer to uint32_t.
 *
 * Description: This API will return the card current status 0 or 1.
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_is_online(void *lib_ctx, uint32_t *data)
{
    simio_ioctl_arg_t arg = {};
    int ret;
    simio_app_ctx_t *ctx = (simio_app_ctx_t *)lib_ctx;

    arg.sim_id = ctx->sim_id;

    if (!data)
    {
        simcard_error("data could not be NULL \n");
        return -EINVAL;
    }

    ret = __simcard_ioctl(SIMIO_IS_ONLINE, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Is Online failed %d \n", arg.ret);
        return arg.ret;
    }

    *data = arg.data.detection_status;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_set_baudrate
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            F -       (IN) F value.
 *            D -       (IN) D value.
 *
 * Description: This API allow to set baud rate F(372,512) D(1,8,16,32,64) 
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_set_baudrate(void *lib_ctx, uint32_t F, uint32_t D)
{
    simio_ioctl_arg_t arg = {};
    int ret;
    simio_app_ctx_t *ctx = (simio_app_ctx_t *)lib_ctx;

    arg.sim_id = ctx->sim_id;
    arg.data.baud_rate.F = F;
    arg.data.baud_rate.D = D;

    ret = __simcard_ioctl(SIMIO_SET_BAUDRATE, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Set Baudrate failed %d \n", arg.ret);
        return arg.ret;
    }

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_set_protocol
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            prot -    (IN) protocol value.
 *
 * Description: This API allow to set a working protocol T0 or T1  
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_set_protocol(void *lib_ctx, PROTOCOL_t prot)
{
    simio_ioctl_arg_t arg = {};
    int ret;
    simio_app_ctx_t *ctx = (simio_app_ctx_t *)lib_ctx;

    arg.sim_id = ctx->sim_id;
    arg.data.protocol = prot;

    ret = __simcard_ioctl(SIMIO_SET_PROTOCOL, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Set Protocol failed %d \n", arg.ret);
        return arg.ret;
    }

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_set_control
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            control - (IN) control value.
 *
 * Description: This API allow to user to deactivate and activate the card (0,1)  
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_set_control(void *lib_ctx, uint32_t control)
{
    simio_ioctl_arg_t arg = {};
    int ret;
    simio_app_ctx_t *ctx = (simio_app_ctx_t *)lib_ctx;

    arg.sim_id = ctx->sim_id;
    arg.data.control = control;

    ret = __simcard_ioctl(SIMIO_SET_CONTROL, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("Set Control failed %d \n", arg.ret);
        return arg.ret;
    }

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_reset
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            reset -   (IN) to do reset or not.
 *            freq -    (IN) new frequency value
 *            volt -    (IN) nre voltage value
 *
 * Description: This API allow to user reset the simcard to new frequncy and voltage value  
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_reset(void *lib_ctx, uint32_t reset, SIMIO_DIVISOR_t freq, SimVoltageLevel_t volt)
{
    simio_ioctl_arg_t arg = {};
    int ret;
    simio_app_ctx_t *ctx = (simio_app_ctx_t *)lib_ctx;

    arg.sim_id = ctx->sim_id;
    arg.data.reset.reset = reset;
    arg.data.reset.freq = freq;
    arg.data.reset.voltage = volt;

    ret = __simcard_ioctl(SIMIO_RESET, (uint32_t)&arg);
    if (ret)
    {
        simcard_error("IOCTL failed %d \n", ret);
        return ret;
    }

    if (arg.ret)
    {
        simcard_error("RESET failed %d \n", arg.ret);
        return arg.ret;
    }

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_open
 *
 * Parameters:
 *            lib_ctx - (IN) pointer to lib context.
 *            file -    (IN) Master file id.
 *            pin1 -    (IN) PIN 1 code for restricted files (optional)
 *            pin2 -    (IN) PIN 2 code for restricted files (optional)
 *
 * Description: Prepare 'file' for access. The file could be a directory.
 *              This will set active file to specified one.
 *
 * Return value: <0 on error, >=0 as a file descriptor
 *
 *******************************************************************************/
int bcm_simio_lib_open(void *lib_ctx, uint16_t file, const char* pin1, const char* pin2)
{
    int i;
    simio_fd_ctx_t *fd = NULL;

    for (i = 0; i < SIMCARD_MAX_FD && simio_lib_ctx->fd[i].owner; i++)
        ;

    if (i == SIMCARD_MAX_FD)
    {
        simcard_error("Too many open files\n");
        return -EMFILE;
    }

    fd = &simio_lib_ctx->fd[i];

    fd->head = malloc(sizeof(simio_file_ctx_t));
    if (!fd->head)
    {
        simcard_error("Unable to create file instance\n");
        return -ENOMEM;
    }

    memset(fd->head, 0, sizeof(simio_file_ctx_t));

    fd->tail = &(fd->head->next); 

    fd->owner = lib_ctx;
    fd->head->filename = file;

    if (pin1)
    {
        fd->head->pin1 = strdup(pin1);
        if (!fd->head->pin1)
        {
            simcard_error("No memory to save pin1\n");
            goto Error;
        }
    }

    if (pin2)
    {
        fd->head->pin2 = strdup(pin2);
        if (!fd->head->pin2)
        {
            simcard_error("No memory to save pin2\n");
            goto Error;
        }
    }
    
    return i;
Error:
    if (fd->head->pin1)
        free(fd->head->pin1);
    free(fd->head);
    fd->head = NULL;
    fd->tail = NULL;

    fd->owner = NULL;

    return -ENOMEM;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_change_file
 *
 * Parameters:
 *            index -      (IN) file descriptor to change.
 *            file -    (IN) File ID
 *            pin1 -    (IN) PIN 1 code for restricted files (optional)
 *            pin2 -    (IN) PIN 2 code for restricted files (optional)
 *
 * Description: Change specific file descriptor to point to new location.
 *              The file could be a directory.
 *              This will also change the active file to the new one.
 *
 * Return value: <0 on error, >=0 as a file descriptor
 *
 *******************************************************************************/
int bcm_simio_lib_change_file(int index, uint16_t file, const char* pin1, const char* pin2)
{
    simio_fd_ctx_t *fd = NULL;
    simio_file_ctx_t *fl = NULL;
    SIMIO_ID_t sim_id;

    fd = &simio_lib_ctx->fd[index];
    if (!fd->owner)
    {
        simcard_error("Using not allocated FD\n");
        return -EINVAL;
    }
    
    fl = malloc(sizeof(simio_file_ctx_t));
    if(!fl)
    {
        simcard_error("Unable to create file instance\n");
        goto Error;
    }

    memset(fl, 0, sizeof(simio_file_ctx_t));

    fl->filename = file;

    if (pin1)
    {
        fl->pin1 = strdup(pin1);
        if (!fl->pin1)
        {
            simcard_error("No memory to save pin1\n");
            goto Error;
        }
    }

    if (pin2)
    {
        fl->pin2 = strdup(pin2);
        if (!fl->pin2)
        {
            simcard_error("No memory to save pin2\n");
            goto Error;
        }
    }

    *fd->tail = fl;
    fd->tail = &(fl->next);

    sim_id = fd->owner->sim_id;

    if (simio_lib_ctx->current_fd[sim_id].index == index)
    {
        simio_lib_ctx->current_fd[sim_id].index = -1;
        simio_lib_ctx->current_fd[sim_id].response_size = 0;
    }

    return index;

Error:
    if (fl->pin1)
        free(fl->pin1);

    if (fl)
        free(fl);

    return -ENOMEM;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_close
 *
 * Parameters:
 *            index -      (IN) file descriptor to close.
 *
 * Description: Remove the specified fd from internal table.
 *              This will also change the active file to the root. 
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_close(int index)
{
    simio_fd_ctx_t *fd = NULL;
    simio_file_ctx_t *fl = NULL;

    fd = &simio_lib_ctx->fd[index];
    if (!fd->owner)
    {
        simcard_error("Using not allocated FD\n");
        return -EINVAL;
    }

    fl = fd->head;

    while (fl)
    {
        if (fl->pin2)
            free(fl->pin2);

        if (fl->pin1)
            free(fl->pin1);

        fd->head = fl->next;
        free(fl);

        fl = fd->head;
    }

    fd->owner = NULL;
    fd->head = NULL;
    fd->tail = NULL;

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_get_select_response
 *
 * Parameters:
 *            fd -      (IN) file descriptor.
 *            data -    (IN)/(OUT) pointer to preallocated buffer.
 *            len -    (IN)/(OUT) size of preallocated buffer, size of respond
 *
 * Description: This API will return the respond of the last select command.
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
int bcm_simio_lib_get_select_response(int fd, uint8_t *data, size_t *len)
{
    int rt;
    SIMIO_ID_t sim_id;

    if (!data)
    {
        simcard_error("No buffer to store data provided\n");
        return -EINVAL;
    }

    if (*len < SIM_CARD_MAX_BUFFER_SIZE)
    {
        simcard_error("Buffer size is too small shoud be at least %d \n", SIM_CARD_MAX_BUFFER_SIZE);
        return -EINVAL;
    }

    rt = simcard_set_fd_active(fd);
    if (rt)
    {
        simcard_error("Failed to set fd %d as active\n", fd);
        return rt;
    }

    sim_id = simio_lib_ctx->fd[fd].owner->sim_id;

    *len = simio_lib_ctx->current_fd[sim_id].response_size;

    memcpy(data, simio_lib_ctx->current_fd[sim_id].response, *len);

    return 0;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_read
 *
 * Parameters:
 *            fd -      (IN) file descriptor to read.
 *            buffer -  (IN) pointer to buffer to store data in.
 *            offset -  (IN) offset in the file to read from.
 *                          (0 - from the beginning of the file,
 *                           -1 - from current position, could be also beginning of the file,
 *                           any other - offset in bytes)
 *            size -    (IN) size to read in bytes.
 *            timeout - (IN) operation timeout.
 *
 * Description: Read the specified number of bytes starting from specified offset from specified
 *              file and store it in specified buffer. Buffer has to be user allocated and to be
 *              big enough to store needed amount of data. The read operation is blocking.
 *
 * Return value: <0 on error, >0 the size of successfully read data 
 *
 *******************************************************************************/
size_t bcm_simio_lib_read(int fd, uint8_t* buffer, size_t offset, size_t size, uint32_t timeout)
{
    int rt;
    size_t actual_size = 0;
    int tmp, readed;
    SIMIO_ID_t sim_id;
    uint8_t SW1, SW2;
    uint8_t pin_id = 0;

    if (!buffer)
    {
        simcard_error("No buffer to store data provided\n");
        return -EINVAL;
    }

    rt = simcard_set_fd_active(fd);
    if (rt)
    {
        simcard_error("Failed to set fd %d as active\n", fd);
        return rt;
    }


    sim_id = simio_lib_ctx->fd[fd].owner->sim_id;
    
    pin_id = SIM_FILE_READ_COND(simio_lib_ctx->current_fd[sim_id].response[SIM_FILE_ACCESS_COND_OFFSET]);
    if (pin_id)
    {
        char *pin = (pin_id == SIM_FILE_ACCESS_CHV1) ? 
            simio_lib_ctx->current_fd[sim_id].ptr_pin1 : simio_lib_ctx->current_fd[sim_id].ptr_pin2;

        rt = simcard_set_pin(sim_id, pin_id, pin);
        if (rt)
        {
            simcard_error("Failed to set pin %d for READ with %d error\n", pin_id, rt);
            goto Exit;
        }
    }

    simcard_debug("Going to read %u bytes from offest %d from fd %d on simcard %d\n", size, offset, fd, sim_id);

    while (size)
    {
        tmp = size > SIM_CARD_MAX_CHUNK_SIZE ? SIM_CARD_MAX_CHUNK_SIZE : size;

        readed = simcard_read_chunk(sim_id, buffer + actual_size, tmp, offset, &SW1, &SW2);
        if (readed < 0)
        {
            simcard_error("Failed to read %d bytes readed till now %u from [%u] result %d\n",
                tmp, actual_size, size, readed);
            rt = readed;
            goto Exit;
        }

        simcard_debug("Readed %u bytes from offest %d from fd %d on simcard %d SW1 0x%x SW2 0x%x\n",
            readed, offset, fd, sim_id, SW1, SW2);

        actual_size += readed;
        size -= readed;
        
        rt = sw_to_os(SW1, SW2);
        if (rt)
            goto Exit;

        if (readed < tmp)
        {
            dump_data(buffer, actual_size);
            rt = actual_size;
            goto Exit;
        }

        offset = 0; /* Only on first read the offset is needed */
    }
    rt = actual_size;

Exit:
    return rt;
}

/*******************************************************************************
 *
 * Function: bcm_simio_lib_write
 *
 * Parameters:
 *            fd -      (IN) file descriptor to write.
 *            buffer -  (IN) pointer to buffer with data.
 *            offset -  (IN) offset in the file to read from.
 *                          (0 - from the beginning of the file,
 *                           -1 - from current position, could be also beginning of the file,
 *                           any other - offset in bytes)
 *            size -    (IN) size to write in bytes.
 *            timeout - (IN) operation timeout.
 *
 * Description: Write the specified number of bytes starting from specified offset to specified
 *              file from specified buffer. The write operation is blocking. 
 *
 * Return value: <0 on error, 0 on success
 *
 *******************************************************************************/
size_t bcm_simio_lib_write(int fd, uint8_t* buffer, size_t offset, size_t size, uint32_t timeout)
{
    int rt;
    size_t actual_size = 0;
    int tmp;
    SIMIO_ID_t sim_id;
    uint8_t SW1, SW2;
    uint8_t pin_id = 0;

    if (!buffer)
    {
        simcard_error("No data buffer provided\n");
        return -EINVAL;
    }

    rt = simcard_set_fd_active(fd);
    if (rt)
    {
        simcard_error("Failed to set fd %d as active\n", fd);
        return rt;
    }

    sim_id = simio_lib_ctx->fd[fd].owner->sim_id;

    pin_id = SIM_FILE_UPDATE_COND(simio_lib_ctx->current_fd[sim_id].response[SIM_FILE_ACCESS_COND_OFFSET]);
    if (pin_id)
    {
        char *pin = (pin_id == SIM_FILE_ACCESS_CHV1) ? 
            simio_lib_ctx->current_fd[sim_id].ptr_pin1 : simio_lib_ctx->current_fd[sim_id].ptr_pin2;

        rt = simcard_set_pin(sim_id, pin_id, pin);
        if (rt)
        {
            simcard_error("Failed to set pin %d for UPDATE with %d error\n", pin_id, rt);
            goto Exit;
        }
    }

    simcard_debug("Going to write %u bytes from offest %d to fd %d on simcard %d\n", size, offset, fd, sim_id);

    while (size)
    {
        tmp = size > SIM_CARD_MAX_CHUNK_SIZE ? SIM_CARD_MAX_CHUNK_SIZE : size;

        rt = simcard_write_chunk(sim_id, buffer + actual_size, tmp, offset, &SW1, &SW2);
        if (rt < 0)
        {
            simcard_error("Failed to write %d bytes writer till now %u from [%u] result %d\n",
                tmp, actual_size, size, rt);
            goto Exit;
        }
        rt = sw_to_os(SW1, SW2);
        if (rt)
            goto Exit;

        actual_size += tmp;
        size -= tmp;

        offset = 0; /* Only on first write the offset is needed */
    }

Exit:
    return rt;
}

