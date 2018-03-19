#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "bcmtypes.h"
#include "LnxInterface.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef CONFIG_NO_KERNEL_THREAD_INFO
extern void	DisplayKernelThreadInfo(void);
#endif

// Local functions
void messageQueueWait (MSGQUEUE *ptrMessageQueue, MessageNo *msgNo);
void messageQueueSend (MSGQUEUE *ptrMessageQueue, char* message, size_t nbytes);

static PTHREADINFO ptr_thread_head = (PTHREADINFO)NULL;
static PTHREADINFO ptr_thread_tail = (PTHREADINFO)NULL;
static char default_name[] = "NONE";
int TelnetReadFd = -1;
int TelnetWriteFd = -1;
int oldstdout = -1;
int oldstderr = -1;

void RegisterThreadInfo(char *name)
{
	char *ptr_name;
	PTHREADINFO ptr_thread_info;
	int len;

	
	if(name != NULL)
	{
		len = strlen(name) + 1;
	}
	else
	{
		len = strlen(default_name) + 1;
		name = default_name;
	}
	// Allocate memory for name
	if( (ptr_name = malloc(len)) == NULL )
	{
		printf("RegisterThreadInfo: failed to allocate memory for thread name! %s\n",name);
		return;
	}
	// Allocate memory for thread info
	if( (ptr_thread_info = malloc(sizeof(THREADINFO))) == NULL)
	{
		printf("RegisterThreadInfo: failed to allocate memory for thread info! %s\n",name);
		free(ptr_name);
		return;
	}
	// Get the name
	strcpy(ptr_name, name);
	ptr_thread_info->name = ptr_name;
	// Get process id
	ptr_thread_info->pid = getpid();
	// Get thread id
	ptr_thread_info->tid = pthread_self();
	// Terminate the list
	ptr_thread_info->next = (PTHREADINFO)NULL;

	// Insert to the list
	if(ptr_thread_head == NULL)
	{
		ptr_thread_tail = ptr_thread_head = ptr_thread_info;
	}
	else
	{
		ptr_thread_tail->next = ptr_thread_info;
		ptr_thread_tail = ptr_thread_info;
	}
}

void UnRegisterThreadInfo(char *name)
{
	int found = 0;
	PTHREADINFO ptr = ptr_thread_head;
   PTHREADINFO prev = NULL;

	while(ptr != NULL)
	{
      if(strcmp(ptr->name, name) == 0)
      {
         if (prev == NULL)
         {
            ptr_thread_head = ptr->next;
         }
         else
         {
            prev->next = ptr->next;
         }
         if (ptr_thread_tail == ptr)
         {
            ptr_thread_tail = prev;
         }
         free(ptr->name);
         free(ptr);
         found = 1;
         break;
      }
      prev = ptr;
		ptr = ptr->next;
	}
   if (found == 0)
   {
      printf("Warning: UnRegisterThreadInfo: task '%s' not found!\n", name);
   }
}

