#ifndef __DNS_MAPPING_
#define __DNS_MAPPING_

/*
 * Read  "/var/dnsinfo.conf" and put the dns mapping info into a linked list.
 * Is called whenever the dnsinfo.conf changes (caused by  WAN connection status change)
 */
void dns_mapping_conifg_init(void);


/*
 * Find a WAN side dns ip from the lan Ip and source port.
 *  
 *
 * @param lanInfo     (IN) LAN ip of the dns query
 * @param queryType   (IN) Source port of the dns query 
 * @param dns1        (OUT) the primary dns ip
 * @param proto       (OUT) layer 3 protocol (IPv4 or IPv6)
 *
 */
UBOOL8 dns_mapping_find_dns_ip(struct sockaddr_storage *lanInfo,
                               int queryType, char *dns1, int *proto);


/*
 * Find out if the dns request has a dns2 to be swapped with dns 1
 *  
 *
 * @param r     (IN) a timed out request
 * @param dnsUsed   (IN/OUT) First time call, it is null and subsequent and then gets filled. Later calls it is used for comparison.
 */
UBOOL8 dns_mapping_is_dns_swap_needed(dns_request_t *r, char *dnsUsed);

/*
 * Swap the dns1 and dns2 in the linked list
 * @param dnsUsed     (IN) dnsUsed in the request
 *
 */
void dns_mapping_do_dns_swap(char *dnsUsed);

#endif
