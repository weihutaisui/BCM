#ifndef _CLIENTPACKET_H
#define _CLIENTPACKET_H

unsigned long random_xid(void);
int send_discover(unsigned long xid, unsigned long requested, unsigned long lease);
int send_selecting(unsigned long xid, unsigned long server,
                   unsigned long requested, unsigned long lease);
int send_renew(unsigned long xid, unsigned long server,
               unsigned long ciaddr, unsigned long lease);
int send_release(unsigned long server, unsigned long ciaddr);
int get_raw_packet(struct dhcpMessage *payload, int fd);

#endif