void DisplayThreadInfo(void)
{
	PTHREADINFO ptr = ptr_thread_head;
    int thdpolicy;
    struct sched_param thdparam;
	unsigned long priority = 0xffffffff;

	Print2File(oldstdout, "PID     TID       PRIORITY NAME\n");
	Print2File(oldstdout, "----------------------------------------------------------\n");
	while(ptr != NULL)
	{
		// Get priority
		if(pthread_getschedparam(ptr->tid, &thdpolicy, &thdparam) != 0 )
		{
			// Log the error...
			Print2File(oldstderr, "DisplayThreadInfo: failed to get thread priority!\n");
		}
		else
		{
			priority = thdparam.sched_priority;
		}
		Print2File(oldstdout, "%-8d0x%-8x%-8lu%s\n",(int)ptr->pid, (int)ptr->tid, priority, ptr->name);
		ptr = ptr->next;
	}
#ifndef CONFIG_NO_KERNEL_THREAD_INFO
	DisplayKernelThreadInfo();
#endif
}
//Below are linux specific stuff
void * ThreadMain(void * arg)
{
	PFN_TREAD_ENTRY thread_entry;
	PTHREADARG ptr_thread_arg = (PTHREADARG) arg;
	if(arg == NULL)
	{
		printf("ThreadMain: arg is NULL!\n");
		return (void *)(-1);
	}
	thread_entry = (PFN_TREAD_ENTRY) ptr_thread_arg->arg0;
	// register the thread information
	RegisterThreadInfo((char *)ptr_thread_arg->arg1);
	// jump to the real entry point and pass argument
	return thread_entry(ptr_thread_arg->arg2);
}
int StartMyThread( char *name, int *taskId, void *(*start_routine)(void*) )
{
    struct sched_param tparam;
    pthread_attr_t fThreadAttr;
	THREADARG thread_arg;

	// 85 is the right priority for now!
    tparam.sched_priority = 85; //10;
    pthread_attr_init(&fThreadAttr);
    pthread_attr_setschedpolicy(&fThreadAttr, SCHED_FIFO);
    pthread_attr_setschedparam(&fThreadAttr, &tparam);
    pthread_attr_setscope(&fThreadAttr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setstacksize(&fThreadAttr, 12 * 1024);

	// set the field of thread_arg
	thread_arg.arg0 = (void *)start_routine;
	thread_arg.arg1 = (void *)name;
	thread_arg.arg2 = (void *)NULL;

    return pthread_create ((pthread_t *) taskId, &fThreadAttr, ThreadMain, (void*)&thread_arg);
}


void WaitForMsgQNotification(msgQid msgQ, MessageNo *pmsgNo)
{
        messageQueueWait (msgQ, pmsgNo);
}

msgQid CreateMyMsgQueue(void)
{
    // Allocate memory for my queue.
    msgQid ptrMyMessageQueue = ( msgQid ) malloc(  sizeof( MSGQUEUE ) );

	pthread_cond_init(&ptrMyMessageQueue->condition, NULL);
	pthread_mutex_init(&ptrMyMessageQueue->lock, NULL);
	ptrMyMessageQueue->busy = 0;
	ptrMyMessageQueue->messageSize = 0;
	memset(&ptrMyMessageQueue->message[0], 0, MAX_MSGQ_LENGTH);

    return ptrMyMessageQueue; 
}

void MsgQueueSend(msgQid msgQID, MessageNo *pMsgPtr, unsigned long nbytes)
{
    messageQueueSend( msgQID, pMsgPtr, nbytes );
}


/*
 *  messageQueueSend
 *  
 *   ****** FROM "AIX Version 4.3 Base Documentation *********
 *  "http://nscp.upenn.edu/aix4.3html/aixgen/topnav/topnav.htm"

 *  A condition  variable  must  always  be associated with a
 *  lock, to avoid the race condition where a thread prepares
 *  to wait on a condition variable and another thread signals
 *  the condition just before the first thread actually  waits
 *  on it.
 */
void
messageQueueSend (MSGQUEUE *ptrMessageQueue, char* message, size_t nbytes)
{
	//int i = 0;

    pthread_mutex_lock (&ptrMessageQueue->lock);

	/* FROM "AIX Version 4.3 Base Documentation "
    "http://nscp.upenn.edu/aix4.3html/aixgen/topnav/topnav.htm"
	A  condition  variable  must  always  be associated with a
    lock, to avoid the race condition where a thread prepares
    to wait on a condition variable and another thread signals
    the condition just before the first thread actually  waits
    on it.

    pthread_cond_init initializes the condition variable cond,
    using the condition attributes specified in cond_attr,  or
    default attributes if cond_attr is NULL.  The LinuxThreads
    implementation  supports  no  attributes  for  conditions,
    hence the cond_attr parameter is actually ignored.

    Variables of type pthread_cond_t can also be initialized
    statically, using the constant PTHREAD_COND_INITIALIZER.*/

    while (ptrMessageQueue->busy)
    {
        pthread_cond_wait (&ptrMessageQueue->condition, &ptrMessageQueue->lock);
    }

    /* Enter protected region */
	/*for(i=0; i<nbytes; i++)
	{
		printf("message[%d]=  0x%02x\n", i, message[i]);
	}*/

    memcpy(ptrMessageQueue->message, message, nbytes );
	ptrMessageQueue->messageSize = nbytes;
    ptrMessageQueue->busy = 1;

	/*for(i=0; i<MAX_MSGQ_LENGTH; i++)
	{
		printf("msgQ->message[%d]=  0x%02x\n", i, ptrMessageQueue->message[i]);

		if( (i % 8) == 0 )
		{
			printf("\n");
		}
	}*/

    /* pthread_cond_signal restarts one of the threads  that  are
       waiting on the condition variable cond.  If no threads are
       waiting on cond, nothing happens. */
    pthread_cond_signal (&ptrMessageQueue->condition);
    pthread_mutex_unlock (&ptrMessageQueue->lock);
}

/*
 * messageQueueWait
 */
void
messageQueueWait (MSGQUEUE *ptrMessageQueue, MessageNo *msgNo)
{
	//int i = 0;

    pthread_mutex_lock (&ptrMessageQueue->lock);

    /* Wait until a message is posted to the queue. */
    while ( ! ptrMessageQueue->busy)
    {
        //printf("messageQueueWait - Waiting\n");
        pthread_cond_wait (&ptrMessageQueue->condition, &ptrMessageQueue->lock);
    }

    /* Retrieve the message */
	memcpy((char*) msgNo, (char *)ptrMessageQueue->message, ptrMessageQueue->messageSize );

	/*for(i=0; i<MAX_MSGQ_LENGTH; i++)
	{
		printf(" [%d]= 0x%02x ", i, ptrMessageQueue->message[i]);

		if( (i % 8) == 0 )
		{
			printf("\n");
		}
	}*/

	/*Clear the queue. */
	memset(ptrMessageQueue->message, 0, MAX_MSGQ_LENGTH);
    ptrMessageQueue->busy = 0;

    /* pthread_cond_signal restarts one of the threads  that  are
       waiting on the condition variable cond.  If no threads are
       waiting on cond, nothing happens. */
    pthread_cond_signal (&ptrMessageQueue->condition);
    pthread_mutex_unlock (&ptrMessageQueue->lock);

    return;
}


void Lnx_CtrlC_Catch()
{
	//if( DebugZone )		//any zone will print out
	//	printf( "Got CTRL_C key! TBD - Will do DOCSIS clean up here\n");

	//install the default behavior back
	signal( SIGINT, SIG_DFL );

}

void osSleep( unsigned long seconds )
{
	//just use the linux user mode sleep function
	sleep( seconds );
}

void inet_ntoa_b (struct in_addr inetAddress, char *pString)
{
    unsigned char byte[4];
    
    // Validate the parameter.
    if (pString == NULL)
        return ;

    byte[0] = (unsigned char) ((inetAddress.s_addr >> 24) & 0xff);
    byte[1] = (unsigned char) ((inetAddress.s_addr >> 16) & 0xff);
    byte[2] = (unsigned char) ((inetAddress.s_addr >>  8) & 0xff);
    byte[3] = (unsigned char) (inetAddress.s_addr & 0xff);

    // Convert to ASCII dotted-decimal format.
    sprintf(pString, "%d.%d.%d.%d", byte[0], byte[1], byte[2], byte[3]);

}

int lnxgetline(char *str, int maxStringSize)
{
	char *pStr = NULL;
	int bytesRead = 0;
	int n;

	fflush(STDIN_FILENO);
	pStr = str;
	while( (bytesRead < maxStringSize) && (*pStr != '\n'))
	{
		  // Wait for user input
		while ( (n = read(STDIN_FILENO, pStr, 1)) == 0 )
		{  
		}

		// Display user input
		printf("%c", *pStr);
        fflush(STDIN_FILENO);

		// Break if LFCR
		if ( *pStr == '\n')  
		{
			break;
		}

		n=0;
		bytesRead++;

		pStr++;
	}

	*pStr = '\0';

	return bytesRead;
}

/***********************************************************************/
/*                                                                     */
/*        DsgTestPrompt: Wait for user input entry.                    */
/*                                                                     */
/***********************************************************************/
void DsgTestPrompt(char * choice)
{
	*choice = '\0';

	while ( read(STDIN_FILENO, choice, 1) == 0 )
	{  
	}

	choice++;
	*choice = '\0';
}


uint32 GetIpStackAddress(uint32* ipAddress, char * interfaceName)
{
	struct ifreq ifr;
	int sock, status;
	struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    
    // Query the existing IP address and subnet mask.
	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_ifrn.ifrn_name, interfaceName );//pInterfaceName);
	sin->sin_family = AF_INET;

    sock = socket (AF_INET, SOCK_PACKET, 0);
    if (sock < 0)
    {
		printf(" Failed to open socket!");
        return EIO; // PTMOD ??
    }

	status = ioctl(sock, SIOCGIFADDR, &ifr) ;
	close(sock);

    if( status )
    {
		printf(" ERROR- SIOCGIFADDR: %s  %s errno = [%d]\n", interfaceName, strerror(errno), errno);
   		return errno;
    }

	memcpy( ipAddress, &(sin->sin_addr.s_addr), sizeof(uint32));
	//printf("Ip Address  %08lx ", *ipAddress);

    return 0;
}

