/*
 * ipc_test_client.c
 */
//#define IPC_TEST_USE_PTHREAD

#ifdef IPC_TEST_USE_PTHREAD
#include <pthread.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#endif
#include "bcm_ipc_api.h"

#define NUM_MESSAGES    10
#define MSG_TIMEOUT     (2*1000000)


typedef struct ep_struct {
    char type[64];
    char local_ep_name[64];
    char remote_ep_name[64];
} ep_t;

/* Counters */
static int tx_ok, rx_ok, rx_timeout, tx_err, rx_err;

static void *rx_thread_handler(void *data_p)
{
    bcmIpcMsg_t *m;
    int rc=1;
    int dd_prev=-1;
    int i;
    static bcmIpcMsgQueue_t mq2;
    bcmIpcMsgQueueParam_t mqp2;
    ep_t *eps = (ep_t *)data_p;

    memset(&mqp2, 0x0, sizeof(bcmIpcMsgQueueParam_t));
    mqp2.name = "my_msgq_2";
    mqp2.ep_type = (!strcmp(eps->type, "domain")) ?
      BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET : BCM_IPC_MSG_QUEUE_EP_UDP_SOCKET;
    mqp2.local_ep_address = eps->local_ep_name;
    mqp2.remote_ep_address = eps->remote_ep_name;
    rc = bcm_ipc_msg_queue_create(&mq2, &mqp2);
    if (rc)
    {
        printf("1 - bcm_ipc_msg_queue_create() -> %d\n", rc);
        return NULL;
    }


    for (i=0; i<NUM_MESSAGES; i++)
    {
        int dd;
        rc = bcm_ipc_msg_recv(&mq2, MSG_TIMEOUT, &m);
        printf(" In bcm_ipc_msg_recv() rc=%d, size=%d\n", rc, m->size);
        if (rc<0)   /* Buffer length is returned in case of succcess */
        {
#if 0
            if (rc == BCM_ERR_TIMEOUT)
                ++rx_timeout;
            else
#endif
                ++rx_err;
            continue;
        }

        /* Received. Check that data is sane */
        memcpy(&dd, m->data, sizeof(dd));
        if (m->size != sizeof(int) || dd <= dd_prev || dd >= NUM_MESSAGES)
        {
            printf("Received message in insane. Received data is %d, size = %u\n", dd, m->size);
            break;
        }
        dd_prev = dd;
        ++rx_ok;
        free(m);
    }

    bcm_ipc_msg_queue_destroy(&mq2);

    return NULL;
}

int main(int argc, char *argv[])
{
    int rc;
    char *local_ep_name;
    char *remote_ep_name;
    bcmIpcMsgQueueParam_t mqp1;
    bcmIpcMsg_t *m;
#ifdef IPC_TEST_USE_PTHREAD
    pthread_t my_thread;
    pthread_attr_t my_pthread_attr;
#else
    int pid;
    int status;
#endif
    int i;
    static bcmIpcMsgQueue_t mq1;
    ep_t  endpoints;

    memset(&mqp1, 0x0, sizeof(bcmIpcMsgQueueParam_t));

    if (argc != 4 || (strcmp(argv[1], "domain") && strcmp(argv[1], "udp")))
    {
        printf("domain | udp <local_address> <remote_address>\n");
        return -1;
    }

    /* Create message queue with domain or UDP socket EP */
    mqp1.name = "my_msgq_1";
    mqp1.ep_type = (!strcmp(argv[1], "domain")) ? 
      BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET : BCM_IPC_MSG_QUEUE_EP_UDP_SOCKET;
    local_ep_name = argv[2];
    remote_ep_name = argv[3];

    /* in linux file name starting with 0 makes it virtual */
#if 0
    if (mqp.ep_type == BCM_IPC_MSG_QUEUE_EP_DOMAIN_SOCKET)
    {
        local_ep_name[0] = 0;
        remote_ep_name[0] = 0;
    }
#endif
    mqp1.local_ep_address = local_ep_name;
    mqp1.remote_ep_address = remote_ep_name;
    strcpy(endpoints.type, argv[1]);
    strncpy(endpoints.remote_ep_name, local_ep_name, sizeof(endpoints.local_ep_name) - 1);
    strncpy(endpoints.local_ep_name, remote_ep_name, sizeof(endpoints.remote_ep_name) - 1);
    rc = bcm_ipc_msg_queue_create(&mq1, &mqp1);
    if (rc)
    {
        printf("1 - bcm_ipc_msg_queue_create() -> %d\n", rc);
        return -1;
    }

#ifdef IPC_TEST_USE_PTHREAD
    /* Create rx task to make sure that transmit and receive are asynchronous */
    rc = pthread_attr_init(&my_pthread_attr);
    if (rc)
    {
        printf("pthread_attr_init() -> %s\n", strerror(rc));
        return -1;
    }
    rc = pthread_create(&my_thread, &my_pthread_attr, rx_thread_handler, &endpoints);
    if (rc)
    {
        printf("pthread_create() -> %s\n", strerror(rc));
        return -1;
    }
#else
    pid = fork();

    if (pid == 0)
    {
        rx_thread_handler(&endpoints);
    }
    else
    {
#endif


    printf("Press a key to start\n");
    getchar();

    /* Send message to server and wait response */
    for (i=0; i<NUM_MESSAGES; i++)
    {
        /* Allocate message and fill some data */
        m = malloc(sizeof(bcmIpcMsg_t) + sizeof(int));
        if (!m)
        {
            printf("Can't allocate message. Test terminated\n");
            break;
        }
        m->data = (void *)((long)m + sizeof(bcmIpcMsg_t));
        memcpy(m->data, &i, sizeof(int));
        m->size = sizeof(int);
        rc = bcm_ipc_msg_send(&mq1, m, BCM_IPC_MSG_SEND_AUTO_FREE);
        if (rc)
        {
            ++tx_err;
            continue;
        }
        ++tx_ok;
    }

    printf("Waiting for receive..\n");

    /* Wait for rx thread to complete */
    printf("Press a key to stop\n");
    getchar();

#ifdef IPC_TEST_USE_PTHREAD
    pthread_cancel(my_thread);
#else
    }
    if ((rc = waitpid(pid, &status, 0)) == -1)
         printf("parent:error\n");
#endif

    usleep(10000);

    printf("tx_ok=%d, rx_ok=%d, rx_timeout=%d, tx_err=%d, rx_err=%d\n",
      tx_ok, rx_ok, rx_timeout, tx_err, rx_err);

    bcm_ipc_msg_queue_destroy(&mq1);

    return 0;
}
