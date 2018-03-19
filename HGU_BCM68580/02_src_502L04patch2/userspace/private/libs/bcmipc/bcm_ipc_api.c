/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:standard

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
#define USE_LINT


#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "bcm_ipc_api.h"


//#define CC_BCM_IPC_API_DEBUG


#if defined(CC_BCM_IPC_API_DEBUG)
#define bcmIpcApi_debug(fmt, arg...) printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define bcmIpcApi_debug(fmt, arg...)
#endif

#define bcmIpcApi_error(fmt, arg...) printf("ERROR[%s.%u]: " fmt, __FUNCTION__, __LINE__, ##arg)

/*
 *  Global variables
 */

/* Resource allocation mutex */
pthread_mutex_t bcmos_res_lock = PTHREAD_MUTEX_INITIALIZER;

/* Message queues */
static STAILQ_HEAD(msg_queue_list, bcmIpcMsgQueue_t) msg_queue_list = STAILQ_HEAD_INITIALIZER(msg_queue_list);


/** "send" to socket */
static int _bcm_ipc_socket_send(bcmIpcMsgQueue_t *queue, UINT8 *buf, UINT32 buf_length)
{
    int rc;

    /* Message queue without remote endpoint address doesn't support transmit */
    if (!queue->ep_extra_data_size)
        return -EINVAL;

    bcmIpcApi_debug(" queue %p  buf %p buf_len %d\n\n", queue, buf, buf_length);
    rc = sendto(queue->ep, buf, buf_length, 0,
        (struct sockaddr *)queue->ep_extra_data, queue->ep_extra_data_size);
    bcmIpcApi_debug(" Message sent - error code %s\n", strerror(errno));
    return (rc == buf_length) ? 0 : -ENOENT;
}

/** "recv" from socket */
static int _bcm_ipc_socket_recv(bcmIpcMsgQueue_t *queue, UINT32 timeout, UINT8 **buf, UINT32 *buf_length)
{
    int rc;
    int wait = 0;

    /* Message queue without local endpoint address doesn't support receive */
    if (!queue->q.parm.local_ep_address)
        return -ENOENT;
    bcmIpcApi_debug (" queue->recv_buf=%p \n", queue->recv_buf);

    if (timeout && timeout != BCM_IPC_WAIT_FOREVER)
    {
        fd_set read_fds;
        struct timeval tv;

        bcmIpcApi_debug (" Timeout = %d\n", timeout);
        FD_ZERO(&read_fds);
        FD_SET(queue->ep, &read_fds);
        tv.tv_sec = timeout / 1000000;
        tv.tv_usec = (timeout % 1000000) * 1000;
        bcmIpcApi_debug (" Before select() queue->ep = %ld\n", queue->ep);
        rc = select(queue->ep + 1, &read_fds, NULL, NULL, &tv);
        bcmIpcApi_debug (" After select() rc = %d\n", rc);

        if (rc < 0)
        {
            return -ENONET;
        }
        if (!rc || !FD_ISSET(queue->ep, &read_fds))
            return -EAGAIN;
        wait = MSG_DONTWAIT;
    }
    rc = 0;
    rc = recv(queue->ep, queue->recv_buf, queue->q.parm.max_mtu, wait);
    bcmIpcApi_debug (" Back from recv() rc=%d\n", rc);
    if (rc < 0)
    {
        return -ENONET;
    }
    if (rc == 0)
        return -ENOENT;

    *buf = queue->recv_buf;
    *buf_length = rc;

    return 0;
}

static int _bcm_ipc_socket_close(bcmIpcMsgQueue_t *queue)
{
    /* Close domain socket */
    if (queue->ep > 0)
    {
        close(queue->ep);
    }
    if (queue->ep_extra_data)
    {
        free(queue->ep_extra_data);
    }
    return 0;
}

/* Pack / unpack message header.
 * In the future we might adopt bcmolt_buf service into OS abstraction and use it
 * for packing / unpacking.
 */

static void bcm_ipc_msg_hdr_pack(const bcmIpcMsg_t *msg, uint8_t *buf, UINT32 data_size)
{
    UINT32 val32_1;
    UINT32 val32_2;

    val32_1 = htonl(msg->type);
    memcpy(buf, &val32_1, sizeof(val32_1));
    buf[4] = (UINT8)msg->instance;
    buf[5] = (UINT8)msg->sender;
    val32_2 = htonl(data_size);
    memcpy(&buf[6], &val32_2, sizeof(val32_2));
}

