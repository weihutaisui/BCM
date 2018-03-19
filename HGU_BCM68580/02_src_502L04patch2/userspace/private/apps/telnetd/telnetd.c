
/*#define DEBUG 1 */

#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <termios.h>
#ifdef DEBUG
#define TELCMDS
#define TELOPTS
#endif
#include <arpa/telnet.h>
#include <ctype.h>

//#include "busybox.h"

/* BRCM begin */
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdarg.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_cli.h"
#include "cms_msg.h"


#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif

/* BRCM end */

#define BUFSIZE 4000

void *msgHandle=NULL;
static char *loginpath = NULL;

/* shell name and arguments */

/*BRCM begin: add a hidden option -r to tell login the request
 is from the remote side */
static char *argv_init[] = {NULL, NULL, NULL};
/*BRCM end */

/* structure that describes a session */

struct tsession {
	struct tsession *next;
	int sockfd, ptyfd;
	int shell_pid;
	/* two circular buffers */
	char *buf1, *buf2;
	int rdidx1, wridx1, size1;
	int rdidx2, wridx2, size2;
};

/*

   This is how the buffers are used. The arrows indicate the movement
   of data.

   +-------+     wridx1++     +------+     rdidx1++     +----------+
   |       | <--------------  | buf1 | <--------------  |          |
   |       |     size1--      +------+     size1++      |          |
   |  pty  |                                            |  socket  |
   |       |     rdidx2++     +------+     wridx2++     |          |
   |       |  --------------> | buf2 |  --------------> |          |
   +-------+     size2++      +------+     size2--      +----------+

   Each session has got two buffers.

*/

static int maxfd;

static struct tsession *sessions;

/* BRCM begin: */
// recognizing ctrl+c
void sig_handler(int sig __attribute__((unused)))
{
// do nothing
}

/* Convert backspace '0x8' to '0x7f' for some windows telnet client
 */
static void convert_bs(char *buf, int size)
{
   int i;

   for ( i = 0; i < size; i ++ ) {
       if (buf[i] == 0x8)
           buf[i] = 0x7f;
   }
}

/*
 * Convert single '\r'(0xd) to '\n'(0xa) for Linux telnet client or TeraTerm,
 * because our terminal input ignores '\r' (termbuf.c_iflag |= IGNCR;)
 * Similar to termbuf.c_iflag |= ICRNL;
 */
static void convert_cr(char *buf, int size)
{
   int i;

   for ( i = 0; i < size-1; i ++ ) {
       if (buf[i] == 0xd && buf[i+1] != 0xa)
	  buf[i] = 0xa;
   }
   if (buf[i] == 0xd)
      buf[i] = 0xa;	  
}


/* BRCM end */


/*

   Remove all IAC's from the buffer pointed to by bf (recieved IACs are ignored
   and must be removed so as to not be interpreted by the terminal).  Make an
   uninterrupted string of characters fit for the terminal.  Do this by packing
   all characters meant for the terminal sequentially towards the end of bf.

   Return a pointer to the beginning of the characters meant for the terminal.
   and make *processed equal to the number of characters that were actually
   processed and *num_totty the number of characters that should be sent to
   the terminal.

   Note - If an IAC (3 byte quantity) starts before (bf + len) but extends
   past (bf + len) then that IAC will be left unprocessed and *processed will be
   less than len.

   FIXME - if we mean to send 0xFF to the terminal then it will be escaped,
   what is the escape character?  We aren't handling that situation here.

  */
static char *
remove_iacs(unsigned char *bf, int len, int *processed, int *num_totty) {
	unsigned char *ptr = bf;
	unsigned char *totty = bf;
	unsigned char *end = bf + len;

	while (ptr < end) {
		if (*ptr != IAC) {
			*totty++ = *ptr++;
		}
		else {
			if ((ptr+2) < end) {
			/* the entire IAC is contained in the buffer
			we were asked to process. */
#ifdef DEBUG
				fprintf(stderr, "Ignoring IAC %s,%s\n",
				    *ptr, TELCMD(*(ptr+1)), TELOPT(*(ptr+2)));
#endif
				ptr += 3;
			} else {
				/* only the beginning of the IAC is in the
				buffer we were asked to process, we can't
				process this char. */
				break;
			}
		}
	}

	*processed = ptr - bf;
	*num_totty = totty - bf;
	/* move the chars meant for the terminal towards the end of the
	buffer. */
	return memmove(ptr - *num_totty, bf, *num_totty);
}