uint32 GetIpStackNetmask(uint32* ipAddress, char * interfaceName)
{
	struct ifreq ifr;
	int sock, status;
	struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    
    // Query the existing IP address and subnet mask.
	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_ifrn.ifrn_name, interfaceName );  //pInterfaceName);
	sin->sin_family = AF_INET;

    sock = socket (AF_INET, SOCK_PACKET, 0);
    if (sock < 0)
    {
		printf(" Failed to open socket!");
        return EIO; // PTMOD ??
    }

	status = ioctl(sock, SIOCGIFNETMASK, &ifr) ;
	close(sock);

    if ( status )
	{
		printf(" ERROR- SIOCGIFMETMASK: %s  %s errno = [%d]\n", interfaceName, strerror(errno), errno);		
		return errno;
	}

	memcpy( ipAddress, &(sin->sin_addr.s_addr), sizeof(uint32));
	//printf("Netmask  %08x ", *ipAddress);

    return 0;
}

int InterfaceIsUp(char * interfaceName)
{
	struct ifreq ifr;
	int sock, status;
	struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    
    // Query the existing IP address and subnet mask.
	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_ifrn.ifrn_name, interfaceName );  //pInterfaceName);
	sin->sin_family = AF_INET;

    sock = socket (AF_INET, SOCK_PACKET, 0);
    if (sock < 0)
    {
		printf(" Failed to open socket!");
        return FALSE; 
    }

	status = ioctl(sock, SIOCGIFFLAGS, &ifr) ;
	close(sock);

    if ( status )
	{
		printf(" ERROR- SIOCGIFFLAGS: %s  %s errno = [%d]\n", interfaceName, strerror(errno), errno);		
		return FALSE;
	}
    if (ifr.ifr_flags & IFF_UP) {
      printf("    %s is UP\n", interfaceName);
      return TRUE;
    } else {
      printf("    %s is DOWN\n", interfaceName);
      return FALSE;
    }

}

