/* dhcpd.c
 *
 * Moreton Bay DHCP Server
 * Copyright (C) 1999 Matthew Ramsay <matthewr@moreton.com.au>
 *			Chris Trew <ctrew@moreton.com.au>
 *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>

#include "debug.h"
#include "dhcpd.h"
#include "arpping.h"
#include "socket.h"
#include "options.h"
#include "files.h"
#include "leases.h"
#include "packet.h"
#include "serverpacket.h"
#include "pidfile.h"
#include "static_leases.h"
#include "cms_mem.h"
#include "cms_msg.h"
#ifdef SUPPORT_DHCP_RELAY
#include "relay.h"
#endif

/* globals */
struct server_config_t server_config;
struct iface_config_t *iface_config;
struct iface_config_t *cur_iface;
#ifdef SUPPORT_DHCP_RELAY
struct relay_config_t *relay_config;
struct relay_config_t *cur_relay;
#endif
// BRCM_begin
struct dhcpOfferedAddr *declines;
pVI_INFO_LIST viList;
void *msgHandle=NULL;
extern char deviceOui[];
extern char deviceSerialNum[];
extern char deviceProductClass[];

extern void bcmQosDhcp(int optionnum, char *cmd);
extern void bcmDelObsoleteRules(void);
extern void bcmExecOptCmd(void);

// BRCM_end

/* Exit and cleanup */
void exit_server(int retval)
{
	cmsMsg_cleanup(&msgHandle);
	pidfile_delete(server_config.pidfile);
	CLOSE_LOG();
	exit(retval);
}


/* SIGTERM handler */
static void udhcpd_killed(int sig)
{
	(void)sig;
	LOG(LOG_INFO, "Received SIGTERM");
	exit_server(0);
}


// BRCM_BEGIN
static void test_vendorid(struct dhcpMessage *packet, char *vid, int *declined)
{
	*declined = 0;

	/*
	 * If both internal vendor_ids list is not NULL and client
	 * request packet contains a vendor id, then check for match.
	 * Otherwise, leave declined at 0.
	 */
	if (cur_iface->vendor_ids != NULL && vid != NULL) {
		int len = (int)(*((unsigned char*)vid - 1));
		vendor_id_t * id = cur_iface->vendor_ids;
		//char tmpbuf[257] = {0};
		//memcpy(tmpbuf, vid, len);
		do {
			//printf("dhcpd:test_vendorid: id=%s(%d) vid=%s(%d)\n", id->id, id->len, tmpbuf, len);
			if (id->len == len && memcmp(id->id, vid, len) == 0) {
				/* vid matched something in our list, decline */
				memcpy(declines->chaddr,  packet->chaddr, sizeof(declines->chaddr));

				/*
				 * vid does not contain a terminating null, so we have to make sure
				 * the declines->vendorid is null terminated.  And use memcpy 
				 * instead of strcpy because vid is not null terminated.
				 */
				memset(declines->vendorid, 0, sizeof(declines->vendorid));
				memcpy(declines->vendorid, vid, len);

				*declined = 1;
			}
		} while((*declined == 0) && (id = id->next));
	}

	if (*declined) {
		write_decline();
		sendNAK(packet);
	}
}

// BRCM_END

