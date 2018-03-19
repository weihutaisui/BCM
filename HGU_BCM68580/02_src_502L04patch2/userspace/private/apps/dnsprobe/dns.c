/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
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


/*
 *  Filename:       dns.c
 *
******************************************************************************
//  Description:
//      It provides dnsprobe functionality.
//
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <syslog.h>

#include <net/if.h>
#include <linux/sockios.h>
#include <sys/uio.h>
#include <cms_params.h>

#define	DNS_PORT	53
#define TRUE 1
#define FALSE 0
void locate_dns_server();
void do_query();
int  verify_dns_server();
int getaddr(char *devname, char* ip);
int do_dns_query(int sockfd, struct sockaddr *serv_addr,int servlen);
char primary[64];
char secondary[64];
char langw[64];
char pingstring[128];
int doit(int verify);
int noprimary=0;
int nosecondary=0;
int onsecondary=0;
int sockfd;
extern char **environ;

int bcmsystem (char *command);
void bcmInsertModule(char *modName);
int bcmIsModuleInserted(char *modName) ;


#ifdef BUILD_STATIC
int dnsprobe_main ()
#else
int main ()
#endif
{
	int status;
	char cmd[128];
	
	bzero(cmd,128);
	openlog("dnsprobe",LOG_PID|LOG_NDELAY,LOG_USER);
	syslog(LOG_NOTICE,"dnsprobe started!\n");
	getaddr("br0",langw);
	langw[strlen(langw)] = 0;
	locate_dns_server();	/* primary and secondary */
#if 0
	status=verify_dns_server();	/* primary and secondary */
	if ( status != 0) { /* Bad dns ip */
	    syslog(LOG_NOTICE,"dnsprobe got bad dns server ip address\n");
	    if ( status == 1) { /* Bad secondary ip */
	      syslog(LOG_NOTICE,"dnsprobe got bad secondary dns server ip address-> exit!\n");
	      //printf("dnsprobe got bad secondary dns server ip address-> exit!\n");
	    }
	    if ( status == 2) { /* Bad primary ip */
	      syslog(LOG_NOTICE,"dnsprobe got bad primary dns server ip address!\n");
	      //printf("dnsprobe got bad primary dns server ip address switch to Secondary\n");
 sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s",langw, primary);
      		bcmsystem(cmd);
 sprintf(cmd,"iptables -t nat -A PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s",langw, secondary);
      		bcmsystem(cmd);
		onsecondary=1;
	    }
	    if ( status == 3) { /* Bad dns ip */
	      syslog(LOG_NOTICE,"dnsprobe got bad dns server ip address\n");
	      //printf("dnsprobe got bad dns server ip address!\n");
	    }
	    //exit(1);
	}
#endif
	while(1) {
          do_query();
	  sleep(DNS_PROBE_INTERVAL);
	}
}
/*
** query the primary dns server
** if primary die, switch to secondary, start to query primary again
** if primary ok, switch back to primary
** perform an type A query to one of root server
** if A record query failed, then this means named die
*/
void do_query()
{
	int status;
	char cmd[128];

	status = doit(0);  /* probe primary */
	if ( status != 0) { /* server down */
	   /* primary dead or still dead,delete iptable,switch to secondary */
	   if ( status != 0 && !onsecondary) {
		printf("Primary DNS server Is Down... Switching To Secondary DNS server \n");
		syslog(LOG_NOTICE,"Primary DNS server Is Down... Switching To Secondary DNS server \n");
                
		bcmInsertModule("ip_tables");
                bcmInsertModule("ip_conntrack");
                bcmInsertModule("iptable_nat");

   sprintf(cmd, "iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to 128.9.0.107 2>/dev/null", langw);
      		bcmsystem(cmd);

		    sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null",langw, primary);
      		    bcmsystem(cmd);

                  /*clear nat item*/
		    bzero(cmd,128);
      		    sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null",langw, primary);
      		    bcmsystem(cmd);		

		    bzero(cmd,128);
      		    sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null",langw, secondary);
      		    bcmsystem(cmd);				  

		    /*added command to clear dns ip_conntrack*/
 		    bzero(cmd,128);
                  sprintf(cmd,"cat /proc/net/ip_clear_dnsconntrack");
      		bcmsystem(cmd);

		    bzero(cmd,128);
      		    sprintf(cmd,"iptables -t nat -A PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s",langw, secondary);
      		bcmsystem(cmd);
		onsecondary=1;
	    }
        }
	if ( status == 0 && onsecondary ) {
	     /* switch back to primary since its up again */
		printf("Switching Back To Primary DNS server \n");
		syslog(LOG_NOTICE,"Switching Back To Primary DNS server \n");
                
		bcmInsertModule("ip_tables");
                bcmInsertModule("ip_conntrack");      
                bcmInsertModule("iptable_nat");
   sprintf(cmd, "iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to 128.9.0.107 2>/dev/null", langw);
      		bcmsystem(cmd);
      		sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null",langw, secondary);
      		bcmsystem(cmd);

		 /*clear nat item*/
		bzero(cmd,128);
      		sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null",langw, primary);
      		bcmsystem(cmd);		

		bzero(cmd,128);
      		sprintf(cmd,"iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null",langw, secondary);
      		bcmsystem(cmd);				  


		 /*added command to clear dns ip_conntrack*/
 		 bzero(cmd,128);
               sprintf(cmd,"cat /proc/net/ip_clear_dnsconntrack");
      		 bcmsystem(cmd);             

 		 bzero(cmd,128);			 
      		 sprintf(cmd, "iptables -t nat -A PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s",langw, primary);
      		bcmsystem(cmd);
		onsecondary=0;
	}
}

