#ifndef EVENT_SOCK_H
#define EVENT_SOCK_H

#include <stdint.h>

#define EVENT_SOCK_NAMED_PIPE "/tmp/openl2tpd.evt"

#define L2TP_EVENT_SESSION_CREATED (1 << 0)
#define L2TP_EVENT_SESSION_DELETED (1 << 1)
#define L2TP_EVENT_SESSION_UP      (1 << 2)
#define L2TP_EVENT_SESSION_DOWN    (1 << 3)

struct l2tp_event_hdr {
	uint16_t event;
	uint16_t len;
	uint8_t data[0];
};

#endif /* EVENT_SOCK_H */