static int
getpty(char *line)
{
	int p;

#ifdef DESKTOP_LINUX  /* was HAVE_DEVPTS_FS */
	p = open("/dev/ptmx", 2);
	if (p > 0) {
		grantpt(p);
		unlockpt(p);
		strcpy(line, ptsname(p));
		return(p);
	}
#else
	struct stat stb;
	int i;
	int j;

	strcpy(line, "/dev/ptyXX");

	for (i = 0; i < 16; i++) {
		line[8] = "pqrstuvwxyzabcde"[i];
		line[9] = '0';
		if (stat(line, &stb) < 0) {
			continue;
		}
		for (j = 0; j < 16; j++) {
			line[9] = "0123456789abcdef"[j];
			if ((p = open(line, O_RDWR | O_NOCTTY)) >= 0) {
				line[5] = 't';
				return p;
			}
		}
	}
#endif /* DESKTOP_LINUX - was HAVE_DEVPTS_FS */
	return -1;
}


static void
send_iac(struct tsession *ts, unsigned char command, int option)
{
	/* We rely on that there is space in the buffer for now.  */
	char *b = ts->buf2 + ts->rdidx2;
	*b++ = IAC;
	*b++ = command;
	*b++ = option;
	ts->rdidx2 += 3;
	ts->size2 += 3;
}


static struct tsession *
make_new_session(int sockfd, NetworkAccessMode accessMode)
{
	struct termios termbuf;
	int pty, pid;
	int keepalive = 1;
	static char tty_name[32];
	struct tsession *ts = (struct tsession *)malloc(sizeof(struct tsession));

	if (ts == NULL) {
		cmsLog_error("out of memory!");
		return 0;
	}
	ts->buf1 = (char *)malloc(BUFSIZE);
	if (ts->buf1 == NULL) {
		cmsLog_error("out of memory!");
		free(ts);
		return 0;
	}
	ts->buf2 = (char *)malloc(BUFSIZE);
	if (ts->buf2 == NULL) {
		free(ts->buf1);
		free(ts);
		cmsLog_error("out of memory!");
		return 0;
	}

	ts->sockfd = sockfd;
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) {
      cmsLog_error("could not set keepalive!");
		free(ts->buf2);
		free(ts->buf1);
		free(ts);
      return 0;
   }


	ts->rdidx1 = ts->wridx1 = ts->size1 = 0;
	ts->rdidx2 = ts->wridx2 = ts->size2 = 0;

	/* Got a new connection, set up a tty and spawn a shell.  */

	pty = getpty(tty_name);

	if (pty < 0) {
		cmsLog_error("cannot accept new session-all tty terminals in use!");
		free(ts->buf2);
		free(ts->buf1);
		free(ts);
		return 0;
	}

	if (pty > maxfd)
		maxfd = pty;

	ts->ptyfd = pty;

	/* Make the telnet client understand we will echo characters so it
	 * should not do it locally. We don't tell the client to run linemode,
	 * because we want to handle line editing and tab completion and other
	 * stuff that requires char-by-char support.
	 */

	send_iac(ts, DO, TELOPT_ECHO);
	send_iac(ts, DO, TELOPT_LFLOW);
	send_iac(ts, WILL, TELOPT_ECHO);
	send_iac(ts, WILL, TELOPT_SGA);


	if ((pid = fork()) < 0) {
		cmsLog_error("fork failed");
	}
	if (pid == 0) {
		/* In child, open the child's side of the tty.  */
		int i, t, msgfd;

		cmsMsg_getEventHandle(msgHandle, &msgfd);
		cmsLog_debug("child at %d, msgHandle=0x%x msgfd=%d", cmsEid_getPid(), msgHandle, msgfd);

		for(i = 0; i <= maxfd; i++) {
			// brcm: keep msgfd open in the child, it needs to send messages from CLI
			if (i != msgfd)
				close(i);
		}

		/* make new process group */
		if (setsid() < 0) {
			cmsLog_error("setsid failed!");
			return 0;
		}	

		t = open(tty_name, O_RDWR /*| O_NOCTTY*/);
		if (t < 0) {
			cmsLog_error("could not open tty!");
			return 0;
		}
		dup(0);
		dup(1);

		tcsetpgrp(0, getpid());

		/* The pseudo-terminal allocated to the client is configured to operate in
		 * cooked mode, and with XTABS CRMOD enabled (see tty(4)).
		 */

		tcgetattr(t, &termbuf);
		termbuf.c_lflag |= ECHO; /* if we use readline we dont want this */
		termbuf.c_oflag |= ONLCR|XTABS;
		//termbuf.c_iflag |= ICRNL;
		termbuf.c_iflag |= IGNCR;
		termbuf.c_iflag &= ~IXOFF;
		/*termbuf.c_lflag &= ~ICANON;*/
		tcsetattr(t, TCSANOW, &termbuf);

#ifdef is_this_dead_code
      /* is this even used anywhere? mwang */
      /*BRCM begin: add a hidden option -r to tell login the request
       is from the remote side */
      if (where == CLI_ACCESS_REMOTE)
        argv_init[1] = "-r";
#endif

      cmsCli_printWelcomeBanner();

      if (cmsCli_authenticate(accessMode, TELNETD_EXIT_ON_IDLE_TIMEOUT) == CMSRET_SUCCESS)
      {
         cmsCli_run(msgHandle, TELNETD_EXIT_ON_IDLE_TIMEOUT);
      }

      cmsMdm_cleanup();
      cmsMsg_cleanup(&msgHandle);
      cmsLog_cleanup();

      exit(0);
      /*BRCM end */
	}

	ts->shell_pid = pid;

	return ts;
}