static void bcm_ipc_msg_hdr_unpack(const UINT8 *buf, bcmIpcMsg_t *msg)
{
    uint32_t val32_1;
    uint32_t val32_2;

    memcpy(&val32_1, buf, sizeof(val32_1));
    val32_1 = ntohl(val32_1);
    msg->type = (bcmIpcMsgId_t)val32_1;
    msg->instance = (bcmIpcMsgInstance_t)buf[4];
    msg->sender = (bcmIpcModuleId_t)buf[5];
    memcpy(&val32_2, &buf[6], sizeof(val32_2));
    msg->size = ntohl(val32_2); /* can be overwritten by unpacker */

    msg->handler = NULL;
    msg->send_flags = 0;
}

/* Remote queue support - common part of create/destroy */

/* Pack message for over-the-socket transmission.
 * This function is good for case when both queue ends are on the same CPU
 * and there is no need to do any translation.
 */
static int _bcm_ipc_transparent_pack(bcmIpcMsgQueue_t *queue, bcmIpcMsg_t *msg, UINT8 **buf, UINT32 *buf_length)
{
    UINT32 size = BCM_IPC_MSG_HDR_SIZE + msg->size;

    if (size > queue->q.parm.max_mtu)
    {
        bcmIpcApi_error( "Attempt to send message longer than configured max_mtu %u > %u\n",
            size, queue->q.parm.max_mtu);
        return -EINVAL;
    }
    bcm_ipc_msg_hdr_pack(msg, queue->send_buf, msg->size);
    if (msg->size)
    {
        if (msg->data == NULL)
        {
            bcmIpcApi_error( "Data pointer is NULL\n");
            return -EINVAL;
        }
        memcpy(queue->send_buf + BCM_IPC_MSG_HDR_SIZE, msg->data, msg->size);
    }
    *buf = queue->send_buf;
    *buf_length = size;
    return 0;
}

/** "unpack" message. In case of domain socket both queue ends are in the same CPU.
 * Message is sent as-is
 */
static int _bcm_ipc_transparent_unpack(bcmIpcMsgQueue_t *queue, UINT8 *buf, UINT32 buf_length, bcmIpcMsg_t **msg)
{
    bcmIpcMsg_t *m;
    int i;

    if (buf_length < BCM_IPC_MSG_HDR_SIZE)
    {
        bcmIpcApi_error( "Received message is too short (%u)\n", buf_length);
        return -EINVAL;
    }

    /* Adjust buf_length to account for difference in packed and unpacked message header sizes */
    buf_length -= BCM_IPC_MSG_HDR_SIZE;
    buf_length += sizeof(bcmIpcMsg_t);

    m = ( bcmIpcMsg_t *)malloc(buf_length);
    if (!m)
    {
        bcmIpcApi_error( "Received message discarded: %u bytes\n", buf_length);
        return -EINVAL;
    }
    bcmIpcApi_debug (" In _bcm_ipc_transparent_unpack() - buffer\n");
    for(i=0; i<sizeof (bcmIpcMsg_t); i++)
    {
        bcmIpcApi_debug(" 0x%02x ", buf[i]);
    }
    bcmIpcApi_debug("\n");
    bcm_ipc_msg_hdr_unpack((UINT8 *)buf, m);
    m->release = NULL;
    m->data_release = NULL;
    if (m->size != buf_length - sizeof(bcmIpcMsg_t))
    {
        bcmIpcApi_error("Received message is insane. Expected data length %u, got %u\n",
            m->size, buf_length - sizeof(bcmIpcMsg_t));
        free(m);
        return -EINVAL;
    }
    bcmIpcApi_debug (" In _bcm_ipc_transparent_unpack() m->size = %d\n", m->size);
    if (m->size)
    {
        m->data = m->start = (void *)(m + 1);
        memcpy(m->data, &buf[BCM_IPC_MSG_HDR_SIZE], m->size);
    }
    else
    {
        m->data = m->start = NULL;
    }
    bcmIpcApi_debug (" In _bcm_ipc_transparent_unpack() m->data = %p\n", m->data);
    *msg = m;

    return 0;
}