//***************************************************************************
//  Linux stub code for change in network stack source.
//  This is borrowed from vxWorks modified source for multiple IP stack support.
//  For now, there no multi Ip support yet.
//  This is just for setting GW Ip address to the Linux Ip stack
//
//  Currently, we only support 8 IP stacks, and we want one gateway for each.
//
//***************************************************************************
void BcmAddRoute(unsigned long routerAddress, char *pInterfaceName, BOOL isDefault)
{
	struct ifreq ifr;
	struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);
	struct rtentry route;

	// Set the socket values.
	memset(&ifr, 0, sizeof(struct ifreq));

	memset(&route, 0, sizeof(route));

	sin->sin_family = AF_INET;
	sin->sin_port = 0;
	//sin->sin_len = sizeof(*sin);

	if (isDefault)
	{
		sin->sin_addr.s_addr = 0; // Use 0,0,GATEWAY for the default route
		memcpy(&route.rt_dst, sin, sizeof(*sin));

		sin->sin_addr.s_addr = 0;
		memcpy(&route.rt_genmask, sin, sizeof(*sin));

		sin->sin_addr.s_addr = htonl(routerAddress); //gateway;
		memcpy(&route.rt_gateway, sin, sizeof(*sin));
	}
	else
	{
		sin->sin_addr.s_addr = htonl(routerAddress); //0; // Use 0,0,GATEWAY for the default route
		memcpy(&route.rt_dst, sin, sizeof(*sin));

		sin->sin_addr.s_addr = htonl(0xffffffff); //0;
		memcpy(&route.rt_genmask, sin, sizeof(*sin));

		sin->sin_addr.s_addr = 0; //gateway;
		memcpy(&route.rt_gateway, sin, sizeof(*sin));
	}

	strcpy(ifr.ifr_name, pInterfaceName);
	route.rt_dev = ifr.ifr_name;
	route.rt_flags = RTF_UP;
	if (isDefault)
	{
		route.rt_flags |= RTF_GATEWAY;
	}
	route.rt_metric = 0;

	int fDhcpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(fDhcpSocket, SIOCADDRT, &route))
	{
		printf(" ERROR- SIOCADDRT: %s  %s errno = [%d]\n", pInterfaceName, strerror(errno), errno);

		close(fDhcpSocket);
		return;
	}

	close(fDhcpSocket);
	return;
}