static void
free_session(struct tsession *ts)
{
	struct tsession *t = sessions;

	/* Unlink this telnet session from the session list.  */
	if(t == ts)
		sessions = ts->next;
	else {
		while(t->next != ts)
			t = t->next;
		t->next = ts->next;
	}

	free(ts->buf1);
	free(ts->buf2);

	kill(ts->shell_pid, SIGKILL);

	wait4(ts->shell_pid, NULL, 0, NULL);

	close(ts->ptyfd);
	close(ts->sockfd);

	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;
	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;

	free(ts);
}


static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   TelnetdCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = cmsObj_get(MDMOID_TELNETD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TELNETD_CFG object failed, ret=%d", ret);
   }
   else
   {
      if (useConfiguredLogLevel)
      {
         cmsLog_setLevel(cmsUtl_logLevelStringToEnum(obj->loggingLevel));
      }

      cmsLog_setDestination(cmsUtl_logDestinationStringToEnum(obj->loggingDestination));

      cmsObj_free((void **) &obj);
   }

   cmsLck_releaseLock();
}


int main(int argc, char **argv)
{
#ifdef SUPPORT_IPV6
	struct sockaddr_in6 sa;
#else
	struct sockaddr_in sa;
#endif
	int master_fd;
	fd_set rdfdset, wrfdset;
	int selret;
   CmsEntityId myEid = EID_TELNETD;
   SINT32 shmId=UNINITIALIZED_SHM_ID;
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   UBOOL8 useConfiguredLogLevel = TRUE;
   SINT32 c, logLevelNum;
   CmsRet ret;

	
	if (!loginpath)
		loginpath = "/bin/sh";

	argv_init[0] = loginpath;
	sessions = 0;


   /*
    * Initialize our logging.
    */
   cmsLog_initWithName(myEid, argv[0]);



   while ((c = getopt(argc, argv, "v:m:")) != -1)
   {
      switch(c)
      {
      case 'm':
         shmId = atoi(optarg);
         break;

      case 'v':
         logLevelNum = atoi(optarg);
         if (logLevelNum == 0)
         {
            logLevel = LOG_LEVEL_ERR;
         }
         else if (logLevelNum == 1)
         {
            logLevel = LOG_LEVEL_NOTICE;
         }
         else
         {
            logLevel = LOG_LEVEL_DEBUG;
         }
         cmsLog_setLevel(logLevel);
         useConfiguredLogLevel = FALSE;
         break;

      default:
         cmsLog_error("bad arguments, exit");
         cmsLog_cleanup();
         exit(-1);
      }
   }



   /*
    * By convention, when smd launches the telnetd server, it will pass
    * the server fd at this fd (3) see cms_params.h.
    */
   master_fd = CMS_DYNAMIC_LAUNCH_SERVER_FD;

   /* mwang: I think we should let sigint kill this process. */
   /* signal(SIGINT,sig_handler); */


   /*
    * Initialize our message link back to smd.
    */
   cmsMsg_initWithFlags(myEid, 0, &msgHandle);


   /*
    * Initialize MDM.  Even though telnetd does not use it, the underlying CLI needs it.
    */
   if ((ret = cmsMdm_initWithAcc(myEid, NDA_ACCESS_TELNETD, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not initialize MDM, ret=%d", ret);
      exit(-1);
   }

   initLoggingFromConfig(useConfiguredLogLevel);


	maxfd = master_fd;

	do {
      struct tsession *ts;
      struct timeval timer;

		// You have to do this in Linux, reinitialize timer each iteration
      /*
       * mwang: When telnetd is first launched, there is a client waiting to
       * connect (and sessions will == NULL), so timeout does not need to be
       * very long.  Once a telnetd client is running (sessions != NULL),
       * set timeout to a little bit longer than TELNED_EXIT_ON_IDLE_TIMEOUT because the
       * client process will detect idle-ness and exit.  Once the client process
       * exits, sessions will equal NULL again, and now we set a short timeout
       * and will exit shortly after.
       */
		timer.tv_sec = (sessions == NULL) ? 2 : TELNETD_EXIT_ON_IDLE_TIMEOUT + 2;
		timer.tv_usec = 0;

		FD_ZERO(&rdfdset);
		FD_ZERO(&wrfdset);

		/* select on the master socket, all telnet sockets and their
		 * ptys if there is room in their respective session buffers.
		 */

		FD_SET(master_fd, &rdfdset);

		ts = sessions;
		while (ts) {
			/* buf1 is used from socket to pty
			 * buf2 is used from pty to socket
			 */
			if (ts->size1 > 0) {
				FD_SET(ts->ptyfd, &wrfdset);  /* can write to pty */
			}
			if (ts->size1 < BUFSIZE) {
				FD_SET(ts->sockfd, &rdfdset); /* can read from socket */
			}
			if (ts->size2 > 0) {
				FD_SET(ts->sockfd, &wrfdset); /* can write to socket */
			}
			if (ts->size2 < BUFSIZE) {
				FD_SET(ts->ptyfd, &rdfdset);  /* can read from pty */
			}
			ts = ts->next;
		}

		selret = select(maxfd + 1, &rdfdset, &wrfdset, 0, &timer);
		
		// If no activity with TELNET_TIMEOUT, terminate all sessions.
		if (!selret) {
			ts = sessions;
			while (ts) {
				struct tsession *next = ts->next;
				free_session(ts);
				ts = next;
			}
			cmsLog_notice("exit on idle (%d seconds) or no more clients", TELNETD_EXIT_ON_IDLE_TIMEOUT);
			cmsMdm_cleanup();
			cmsMsg_cleanup(&msgHandle);
			cmsLog_cleanup();
			exit(0);
		}

		/* First check for and accept new sessions.  */
		if (FD_ISSET(master_fd, &rdfdset)) {
			int fd;
         UINT32 salen;
         char ipAddrBuf[BUFLEN_40];
         char * pIpAddr;
         int accessMode;

         char connIfName[CMS_IFNAME_LENGTH] = {0};

			memset((void *)&sa, 0, sizeof(sa));
			salen = sizeof(sa);
			if ((fd = accept(master_fd, (struct sockaddr *)&sa, &salen)) < 0) {
				continue;
			} else {
            
            /* BRCM begin: Add remote access check when there is no firewall*/

#ifdef SUPPORT_IPV6
            ipAddrBuf[0] = '\0';
            inet_ntop(AF_INET6, &sa.sin6_addr, ipAddrBuf, sizeof(ipAddrBuf));

            /* see if this is a IPv4-Mapped IPv6 address (::ffff:xxx.xxx.xxx.xxx) */
            if (strchr(ipAddrBuf, '.') && strstr(ipAddrBuf, ":ffff:"))
            {
               /* IPv4 client */

               /* convert address to clean ipv4 address */
               pIpAddr = strrchr(ipAddrBuf, ':') + 1;
            }
            else
            {
               /* IPv6 client */
               pIpAddr = &ipAddrBuf[0];
            }
#else
            strcpy(ipAddrBuf, inet_ntoa(sa.sin_addr));
            pIpAddr = &ipAddrBuf[0];
#endif
            if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
            {
               cmsLog_error("failed to get lock, ret=%d", ret);
               return 0;
            }

            cmsImg_saveIfNameFromSocket(fd, connIfName);
#ifndef DESKTOP_LINUX
            if (cmsUtl_strstr(connIfName,"br0") == NULL)
            {
               accessMode = NETWORK_ACCESS_WAN_SIDE;
            }
            else
#endif /* DESKTOP_LINUX */
            {
               accessMode = NETWORK_ACCESS_LAN_SIDE;
            }

            cmsLck_releaseLock();

            /* update cli lib with the application data */
            cmsCli_setAppData("Telnetd", pIpAddr, NULL, TELNETD_PORT);

#ifdef later
                     // mwang_todo: is this for downloads?  Don't kill this process if
                     // this process was the one that requested the download?

                     // get the interface name for dltftp (for not killing the process it is on).
                     char ifName[32];
                     FILE *ifc_fp;
                     ifName[0] = '\0';
                     if (bcmGetIntfNameSocket(fd, ifName) == 0) {
                        if (ifName[0] != '\0') {
                           if (!(ifc_fp = fopen("/var/run/telnetd_ifc", "w"))) {
                              perror("/var/run/telnetd_ifc");
                              return errno;
                           }
                           fprintf(ifc_fp, "%s\n", ifName);
                           fclose(ifc_fp);
                        }
                     }
#endif
                /* BRCM end */


				/* Create a new session and link it into
					our active list.  */

				struct tsession *new_ts = make_new_session(fd, accessMode);
				if (new_ts) {
					new_ts->next = sessions;
					sessions = new_ts;
					if (fd > maxfd)
						maxfd = fd;
				} else {
					close(fd);
				}
			}
		}

		/* Then check for data tunneling.  */

		ts = sessions;
		while (ts) { /* For all sessions...  */
			int maxlen, w, r;
			struct tsession *next = ts->next; /* in case we free ts. */

			if (ts->size1 && FD_ISSET(ts->ptyfd, &wrfdset)) {
				int processed, num_totty;
				char *ptr;
				/* Write to pty from buffer 1.  */

				maxlen = MIN(BUFSIZE - ts->wridx1, ts->size1);
				ptr = remove_iacs((unsigned char *)(ts->buf1) + ts->wridx1, maxlen,
					&processed, &num_totty);

				/* the difference between processed and num_to tty
				   is all the iacs we removed from the stream.
				   Adjust buf1 accordingly. */
				ts->wridx1 += processed - num_totty;
				ts->size1 -= processed - num_totty;

				w = write(ts->ptyfd, ptr, num_totty);
				if (w < 0) {
					cmsLog_error("write");
					free_session(ts);
					ts = next;
					continue;
				}
				ts->wridx1 += w;
				ts->size1 -= w;
				if (ts->wridx1 == BUFSIZE)
					ts->wridx1 = 0;
			}

			if (ts->size2 && FD_ISSET(ts->sockfd, &wrfdset)) {
				/* Write to socket from buffer 2.  */
				maxlen = MIN(BUFSIZE - ts->wridx2, ts->size2);
				w = write(ts->sockfd, ts->buf2 + ts->wridx2, maxlen);
				if (w < 0) {
					cmsLog_error("write");
					free_session(ts);
					ts = next;
					continue;
				}
				ts->wridx2 += w;
				ts->size2 -= w;
				if (ts->wridx2 == BUFSIZE)
					ts->wridx2 = 0;
			}

			if (ts->size1 < BUFSIZE && FD_ISSET(ts->sockfd, &rdfdset)) {
				/* Read from socket to buffer 1. */
				maxlen = MIN(BUFSIZE - ts->rdidx1, BUFSIZE - ts->size1);
				r = read(ts->sockfd, ts->buf1 + ts->rdidx1, maxlen);
				if (!r || (r < 0 && errno != EINTR)) {
					free_session(ts);
					ts = next;
					continue;
				}
				if(!*(ts->buf1 + ts->rdidx1 + r - 1)) {
					r--;
					if(!r)
						continue;
				}
				convert_bs(ts->buf1 + ts->rdidx1, r);
				convert_cr(ts->buf1 + ts->rdidx1, r);
				ts->rdidx1 += r;
				ts->size1 += r;
				if (ts->rdidx1 == BUFSIZE)
					ts->rdidx1 = 0;
			}

			if (ts->size2 < BUFSIZE && FD_ISSET(ts->ptyfd, &rdfdset)) {
				/* Read from pty to buffer 2.  */
				maxlen = MIN(BUFSIZE - ts->rdidx2, BUFSIZE - ts->size2);
				r = read(ts->ptyfd, ts->buf2 + ts->rdidx2, maxlen);
				if (!r || (r < 0 && errno != EINTR)) {
					free_session(ts);
					ts = next;
					continue;
				}
				ts->rdidx2 += r;
				ts->size2 += r;
				if (ts->rdidx2 == BUFSIZE)
					ts->rdidx2 = 0;
			}

			if (ts->size1 == 0) {
				ts->rdidx1 = 0;
				ts->wridx1 = 0;
			}
			if (ts->size2 == 0) {
				ts->rdidx2 = 0;
				ts->wridx2 = 0;
			}
			ts = next;
		}

	} while (1);

	return 0;
}