#ifdef COMBINED_BINARY	
int udhcpd(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	CmsRet ret;
	int msg_fd;
	fd_set rfds;
	struct timeval tv;
        //BRCM --initialize server_socket to -1
	int bytes, retval;
	struct dhcpMessage packet;
	unsigned char *state;
        // BRCM added vendorid
	char *server_id, *requested, *hostname, *vendorid = NULL;
	u_int32_t server_id_align, requested_align;
	unsigned long timeout_end;
	struct dhcpOfferedAddr *lease;
	//For static IP lease
	struct dhcpOfferedAddr static_lease;
	uint32_t static_lease_ip;

	int pid_fd;
	/* Optimize malloc */
	mallopt(M_TRIM_THRESHOLD, 8192);
	mallopt(M_MMAP_THRESHOLD, 16384);

	//argc = argv[0][0]; /* get rid of some warnings */

	OPEN_LOG("udhcpd");
#ifdef VERBOSE
	cmsLog_setLevel(LOG_LEVEL_DEBUG);
#else
	cmsLog_setLevel(DEFAULT_LOG_LEVEL);
#endif
	LOG(LOG_INFO, "udhcp server (v%s) started", VERSION);
	
	pid_fd = pidfile_acquire(server_config.pidfile);
	pidfile_write_release(pid_fd);

	if ((ret = cmsMsg_initWithFlags(EID_DHCPD, 0, &msgHandle)) != CMSRET_SUCCESS) {
		LOG(LOG_ERR, "cmsMsg_init failed, ret=%d", ret);
		pidfile_delete(server_config.pidfile);
		CLOSE_LOG();
		exit(1);
	}

	//read_config(DHCPD_CONF_FILE);
	read_config(argc < 2 ? DHCPD_CONF_FILE : argv[1]);
	
	read_leases(server_config.lease_file);

        // BRCM begin
	declines = malloc(sizeof(struct dhcpOfferedAddr));
	/* vendor identifying info list */
	viList = malloc(sizeof(VI_INFO_LIST));
	memset(viList, 0, sizeof(VI_INFO_LIST));
        deviceOui[0] = '\0';
        deviceSerialNum[0] = '\0';
        deviceProductClass[0] = '\0';
        // BRCM end


#ifndef DEBUGGING
	pid_fd = pidfile_acquire(server_config.pidfile); /* hold lock during fork. */

#ifdef BRCM_CMS_BUILD
   /*
    * BRCM_BEGIN: mwang: In CMS architecture, we don't want dhcpd to
    * daemonize itself.  It creates an extra zombie process that
    * we don't want to deal with.
    * However, we do want to do a setid to detach from console.
    */
   if (setsid() < 0)
   {
      cmsLog_error("could not detach from terminal");
      exit(-1);
   }
#else
	switch(fork()) {
	case -1:
		perror("fork");
		exit_server(1);
		/*NOTREACHED*/
	case 0:
		break; /* child continues */
	default:
		exit(0); /* parent exits */
		/*NOTREACHED*/
		}
	close(0);
	setsid();
#endif /* BRCM_CMS_BUILD */

	pidfile_write_release(pid_fd);
#endif
	signal(SIGUSR1, write_leases);
	signal(SIGTERM, udhcpd_killed);
	signal(SIGUSR2, write_viTable);

#ifdef BRCM_CMS_BUILD
	/* ignore some common, problematic signals */
	signal(SIGINT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
#endif

	timeout_end = time(0) + server_config.auto_time;

	cmsMsg_getEventHandle(msgHandle, &msg_fd);

	while(1) { /* loop until universe collapses */
                //BRCM_begin
                int declined = 0;
		int max_skt = msg_fd;
		FD_ZERO(&rfds);
		FD_SET(msg_fd, &rfds);
#ifdef SUPPORT_DHCP_RELAY
                for(cur_relay = relay_config; cur_relay;
			cur_relay = cur_relay->next ) {
			if (cur_relay->skt < 0) {
				cur_relay->skt = listen_socket(INADDR_ANY,
					SERVER_PORT, cur_relay->interface);
				if(cur_relay->skt == -1) {
					LOG(LOG_ERR, "couldn't create relay "
						"socket");
					exit_server(0);
				}
			}

			FD_SET(cur_relay->skt, &rfds);
			if (cur_relay->skt > max_skt)
				max_skt = cur_relay->skt;
		}
#endif
                for(cur_iface = iface_config; cur_iface;
			cur_iface = cur_iface->next ) {
			if (cur_iface->skt < 0) {
				cur_iface->skt = listen_socket(INADDR_ANY,
					SERVER_PORT, cur_iface->interface);
				if(cur_iface->skt == -1) {
					LOG(LOG_ERR, "couldn't create server "
						"socket on %s -- au revoir",
						cur_iface->interface);
					exit_server(0);
				}
			}

			FD_SET(cur_iface->skt, &rfds);
			if (cur_iface->skt > max_skt)
				max_skt = cur_iface->skt;
                }  //BRCM_end
		if (server_config.auto_time) {
			tv.tv_sec = timeout_end - time(0);
			if (tv.tv_sec <= 0) {
				tv.tv_sec = server_config.auto_time;
				timeout_end = time(0) + server_config.auto_time;
				write_leases(0);
			}
			tv.tv_usec = 0;
		}
		retval = select(max_skt + 1, &rfds, NULL, NULL,
			server_config.auto_time ? &tv : NULL);
		if (retval == 0) {
			write_leases(0);
			timeout_end = time(0) + server_config.auto_time;
			continue;
		} else if (retval < 0) {
			if (errno != EINTR)
				perror("select()");
			continue;
		}

		/* Is there a CMS message received? */
		if (FD_ISSET(msg_fd, &rfds)) {
			CmsMsgHeader *msg;
			CmsRet ret;
			ret = cmsMsg_receiveWithTimeout(msgHandle, &msg, 0);
			if (ret == CMSRET_SUCCESS) {
            switch (msg->type) {
            case CMS_MSG_DHCPD_RELOAD:
					/* Reload config file */
					write_leases(0);
					read_config(argc < 2 ? DHCPD_CONF_FILE : argv[1]);
					read_leases(server_config.lease_file);
               break;
            case CMS_MSG_WAN_CONNECTION_UP:
#ifdef SUPPORT_DHCP_RELAY
					/* Refind local addresses for relays */
					set_relays();
#endif
               break;
            case CMS_MSG_GET_LEASE_TIME_REMAINING:
               if (msg->dataLength == sizeof(GetLeaseTimeRemainingMsgBody)) {
					   GetLeaseTimeRemainingMsgBody *body = (GetLeaseTimeRemainingMsgBody *) (msg + 1);
					   u_int8_t chaddr[16]={0};

					   cur_iface = find_iface_by_ifname(body->ifName);
					   if (cur_iface != NULL) {
							   msg->dst = msg->src;
							   msg->src = EID_DHCPD;
							   msg->flags_request = 0;
							   msg->flags_response = 1;
						   cmsUtl_macStrToNum(body->macAddr, chaddr);
						   lease = find_lease_by_chaddr(chaddr);
						   if (lease != NULL) {
							   msg->wordData = lease_time_remaining(lease);
						   }else{
							   msg->wordData = 0;
						   }
							   msg->dataLength = 0;
							   cmsMsg_send(msgHandle, msg);
					   }
               }
               else {
                  LOG(LOG_ERR, "invalid msg, type=0x%x dataLength=%d", msg->type, msg->dataLength);
               }
               break;
            case CMS_MSG_EVENT_TIME_SYNC:               
               if (msg->dataLength == sizeof(long)) 
               {
                  long *delta = (long *) (msg + 1); 
                  adjust_lease_time(*delta);
               }
               else
               {
                  LOG(LOG_ERR, "Invalid CMS_MSG_EVENT_TIME_SYNC msg, dataLength=%d", msg->dataLength);
               }
               break;
            case CMS_MSG_QOS_DHCP_OPT60_COMMAND:
               bcmQosDhcp(DHCP_VENDOR, (char *)(msg+1));
               break;
            case CMS_MSG_QOS_DHCP_OPT77_COMMAND:
               bcmQosDhcp(DHCP_USER_CLASS_ID, (char *)(msg+1));
               break;
            default:
               LOG(LOG_ERR, "unrecognized msg, type=0x%x dataLength=%d", msg->type, msg->dataLength);
               break;
            }
				cmsMem_free(msg);
			} else if (ret == CMSRET_DISCONNECTED) {
				if (!cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS)) {
					LOG(LOG_ERR, "lost connection to smd, exiting now.");
				}
				exit_server(0);
			}
			continue;
		}
#ifdef SUPPORT_DHCP_RELAY
		/* Check packets from upper level DHCP servers */
		for (cur_relay = relay_config; cur_relay;
			cur_relay = cur_relay->next) {
			if (FD_ISSET(cur_relay->skt, &rfds))
				break;
		}
		if (cur_relay) {
			process_relay_response();
			continue;
		}
#endif

		/* Find out which interface is selected */
		for(cur_iface = iface_config; cur_iface;
			cur_iface = cur_iface->next ) {
			if (FD_ISSET(cur_iface->skt, &rfds))
				break;
		}
		if (cur_iface == NULL)
			continue;

		bytes = get_packet(&packet, cur_iface->skt); /* this waits for a packet - idle */
		if(bytes < 0)
			continue;

		/* Decline requests */
		if (cur_iface->decline)
			continue;

#ifdef SUPPORT_DHCP_RELAY
		/* Relay request? */
		if (cur_iface->relay_interface[0]) {
			process_relay_request((char*)&packet, bytes);
			continue;
		}
#endif

		if((state = get_option(&packet, DHCP_MESSAGE_TYPE)) == NULL) {
			DEBUG(LOG_ERR, "couldnt get option from packet -- ignoring");
			continue;
		}
		
		/* For static IP lease */
		/* Look for a static lease */
		static_lease_ip = getIpByMac(cur_iface->static_leases, &packet.chaddr);

		if(static_lease_ip)
		{
			memcpy(&static_lease.chaddr, &packet.chaddr, 16);
			static_lease.yiaddr = static_lease_ip;
			static_lease.expires = -1; /* infinite lease time */
			lease = &static_lease;
		} /* For static IP lease end */
		else
		{
			lease = find_lease_by_chaddr(packet.chaddr);
		}
		switch (state[0]) {
		case DHCPDISCOVER:
			DEBUG(LOG_INFO,"received DISCOVER");
			//BRCM_begin
			vendorid = (char *)get_option(&packet, DHCP_VENDOR);
			test_vendorid(&packet, vendorid, &declined);
			// BRCM_end

         if (!declined) {
				if (sendOffer(&packet) < 0) {
					LOG(LOG_DEBUG, "send OFFER failed -- ignoring");
				}
//brcm begin
            /* delete any obsoleted QoS rules because sendOffer() may have
             * cleaned up the lease data.
             */
            bcmDelObsoleteRules(); 
//brcm end
			}
			break;			
 		case DHCPREQUEST:
			DEBUG(LOG_INFO,"received REQUEST");

			requested = (char *)get_option(&packet, DHCP_REQUESTED_IP);
			server_id = (char *)get_option(&packet, DHCP_SERVER_ID);
			hostname  = (char *)get_option(&packet, DHCP_HOST_NAME);
			if (requested) memcpy(&requested_align, requested, 4);
			if (server_id) memcpy(&server_id_align, server_id, 4);
		
			//BRCM_begin
			vendorid = (char *)get_option(&packet, DHCP_VENDOR);
			test_vendorid(&packet, vendorid, &declined);
			// BRCM_end

			if (!declined) {
				if (lease) {
					if (hostname) {
						bytes = hostname[-1];
						if (bytes >= (int) sizeof(lease->hostname))
							bytes = sizeof(lease->hostname) - 1;
						strncpy(lease->hostname, hostname, bytes);
						lease->hostname[bytes] = '\0';
						send_lease_info(FALSE, lease);
					}  else
						lease->hostname[0] = '\0';             
					if (server_id) {
						/* SELECTING State */
						DEBUG(LOG_INFO, "server_id = %08x", ntohl(server_id_align));
						if (server_id_align == cur_iface->server && requested) {
                                                   if ( requested_align == lease->yiaddr) {
							sendACK(&packet, lease->yiaddr);
							send_lease_info(FALSE, lease);
//brcm begin
                     /* delete any obsoleted QoS rules because sendACK() may have
                      * cleaned up the lease data.
                      */
                     bcmDelObsoleteRules(); 
                     bcmExecOptCmd(); 
//brcm end
                                                   }
                                                   else sendNAK(&packet);
						}
					} else {
						if (requested) {
							/* INIT-REBOOT State */
							if (lease->yiaddr == requested_align) {
								sendACK(&packet, lease->yiaddr);
								send_lease_info(FALSE, lease);
//brcm begin
                        /* delete any obsoleted QoS rules because sendACK() may have
                         * cleaned up the lease data.
                         */
                        bcmDelObsoleteRules(); 
                        bcmExecOptCmd(); 
//brcm end
							}
							else sendNAK(&packet);
						} else {
							/* RENEWING or REBINDING State */
							if (lease->yiaddr == packet.ciaddr) {
								sendACK(&packet, lease->yiaddr);
								send_lease_info(FALSE, lease);
//brcm begin
                        /* delete any obsoleted QoS rules because sendACK() may have
                         * cleaned up the lease data.
                         */
                        bcmDelObsoleteRules(); 
                        bcmExecOptCmd(); 
//brcm end
							}
							else {
								/* don't know what to do!!!! */
								sendNAK(&packet);
							}
						}						
					}
				} else { /* else remain silent */				
    					sendNAK(&packet);
            			}
			}
			break;
		case DHCPDECLINE:
			DEBUG(LOG_INFO,"received DECLINE");
			if (lease) {
				memset(lease->chaddr, 0, 16);
				lease->expires = time(0) + server_config.decline_time;
			}			
			break;
		case DHCPRELEASE:
			DEBUG(LOG_INFO,"received RELEASE");
			if (lease) {
				lease->expires = time(0);
				send_lease_info(TRUE, lease);
//brcm begin
            sleep(1);
            /* delete the obsoleted QoS rules */
            bcmDelObsoleteRules(); 
//brcm end
			}
			break;
		case DHCPINFORM:
			DEBUG(LOG_INFO,"received INFORM");
			send_inform(&packet);
			break;	
		default:
			LOG(LOG_WARNING, "unsupported DHCP message (%02x) -- ignoring", state[0]);
		}
	}
	return 0;
}