//***************************************************************************
//  Linux stub code for change in network stack source.
//  This is borrowed from vxWorks modified source for multiple IP stack support.
//  For now, there no multi Ip support yet.
//  This is just for setting GW Ip address to the Linux Ip stack
//
//  Currently, we only support 8 IP stacks, and we want one gateway for each.
//
//***************************************************************************
// Deletes the route for the given interface
//
// Parameters:
//      ipAddress - the IP address of the router (will be returned in this parameter)
//      interfaceName - the name of the given interface
//
// Returns:
//      true if the router was successfully added
//      false if there was a problem adding the route
//
BOOL BcmDeleteRoute(unsigned long routerAddress, char *pInterfaceName)
{
	struct ifreq ifr;
	struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);
	struct rtentry route;

	// Set the socket values.
	memset(&ifr, 0, sizeof(struct ifreq));

	memset(&route, 0, sizeof(route));

	sin->sin_family = AF_INET;
	sin->sin_port = 0;

	sin->sin_addr.s_addr = htonl(routerAddress); //0; // Use 0,0,GATEWAY for the default route
	memcpy(&route.rt_dst, sin, sizeof(*sin));

	sin->sin_addr.s_addr = htonl(0xffffffff); //0;
	memcpy(&route.rt_genmask, sin, sizeof(*sin));

	sin->sin_addr.s_addr = 0; //gateway;
	memcpy(&route.rt_gateway, sin, sizeof(*sin));

	strcpy(ifr.ifr_name, pInterfaceName);
	route.rt_dev = ifr.ifr_name;
	route.rt_flags = 0;
	route.rt_metric = 0;

	int fDhcpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(fDhcpSocket, SIOCDELRT, &route))
	{
		printf(" ERROR- SIOCDELRT: errno=%s  %s\n", strerror(errno), pInterfaceName);
		return FALSE;
	}

	close(fDhcpSocket);
	return TRUE;

}

uint32 SetInterfaceFlags(uint32 ipAddress, char *pInterfaceName, uint32 flags)
{
   struct ifreq ifr;
   struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);
   uint32 retVal = 0;

   // Set the socket values.
   memset(&ifr, 0, sizeof(struct ifreq));
   strcpy(ifr.ifr_name, pInterfaceName);
   sin->sin_family = AF_INET;
   sin->sin_port = 0;
   //sin->sin_len = sizeof(*sin);
   //sin->sin_addr.s_addr = htonl(ipAddress);

   ifr.ifr_flags = flags;

   int fSocketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
   if (ioctl(fSocketDescriptor, SIOCSIFFLAGS, &ifr))
   {
      printf(" ERROR- SIOCSIFFLAGS: [ %s  %s ]  errno = %d\n", strerror(errno), pInterfaceName, errno );
	  retVal = errno;
   }

   close(fSocketDescriptor);

   return retVal;
}

BOOL SetEcmArpEntry(unsigned long ipAddress, uint8* macAddress, char* devName)
{
    
    struct arpreq req;
	int sockfd, status ;

    memset((char *) &req, 0, sizeof(req));

	//uint8 macAddr[6] = kRemoteEcmMacAddress;

	struct sockaddr_in *pIpAddress = (struct sockaddr_in *) &(req.arp_pa);
	// Set up the source IP parameters.
	pIpAddress->sin_family = AF_INET;
	pIpAddress->sin_addr.s_addr = htonl(ipAddress); //(kRemoteEcmIpAddress);

	memcpy((char*)&req.arp_ha.sa_data, macAddress, 6);
	req.arp_ha.sa_family = AF_UNSPEC;
	
    req.arp_flags = ATF_PERM;

	//char* devName = kEcmInterfaceName;
	memcpy(req.arp_dev, devName, strlen(devName));

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return kFalse;
	}

    status = ioctl(sockfd, SIOCSARP, &req);
	close( sockfd );

	if( status )
	{
		printf(" ERROR- SIOCSARP: errno=%s [%d]\n", strerror(errno), errno);

		return kFalse;
	}

    return kTrue;
}

