/* script.c
 *
 * Functions to call the DHCP client notification scripts 
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
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

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "options.h"
#include "dhcpd.h"
#include "dhcpc.h"
#include "packet.h"
#include "options.h"
#include "debug.h"
#include "cms.h"
#include "cms_msg.h"

// brcm
static DhcpcStateChangedMsgBody dhcpc_opt = 
    {0, 0, "", "", "", "", "", 0, 0, "", "", "", "", "", "", 0, 0};

extern void sendEventMessage(UBOOL8 assigned, UBOOL8 isExpired, DhcpcStateChangedMsgBody *options);


/** Extract the specified Encapsulated Vendor-Specific Option from the given
 * option string.  This is described in TR-069 Amendment 3 page 24 and also
 * RFC-2132 page 19.
 *
 * This function is used by dhcpcv4 and dhcpcv6
 *
 * @param option (IN) points to the beginning of the option 43/17 option.
 * @param len    (IN) is the total length of the option 43/17 option.
 * @param sub_option_num (IN) is the desired vendor specific sub-option:
 * 1 = URL of ACS
 * 2 = Provisioning Code
 * 3 = CWMP retry-minimum wait interval
 * 4 = CWMP retry interval multiplier
 * 255 = optional end of sub-options marker
 *
 * @param sub_option_offset (OUT) only valid if return val is 1
 * @param sub_option_len    (OUT) only valid if return val is 1
 *
 * @return 1 if the specified vendor option is found, else return 0.
 * If return value is 1, option[sub_option_offset] will be the beginning of the
 * desired option and sub_option_len is the length of the sub-option.
 */
static UBOOL8 findEncapVendorSpecificOption(const unsigned char *option,
                                int len, unsigned char sub_option_num,
                                int *sub_option_offset, int *sub_option_len)
{
   int i=0;
   unsigned char curr_sub_option_num;
   int curr_sub_option_len;

   while (i < len)
   {
      curr_sub_option_num = option[i];
      if (255 == curr_sub_option_num)
      {
         return FALSE;
      }

      /* sanity check */
      curr_sub_option_len = (int) option[i+1];
      if (i + 2 + curr_sub_option_len > len)
      {
         cmsLog_error("sub-option exceeds len, %d %d %d",
                      i, curr_sub_option_len, len);
         return FALSE;
      }

      if (sub_option_num == curr_sub_option_num)
      {
         *sub_option_offset = i+2;
         *sub_option_len = curr_sub_option_len;
         return TRUE;
      }

      i += 2 + curr_sub_option_len;  /* advance i to the next sub-option */
   }

   return FALSE;
}


/* get a rough idea of how long an option will be (rounding up...) */
static int max_option_length(char *option, struct dhcp_option *type)
{
	int size = 0;
	
	switch (type->flags & TYPE_MASK) {
	case OPTION_IP:
	case OPTION_IP_PAIR:
		size = (option[OPT_LEN - 2] / 4) * sizeof("255.255.255.255 ");
		break;
	case OPTION_STRING:
		size = option[OPT_LEN - 2] + 1;
		break;
	case OPTION_BOOLEAN:
		size = option[OPT_LEN - 2] * sizeof("yes ");
		break;
	case OPTION_U8:
		size = option[OPT_LEN - 2] * sizeof("255 ");
		break;
	case OPTION_U16:
		size = (option[OPT_LEN - 2] / 2) * sizeof("65535 ");
		break;
	case OPTION_S16:
		size = (option[OPT_LEN - 2] / 2) * sizeof("-32768 ");
		break;
	case OPTION_U32:
		size = (option[OPT_LEN - 2] / 4) * sizeof("4294967295 ");
		break;
	case OPTION_S32:
		size = (option[OPT_LEN - 2] / 4) * sizeof("-2147483684 ");
		break;
	}
	
	return size;
}