/*
** If verify = 0, we are in the loop after we know both ip address of dns servers are good.
** If verify =1 , we are verifying the primary dns server's ip address
** If verify =2 , we are verifying the primary dns server's ip address
*/
int
doit(int verify)
{
	struct sockaddr_in	cli_addr;
	struct sockaddr_in	serv_addr;
	int ret;
	int ntries = 0;

	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	if ( verify == 2) /* check the IP address of secondary dns server */
          serv_addr.sin_addr.s_addr = inet_addr(secondary);
	else
          serv_addr.sin_addr.s_addr = inet_addr(primary);

	serv_addr.sin_port = htons(DNS_PORT); /* 53 */

	if ( (sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		perror("socket");

	bzero((char *)&cli_addr,sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(0); 
	if ( bind(sockfd,(struct sockaddr *)&cli_addr,sizeof(cli_addr)) <0)
		perror("bind");

	/*
	** Do dns type A query every  30 secs to ensure the health of named
	** If you do not receive a response retry 3 times and then switch to
	** the secondary DNS server. DNS uses UDP and these packets can be
	** lost on the internet so it is safer to retry a couple of times
	** before giving it up.
	*/
	for ( ; ntries < DNS_PROBE_MAX_TRY; ntries++ ) {
		ret = do_dns_query(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		if ( !ret ) { 
			//printf("dns query failed \n");
			syslog(LOG_WARNING,"dns query failed \n");
			//close(sockfd);
			//return 1;
			continue;
		} else {
			close(sockfd);
			return 0;
		}
	}
	close (sockfd);
	return 1;
}

int
do_dns_query(int sockfd, struct sockaddr *serv_addr,int servlen)
{
	int nbytes;
	char buf[128];
	char rbuf[256];
	char *ptr;
	short one,try=0;
	short n=0;
	short answers;


	ptr = buf;
	*((u_short *)ptr) = htons(1234); /* id */
	ptr += 2;
	*((u_short *)ptr) = htons(0x0100); /* flag */
	ptr += 2;
	*((u_short *)ptr) = htons(1); /* number of questions */
	ptr += 2;
	*((u_short *)ptr) = htons(0); /* number of answers RR */
	ptr += 2;
	*((u_short *)ptr) = htons(0); /* number of Authority RR */
	ptr += 2;
	*((u_short *)ptr) = htons(0); /* number of Additional RR */
	ptr += 2;

	memcpy(ptr,"\001a\014root-servers\003net\000",20);
	//memcpy(ptr,"\003www\005yahoo\003com\000",15);
	ptr += 20;
	//ptr += 15;
	one = htons(1);
	memcpy(ptr,&one,2);		/* query type = A */
	ptr += 2;
	memcpy(ptr,&one,2);		/* query class = 1 (IP addr) */
	ptr += 2;

	//nbytes = 31;
	nbytes = 36;
	if ( sendto(sockfd,buf,nbytes,0,serv_addr,servlen) != n)
	//	perror("sendto");

	fcntl(sockfd,F_SETFL,O_NONBLOCK);
	//printf("recvfrom.. \n");
tryagain:
	try++;
	n = recvfrom(sockfd,rbuf,128,0,(struct sockaddr *)0,(int *)0);
	if ( n == -1 && errno == EAGAIN && try < DNS_PROBE_TIMEOUT) {
		sleep(1);
		goto tryagain;
	}
	else if (n >= 0) { /* good answer */
	  rbuf[n]=0;
	  answers = htons(*(short *)&rbuf[6]);
	  //printf("n=%d answer=%d \n",n,htons(*(short *)&rbuf[6]));
	  if ( answers >= 1) /* good we got an answer */
		return 1;
	  else
		return 0;
	}
	  else
		return 0;
}
/*
 * determine primary and secondary dns server
 */
void locate_dns_server()
{
	char s[128];
	char *p;
	FILE *fd;
	int pr=0;

	fd = fopen("/etc/resolv.conf","r");
	if (fd == NULL) {
	   printf("dnsprobe: Can not open /etc/resolv.conf  \n");
	   syslog(LOG_WARNING,"dnsprobe: Can not open /etc/resolv.conf \n");
	   exit(1);
	}
	while (fgets(s,64,fd) != NULL) {
		p = strtok(s," ");
		if ( !strcmp("nameserver",p)) {
		   p = strtok(0," ");
		   if ( pr == 0) {
		     strcpy(primary,p);
		     primary[strlen(primary)-1] =0;
		     printf("Primary DNS server = %s\n",primary);
		     pr = 1;
		   }
		   else {
		     strcpy(secondary,p);
		     secondary[strlen(secondary)-1] =0;
		     printf("Secondary DNS server = %s\n",secondary);
		   }
		}
	}
	if (primary[0] == 0) {
	  printf("No Primary DNS server found \n");
	  syslog(LOG_WARNING,"No Primary DNS server found \n");
	  noprimary=1;
	  fclose(fd);
	  exit(0);
	}
	if (secondary[0] == 0) {
	  printf("No Secondary DNS server found \n");
	  syslog(LOG_WARNING,"No Secondary DNS server found \n");
	  nosecondary=1;
	  fclose(fd);
	  /* do nothing,no need to switch server */
	  exit(0);
	}
	
	fclose(fd);
}
/*
** get the interface address for "br0" or,if different, other
*/
int
getaddr(char *devname, char* ip)
{
   int  fd;
   int  ret=-1;
   struct ifreq intf;
   char *ptr;

   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
     return -1;
   }

   strcpy(intf.ifr_name, devname);

   if (ioctl(fd, SIOCGIFDSTADDR, &intf) != -1) {
     if ((ptr =
          inet_ntoa(((struct sockaddr_in *)(&(intf.ifr_dstaddr)))->sin_addr)) != NULL) {
       strcpy(ip,ptr);
       ret = 0;
     }
   }
   close(fd);
   return ret;
}

int bcmsystem (char *command) {
   int pid = 0, status = 0;

   /* printf("dnsprobe: %s\n",command); */
   if ( command == 0 )
      return 1;

   pid = vfork();
   if ( pid == -1 )
      return -1;

   if ( pid == 0 ) {
      char *argv[4];
      argv[0] = "sh";
      argv[1] = "-c";
      argv[2] = command;
      argv[3] = 0;
      execve("/bin/sh", argv, environ);
      exit(127);
   }

   /* wait for child process return */
   do {
      if ( waitpid(pid, &status, WUNTRACED) == -1 ) {
         if ( errno != EINTR )
            return -1;
      } else  {
         return status;
	}
   } while ( 1 );

   return status;
}

/***************************************************************************
// Function Name: bcmInsertModule.
// Description  : insert module with the given name.
// Parameters   : modName -- the given module name.
// Returns      : none.
****************************************************************************/
void bcmInsertModule(char *modName) {
   char cmd[256];

   if ( bcmIsModuleInserted(modName) == FALSE ) {
      sprintf(cmd, "insmod %s", modName);
      bcmsystem(cmd);
   }
}

/***************************************************************************
// Function Name: bcmIsModuleInserted.
// Description  : verify the given module name is already inserted or not.
// Parameters   : modName -- the given module name.
// Returns      : TRUE or FALSE.
****************************************************************************/
#if STATIC_KERNEL_NF_FIREWALL
int bcmIsModuleInserted(char *modName) {
   int ret = TRUE;
   if ((strcmp(modName, "ip_tables") == 0) ||
#if STATIC_KERNEL_NF_NAT
       (strcmp(modName, "iptable_nat")   == 0) ||
#endif
       (strcmp(modName, "iptable_raw") == 0)) {
      ret = TRUE;
   }

   return ret;
}
#else
int bcmIsModuleInserted(char *modName) {
   int ret = FALSE;
   char buf[256];
   FILE* fs = fopen("/proc/modules", "r");

   if ( fs != NULL ) {
      while ( fgets(buf, 256, fs) > 0 )
         if ( strstr(buf, modName) != NULL ) {
            ret = TRUE;
            break;
         }
      fclose(fs);
   }

   return ret;
}
#endif



/*
 * To determine the health of both primary and secondary dns server
 * This is needed to make sure user entered the correct dns ip addresses
 */
int verify_dns_server()
{
	int pstatus=0;
	int sstatus=0;


	pstatus=doit(1); /* verify primary dns server's ip address*/
	sstatus=doit(2); /* verify secondary dns server's ip address*/

	if ( pstatus==0 && sstatus==0)
	    return 0; /* good on both IP */
	else if (pstatus == 0 && sstatus != 0)
	    return 1; /* Bad secondary IP */
	else if (pstatus != 0 && sstatus == 0)
	    return 2; /* Bad Primary IP */
	else
	    return 3; /* Both are Bad IP */
}