// Sets the IP address that is associated with the IP stack
// for this HalIf.  Note that the stack can have multiple IP addresses
// associated with it.
//
// Parameters:
//      ipAddress - the IP address will be returned in this parameter.
//
// Returns:
//      kTrue if the IP address was successfully set.
//      kFalse if there was a problem setting the IP address.
//
void SetStaticIpAddress(unsigned long ipAddressLong, char *pInterfaceName)
{
   struct ifreq ifr;
   struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);

   // Set the socket values.
   memset(&ifr, 0, sizeof(struct ifreq));
   strcpy(ifr.ifr_name, pInterfaceName);
   sin->sin_family = AF_INET;
   sin->sin_port = 0;
//   sin->sin_len = sizeof(*sin);
   sin->sin_addr.s_addr = htonl(ipAddressLong);

   int fSocketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
   // Set the IP address.
   if (ioctl(fSocketDescriptor, SIOCSIFADDR, &ifr) == -1)
   {
		printf(" ERROR- SIOCSIFADDR: errno=%s \n", strerror(errno));      
   }

   close(fSocketDescriptor);
   return;
}

// Sets the subnet mask that is associated with the IP stack
// for this HalIf.  Note that the stack can have multiple IP addresses
// associated with it.
//
// Parameters:
//      subnetMask - the subnet mask will be returned in this parameter.
//
// Returns:
//      kTrue if the subnet mask was successfully set.
//      kFalse if there was a problem setting the subnet mask.
//
void SetSubnetMask(unsigned long netMaskLong,
                                   char *pInterfaceName)
{
   struct ifreq ifr;
   struct sockaddr_in *sin = (struct sockaddr_in *)&(ifr.ifr_addr);

   // Set the socket values.
   memset(&ifr, 0, sizeof(struct ifreq));
   strcpy(ifr.ifr_name, pInterfaceName);
   sin->sin_family = AF_INET;
   //sin->sin_len = sizeof(*sin);
   sin->sin_port = 0;
   sin->sin_addr.s_addr = htonl(netMaskLong);

   int fSocketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
   // Set the netmask.
   if (ioctl(fSocketDescriptor, SIOCSIFNETMASK, &ifr) == -1)
   {
		printf(" ERROR- SIOCSIFNETMASK: errno=%s \n", strerror(errno));     
   }

   close(fSocketDescriptor);
   return;
}

// Use Linux shell command to do dhcp renew
void RenewDhcpPriIpAddress(char *pInterfaceName)
{
    char syscmdstring[80];

    sprintf( syscmdstring, "dhcpcd -n %s", pInterfaceName );
    printf("#Execute Linux shell cmd and wait 1 second: \"%s\"\n", syscmdstring );
    system(syscmdstring);
    osSleep( 1 );

}

// Redirect/Restore stdout and stderr device to/from Telnet
int Pipe2Telnet(int flag)
{
	extern int TelnetWriteFd;
	extern int oldstdout;
	extern int oldstderr;

	// close stdout (1)
	close(1);
	// close stderr (2)
	close(2);

	if(flag == 1)
	{
		// redirect stdout and stderr to telnet pipe
		if(dup(TelnetWriteFd) != 1 || dup(TelnetWriteFd)!= 2)
		{
			printf("dup failure!!! %d\n", flag);
			return -1;
		}

		//string = "stdout and stderr have been redirected to telnet!\n";
		//printf("test pipe write\n");
		//write(oldstdout, string, strlen(string));
	}
	else
	{
		// restore original stdout and stderr devices
		if(dup(oldstdout) != 1 || dup(oldstderr)!= 2)
		{
			write(oldstdout, "dup failure!!!\n", 15);
			return -1;
		}
		//string = "stdout and stderr device have been restored!\n";
		//write(oldstdout, string, strlen(string));
	}

	return 0;
}

int Print2File(int fd, char *FmtStr, ...)
{
   va_list Args;
   char Text[256];
   int Length;

   va_start (Args, FmtStr);
   Length = vsprintf (Text, FmtStr, Args);
   va_end (Args);
   write(fd, Text, strlen(Text));
   return Length;
}