/* Fill dest with the text of option 'option'. */
static void fill_options(char *dest, unsigned char *option, struct dhcp_option *type_p)
{
	int type, optlen;
	u_int16_t val_u16;
	int16_t val_s16;
	u_int32_t val_u32;
	int32_t val_s32;
	int len = option[OPT_LEN - 2];
        // brcm
	char tmp[128]="";

	dest += sprintf(dest, "%s=", type_p->name);

	type = type_p->flags & TYPE_MASK;
	optlen = option_lengths[type];
	for(;;) {
		switch (type) {
		case OPTION_IP:	/* Works regardless of host byte order. */
			dest += sprintf(dest, "%d.%d.%d.%d",
					option[0], option[1],
					option[2], option[3]);
		        // brcm
		        sprintf(tmp, "%d.%d.%d.%d",
				option[0], option[1],
				option[2], option[3]);
			if (!strcmp(type_p->name, "dns")) {
			    // cwu
             if (strlen(dhcpc_opt.nameserver) > 0)
			    {
			        strcat(dhcpc_opt.nameserver, ",");
             }
             strcat(dhcpc_opt.nameserver, tmp);
			}
			if (!strcmp(type_p->name, "router"))
			    strcpy(dhcpc_opt.gateway, tmp);
			if (!strcmp(type_p->name, "subnet"))
			    strcpy(dhcpc_opt.mask, tmp);
 			break;
		case OPTION_IP_PAIR:
			dest += sprintf(dest, "%d.%d.%d.%d, %d.%d.%d.%d",
					option[0], option[1],
					option[2], option[3],
					option[4], option[5],
					option[6], option[7]);
			break;
		case OPTION_BOOLEAN:
			dest += sprintf(dest, *option ? "yes" : "no");
			break;
		case OPTION_U8:
			dest += sprintf(dest, "%u", *option);
			break;
		case OPTION_U16:
			memcpy(&val_u16, option, 2);
			dest += sprintf(dest, "%u", ntohs(val_u16));
			break;
		case OPTION_S16:
			memcpy(&val_s16, option, 2);
			dest += sprintf(dest, "%d", ntohs(val_s16));
			break;
		case OPTION_U32:
			memcpy(&val_u32, option, 4);
			dest += sprintf(dest, "%lu", (unsigned long) ntohl(val_u32));
			break;
		case OPTION_S32:
			memcpy(&val_s32, option, 4);
			dest += sprintf(dest, "%ld", (long) ntohl(val_s32));
			break;
		case OPTION_STRING:
			memcpy(dest, option, len);

			// brcm
			if ( !strcmp(type_p->name, "6rd") )
			{
				uint16_t *ptr16;

				dhcpc_opt.ipv4MaskLen = (int)option[0];
				dhcpc_opt.ipv6PrefixLen = (int)option[1];
				ptr16 = (uint16_t*)&option[2];
                     
				sprintf(dhcpc_opt.prefix, "%x:%x:%x:%x:%x:%x:%x:%x",
							ntohs(ptr16[0]), ntohs(ptr16[1]), ntohs(ptr16[2]),
							ntohs(ptr16[3]), ntohs(ptr16[4]), ntohs(ptr16[5]),
							ntohs(ptr16[6]), ntohs(ptr16[7]));
                     
				sprintf(dhcpc_opt.brAddr, "%d.%d.%d.%d",
							option[18], option[19],
							option[20], option[21]);
			}
			else if ( !strcmp(type_p->name, "vendinfo") )
			{
				unsigned char sub_option_num=1;
				int sub_option_offset=0;
				int sub_option_len=0;

				if (findEncapVendorSpecificOption(option, len,
				                         sub_option_num,
				                         &sub_option_offset, &sub_option_len))
				{
					int copyLen = sizeof(dhcpc_opt.acsURL) - 1;
					if (copyLen > sub_option_len) copyLen=sub_option_len;

					memcpy(dhcpc_opt.acsURL, &option[sub_option_offset], copyLen);
					dhcpc_opt.acsURL[copyLen] = '\0';
					cmsLog_debug("vendor specific info detected %s (len=%d)",
					             dhcpc_opt.acsURL, copyLen);
				}
			}
            else if ( !strcmp(type_p->name, "hostname") )
            {
                int copyLen = sizeof(dhcpc_opt.hostName) - 1;
                
                if (copyLen > len) copyLen=len;
                memcpy(dhcpc_opt.hostName, option, copyLen);
                dhcpc_opt.hostName[copyLen] = '\0';
            }
            else if ( !strcmp(type_p->name, "domain") )
            {
                int copyLen = sizeof(dhcpc_opt.domain) - 1;
                
                if (copyLen > len) copyLen=len;
                memcpy(dhcpc_opt.domain, option, copyLen);
                dhcpc_opt.domain[copyLen] = '\0';
            }

			dest[len] = '\0';
			return;	 /* Short circuit this case */
		}
		option += optlen;
		len -= optlen;
		if (len <= 0) break;
		*(dest++) = ' ';
	}
}


static char *find_env(const char *prefix, char *defaultstr)
{
	extern char **environ;
	char **ptr;
	const int len = strlen(prefix);

	for (ptr = environ; *ptr != NULL; ptr++) {
		if (strncmp(prefix, *ptr, len) == 0)
		return *ptr;
	}
	return defaultstr;
}