/* Domain-socket-based inter-process communication */
static int bcm_ipc_msg_queue_domain_socket_open(bcmIpcMsgQueue_t *queue)
{
    struct sockaddr_un sa;

    /* Open domain socket */
    queue->ep = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (queue->ep < 0)
    {
        bcmIpcApi_error("Can't create domain socket. error %s\n", strerror(errno));
        return errno;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_UNIX;

    /* If local_ep_address is set - the queue supports receive */
    if (queue->q.parm.local_ep_address)
    {
        bcmIpcApi_debug("1) In q.parm.local_ep_address handling\n");
        strncpy(sa.sun_path, queue->q.parm.local_ep_address, sizeof(sa.sun_path) - 1);
        /* In linux path can start from 0 character */
        if (!sa.sun_path[0])
        {
            strncpy(&sa.sun_path[1], &queue->q.parm.local_ep_address[1], sizeof(sa.sun_path) - 1);
        }
        if (bind(queue->ep, (struct sockaddr *)&sa, sizeof(sa)) < 0)
        {
            int err = errno;
            close(queue->ep);
            bcmIpcApi_error("Can't bind domain socket to %s. error %s\n",
                queue->q.parm.local_ep_address, strerror(err));
            return err;
        }
    }

    /* If remote_ep_address is set - the queue supports transmit */
    if (queue->q.parm.remote_ep_address)
    {
        bcmIpcApi_debug("2) In q.parm.remote_ep_address handling %s  %s\n", queue->q.parm.remote_ep_address, &queue->q.parm.remote_ep_address[1]);
        queue->ep_extra_data = malloc(sizeof(struct sockaddr_un));
        if (!queue->ep_extra_data)
        {
            close(queue->ep);
            return -ENOMEM;
        }
        strncpy(sa.sun_path, queue->q.parm.remote_ep_address, sizeof(sa.sun_path) - 1);
        /* In linux path can start from 0 character */
        if (!sa.sun_path[0])
        {
            strncpy(&sa.sun_path[1], &queue->q.parm.remote_ep_address[1], sizeof(sa.sun_path) - 1);
        }
        memcpy(queue->ep_extra_data, &sa, sizeof(sa));
        queue->ep_extra_data_size = sizeof(sa);
    }

    /* Set callbacks */
    queue->q.parm.close = _bcm_ipc_socket_close;
    queue->q.parm.send = _bcm_ipc_socket_send;
    queue->q.parm.recv = _bcm_ipc_socket_recv;
    if (!queue->q.parm.pack)
        queue->q.parm.pack = _bcm_ipc_transparent_pack;
    if (!queue->q.parm.unpack)
        queue->q.parm.unpack = _bcm_ipc_transparent_unpack;

    return 0;
}

/* UDP-socket-based inter-process communication */
static int _bcm_ipc_parse_ip_port(const char *s, struct sockaddr_in *sa)
{
    uint32_t ip;
    int n;
    uint32_t ip1, ip2, ip3, ip4, port;

    n = sscanf(s, "%u.%u.%u.%u:%u", &ip1, &ip2, &ip3, &ip4, &port);
    if (n != 5 || ip1 > 0xff || ip2 > 0xff || ip3 > 0xff || ip4 > 0xff || port > 0xffff)
    {
         bcmIpcApi_error("Can't parse %s. Must be ip_address:port\n", s);
         return -EINVAL;
    }
    ip = (ip1 << 24) | (ip2 << 16) | (ip3 << 8) | ip4;
    bcmIpcApi_debug(" ip=0x%x port=%d\n", ip, port);
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    sa->sin_addr.s_addr = htonl(ip);

    bcmIpcApi_debug(" ip=0x%x port=%d   sa->sin_addr.s_addr=0x%x sa->sin_port=0x%x\n", ip, port,sa->sin_addr.s_addr,sa->sin_port);
    return 0;
}

static int bcm_ipc_msg_queue_udp_socket_open(bcmIpcMsgQueue_t *queue)
{
    struct sockaddr_in sa;
    int rc;
    UINT8 *p;

    /* Open UDP socket */
    queue->ep = socket(AF_INET, SOCK_DGRAM, 0);
    if (queue->ep < 0)
    {
        bcmIpcApi_error("Can't create UDP socket. error %s\n", strerror(errno));
        return errno;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    /* If local_ep_address is set - the queue supports receive */
    if (queue->q.parm.local_ep_address)
    {
        rc = _bcm_ipc_parse_ip_port(queue->q.parm.local_ep_address, &sa);
        if (rc)
            return rc;
        if (!sa.sin_addr.s_addr)
            sa.sin_addr.s_addr = INADDR_ANY;
        if (bind(queue->ep, (struct sockaddr *)&sa, sizeof(sa)) < 0)
        {
            int err = errno;
            close(queue->ep);
            bcmIpcApi_error("Can't bind UDP socket to %s. error %s\n",
                queue->q.parm.local_ep_address, strerror(err));
            return err;
        }
    }

    /* If remote_ep_address is set - the queue supports transmit */
    if (queue->q.parm.remote_ep_address)
    {

        rc = _bcm_ipc_parse_ip_port(queue->q.parm.remote_ep_address, &sa);
        if (rc)
            return rc;
        queue->ep_extra_data = malloc(sizeof(sa));
        if (!queue->ep_extra_data)
        {
            close(queue->ep);
            return -ENOMEM;
        }
        memcpy(queue->ep_extra_data, &sa, sizeof(sa));
        queue->ep_extra_data_size = sizeof(sa);

        bcmIpcApi_debug(" sa.sin_addr.s_addr=0x%x sa.sin_port=0x%x size = %d\n", sa.sin_addr.s_addr, sa.sin_port, queue->ep_extra_data_size);
        p = queue->ep_extra_data;
        for (rc=0; rc < queue->ep_extra_data_size; rc ++)
            bcmIpcApi_debug(" 0x%02x ", p[rc]);
    }

    /* Set callbacks */
    queue->q.parm.close = _bcm_ipc_socket_close;
    queue->q.parm.send = _bcm_ipc_socket_send;
    queue->q.parm.recv = _bcm_ipc_socket_recv;
    if (!queue->q.parm.pack)
        queue->q.parm.pack = _bcm_ipc_transparent_pack;
    if (!queue->q.parm.unpack)
        queue->q.parm.unpack = _bcm_ipc_transparent_unpack;

    return 0;
}

static int _bcm_ipc_msg_queue_destroy_remote_ep(bcmIpcMsgQueue_t *queue)
{
    if (queue->q.parm.close)
        queue->q.parm.close(queue);
    pthread_mutex_destroy(&queue->send_lock);
    if (queue->send_buf)
        free(queue->send_buf);
    if (queue->recv_buf)
        free(queue->recv_buf);
    return 0;
}

static int _bcm_ipc_msg_queue_create_remote_ep(bcmIpcMsgQueue_t *queue)
{
    int rc = 0;

    /* Allocate tx and rx buffers */
    if (!queue->q.parm.max_mtu)
    {
        queue->q.parm.max_mtu = BCM_IPC_MSG_QUEUE_DEFAULT_MAX_MTU;
    }
    queue->send_buf = malloc(queue->q.parm.max_mtu);
    if (!queue->send_buf)
    {
        bcmIpcApi_error("Can't allocate send_buf\n");
        return -ENOMEM;
    }

    queue->recv_buf = malloc(queue->q.parm.max_mtu);
    if (!queue->recv_buf)
    {
        free(queue->send_buf);
        bcmIpcApi_error( "Can't allocate recv_buf\n");
        return -ENOMEM;
    }
    pthread_mutex_init(&queue->send_lock,NULL);

    switch (queue->q.parm.ep_type)
    {
    case BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET:
        rc = bcm_ipc_msg_queue_domain_socket_open(queue);
        break;

    case BCM_IPC_MSG_QUEUE_EP_UDP_SOCKET:
        rc = bcm_ipc_msg_queue_udp_socket_open(queue);
        break;

    case BCM_IPC_MSG_QUEUE_EP_USER_DEFINED:
        bcmIpcApi_error("Not supported yet\n");
        break;

    default:
        rc = -EINVAL;
        break;
    }

    if (rc)
    {
        _bcm_ipc_msg_queue_destroy_remote_ep(queue);
    }

    return rc;
}

/* Send message to remote EP wrapper */
static int _bcm_ipc_msg_send_to_remote_ep(bcmIpcMsgQueue_t *queue, bcmIpcMsg_t *msg, bcmIpcMsgSendFlags_t flags)
{
    uint8_t *buf = NULL;
    uint32_t buf_length = 0;
    int rc;

    pthread_mutex_lock(&queue->send_lock);
    rc = queue->q.parm.pack(queue, msg, &buf, &buf_length);
    rc = rc ? rc : queue->q.parm.send(queue, buf, buf_length);
    pthread_mutex_unlock(&queue->send_lock);

    /* Release if sent successfully or if auto-release flag is set */
    if (
      (rc == 0 || !(flags & BCM_IPC_MSG_SEND_NO_FREE_ON_ERROR)) &&
      !(flags & BCM_IPC_MSG_SEND_NO_FREE)
       )
    {
        free(msg);
    }
    if (buf && queue->q.parm.free_packed)
    {
        queue->q.parm.free_packed(queue, buf);
    }
    if (rc)
    {
        ++queue->q.stat.msg_discarded;
    }
    else
    {
        ++queue->q.stat.msg_sent;
    }

    return rc;
}

/* Receive message from remote EP wrapper */
static int _bcm_ipc_msg_recv_from_remote_ep(bcmIpcMsgQueue_t *queue, UINT32 timeout, bcmIpcMsg_t **msg)
{
    uint8_t *buf = NULL;
    uint32_t buf_length = 0;
    int rc;

    rc = queue->q.parm.recv(queue, timeout, &buf, &buf_length);
    rc = rc ? rc : queue->q.parm.unpack(queue, buf, buf_length, msg);
    if (buf && queue->q.parm.free_packed)
    {
        queue->q.parm.free_packed(queue, buf);
    }
    if (!rc)
    {
        ++queue->q.stat.msg_received;
        rc = buf_length;
    }

    return rc;
}

/* Create message queue. */
int bcm_ipc_msg_queue_create(bcmIpcMsgQueue_t *queue, const bcmIpcMsgQueueParam_t *parm)
{
    int rc;

    if (!queue || !parm)
    {
        bcmIpcApi_error("queue %p, parm %p\n", queue, parm);
        return -EINVAL;
    }

    memset(queue, 0, sizeof(*queue));
    queue->q.parm = *parm;

    rc = _bcm_ipc_msg_queue_create_remote_ep(queue);

    if (rc)
        return rc;

    queue->magic = BCM_IPC_MSG_QUEUE_VALID;
    /* Copy name to make sure that it is not released - in case it was on the stack */
    if (queue->q.parm.name)
    {
        strncpy(queue->name, queue->q.parm.name, sizeof(queue->name) - 1);
        queue->q.parm.name = queue->name;
    }
    pthread_mutex_lock(&bcmos_res_lock);
    STAILQ_INSERT_TAIL(&msg_queue_list, queue, list);
    pthread_mutex_unlock(&bcmos_res_lock);
    return 0;
}

/* Destroy queue */
int bcm_ipc_msg_queue_destroy(bcmIpcMsgQueue_t *queue)
{
    int rc = 0;

    if (!queue || queue->magic != BCM_IPC_MSG_QUEUE_VALID)
    {
        bcmIpcApi_error("queue handle is invalid\n");
        return -EINVAL;
    }
    queue->magic = BCM_IPC_MSG_QUEUE_DELETED;
    pthread_mutex_lock(&bcmos_res_lock);
    STAILQ_REMOVE(&msg_queue_list, queue, bcmIpcMsgQueue_t, list);
    pthread_mutex_unlock(&bcmos_res_lock);

    rc = _bcm_ipc_msg_queue_destroy_remote_ep(queue);

    return rc;
}

/* Get queue info */
int bcm_ipc_msg_queue_query(const bcmIpcMsgQueue_t *queue, bcmIpcMsgQueueInfo_t *info)
{
    if (!queue || !info)
    {
        bcmIpcApi_error("queue %p, info %p\n", queue, info);
        return -EINVAL;
    }
    info->stat = queue->q.stat;
    info->parm = queue->q.parm;
    return 0;
}

/* Message queue iterator */
int bcm_ipc_msg_queue_get_next(const bcmIpcMsgQueue_t **prev)
{
    const bcmIpcMsgQueue_t *queue;

    if (prev == NULL)
    {
        return -EINVAL;
    }
    queue = *prev;
    if (queue && queue->magic != BCM_IPC_MSG_QUEUE_VALID)
    {
        return -EINVAL;
    }
    if (queue)
    {
        queue = STAILQ_NEXT(queue, list);
    }
    else
    {
        queue = STAILQ_FIRST(&msg_queue_list);
    }
    *prev = queue;
    if (!queue)
    {
        return -ENOENT;
    }
    return 0;
}

/* Send message to queue */
int bcm_ipc_msg_send(bcmIpcMsgQueue_t *queue, bcmIpcMsg_t *msg, bcmIpcMsgSendFlags_t flags)
{
    if (queue->q.parm.ep_type != BCM_IPC_MSG_QUEUE_EP_LOCAL)
    {
        return _bcm_ipc_msg_send_to_remote_ep(queue, msg, flags);
    }
    else
    {
        bcmIpcApi_error("local queues not supported yet\n");
        return -EINVAL;
    }
    return 0;
}

/* Get message from the head of message queue */
int bcm_ipc_msg_recv(bcmIpcMsgQueue_t *queue, uint32_t timeout, bcmIpcMsg_t **msg)
{
    if (!queue || !msg)
    {
        bcmIpcApi_error("queue %p, msg %p\n", queue, msg);
        return -EINVAL;
    }

    if (queue->q.parm.ep_type != BCM_IPC_MSG_QUEUE_EP_LOCAL)
    {
        return _bcm_ipc_msg_recv_from_remote_ep(queue, timeout, msg);
    }
    else
    {
        bcmIpcApi_error("Local queues not supported yet\n");
        return -EINVAL;
    }

    return 0;
}