/* put all the paramaters into an environment */
static char **fill_envp(struct dhcpMessage *packet)
{
	int num_options = 0;
	int i, j;
	unsigned char *addr;
	char **envp, *temp;
	char over = 0;

	if (packet == NULL)
		num_options = 0;
	else {
		for (i = 0; options[i].code; i++)
			if (get_option(packet, options[i].code))
				num_options++;
		if (packet->siaddr) num_options++;
		if ((temp = (char *)get_option(packet, DHCP_OPTION_OVER)))
			over = *temp;
		if (!(over & FILE_FIELD) && packet->file[0]) num_options++;
		if (!(over & SNAME_FIELD) && packet->sname[0]) num_options++;		
	}
	
	envp = malloc((num_options + 5) * sizeof(char *));
	envp[0] = malloc(strlen("interface=") + strlen(client_config.interface) + 1);
	sprintf(envp[0], "interface=%s", client_config.interface);
	envp[1] = malloc(sizeof("ip=255.255.255.255"));
	envp[2] = find_env("PATH", "PATH=/bin:/usr/bin:/sbin:/usr/sbin");
	envp[3] = find_env("HOME", "HOME=/");

	if (packet == NULL) {
		envp[4] = NULL;
		return envp;
	}

	addr = (unsigned char *) &packet->yiaddr;
	sprintf(envp[1], "ip=%d.%d.%d.%d",
		addr[0], addr[1], addr[2], addr[3]);
	// brcm
	sprintf(dhcpc_opt.ip, "%d.%d.%d.%d",
		addr[0], addr[1], addr[2], addr[3]);
	strcpy(dhcpc_opt.nameserver, "");
	memset(dhcpc_opt.hostName, 0, sizeof(dhcpc_opt.hostName));
	memset(dhcpc_opt.domain, 0, sizeof(dhcpc_opt.domain));
	if (is_opt_req_map_used) {
		for (i = DHCP_PADDING; i < DHCP_END; i++) {
			if (opt_req_map[i]) {
				if ((temp = (char *)get_option(packet, i))) {
					LOG(LOG_INFO, "DHCP option %d", i);
					SaveOptionX(i, temp - VENDOR_OPTION_SUBCODE_OFFSET,
								*(temp - 1) + VENDOR_OPTION_SUBCODE_OFFSET);
				} else {
					LOG(LOG_ERR, "NO DHCP option %d", i);
				}
			}
		}
	}

	for (i = 0, j = 4; options[i].code; i++) {
		if ((temp = (char *)get_option(packet, options[i].code))) {
			envp[j] = malloc(max_option_length(temp, &options[i]) + 
					strlen(options[i].name) + 2);
			fill_options(envp[j], (unsigned char *)temp, &options[i]);
			j++;
		}
	}
	if (packet->siaddr) {
		envp[j] = malloc(sizeof("siaddr=255.255.255.255"));
		addr = (unsigned char *) &packet->siaddr;
		sprintf(envp[j++], "siaddr=%d.%d.%d.%d",
			addr[0], addr[1], addr[2], addr[3]);
	}
	if (!(over & FILE_FIELD) && packet->file[0]) {
		/* watch out for invalid packets */
		packet->file[sizeof(packet->file) - 1] = '\0';
		envp[j] = malloc(sizeof("boot_file=") + strlen((char *)(packet->file)));
		sprintf(envp[j++], "boot_file=%s", packet->file);
	}
	if (!(over & SNAME_FIELD) && packet->sname[0]) {
		/* watch out for invalid packets */
		packet->sname[sizeof(packet->sname) - 1] = '\0';
		envp[j] = malloc(sizeof("sname=") + strlen((char *)(packet->sname)));
		sprintf(envp[j++], "sname=%s", packet->sname);
	}	
	envp[j] = NULL;

   SaveEnv(envp); /* SaveEnv before sendEventMessage */
   sendEventMessage(TRUE, FALSE, &dhcpc_opt);

	return envp;
}


/* Call a script with a par file and env vars */
void run_script(struct dhcpMessage *packet, const char *name)
{
	char **envp;

   if (!strcmp(name, "bound"))
   {
      envp = fill_envp(packet);
      free(*envp);
   }else if (!strcmp(name, "deconfig")) {
      /* brcm: Expired, set the connection down */
      sendEventMessage(FALSE, TRUE, &dhcpc_opt);
   }

}
