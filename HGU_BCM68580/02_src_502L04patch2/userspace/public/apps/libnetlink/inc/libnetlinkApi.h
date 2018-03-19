#ifndef __LIBNETLINKAPI_H__
#define __LIBNETLINKAPI_H__ 1

#ifndef IPSEC_PROTO_ESP
#define IPSEC_PROTO_ESP				50
#endif

#ifndef IPSEC_PROTO_AH
#define IPSEC_PROTO_AH				51
#endif

#define IPSEC_RTA_BUF_SIZE			1024
#define XFRM_TMPLS_BUF_SIZE			512
#define IPSEC_RTA_BUF_SIZE_DELETE	256

#define BRCM_IPSECPFD_SPI_PS	1		// ue_c -> p_s
#define BRCM_IPSECPFD_SPI_US	2		// p_c -> ue_s
#define BRCM_IPSECPFD_SPI_PC	3		// ue_s -> p_c
#define BRCM_IPSECPFD_SPI_UC	4		// p_s -> ue_c
#define BRCM_IPSECPFD_SA_COUNT	4		// 4 SAs to be added to kernel SA DB as mentioned above
#define BRCM_IPSECPFD_SP_COUNT	4		// 2 SAs to be added to kernel SP DB INBOUND & OUTBOUND direction
#define BRCM_IPSEC_BITS			8

#define BRCM_IPSECPFD_SRV_INBOUND_DIR		1		//  INBOUND :: Pcscf -> UE
#define BRCM_IPSECPFD_CLNT_OUTBOUND_DIR		2	//  OUTBOUND :: UE --> Pcscf
#define BRCM_IPSECPFD_SRV_OUTBOUND_DIR		3	//  OUTBOUND :: UE --> Pcscf_s
#define BRCM_IPSECPFD_CLNT_INBOUND_DIR		4		//  INBOUND :: Pcscf -> UE_s

#define BRCM_IPSEC_DES_KEY_EXTRA_LEN	8
#define BRCM_IPSEC_MD5_KEY_EXTRA_LEN	4

#define IP_ADDR_LEN             64
#define MAX_IKEY_LEN            32
#define MAX_CKEY_LEN            32
#define MAX_AUTH_LEN            16
#define MAX_ENCRYPT_LEN         16

#define LIBNETLINK_SERVER_SOCK_PATH   "/data/libnetlink_server.sock"
#define LIBNETLINK_CLIENT_SOCK_PATH   "/data/libnetlink_client.sock"

typedef enum 
{
    LIBNETLINK_STATUS_REQUEST,
    LIBNETLINK_STATUS_OK,
	LIBNETLINK_STATUS_ERROR,
    LIBNETLINK_OPEN,
    LIBNETLINK_CLOSE,
	LIBNETLINK_ADD_SA,
	LIBNETLINK_DEL_SA,
    LIBNETLINK_ADD_SP,
    LIBNETLINK_DEL_SP
} eLibnetlinkMsgType_t;

typedef enum 
{
   LIBNETLINK_SUCCESS = 0,
   LIBNETLINK_ERROR	= -1,
   LIBNETLINK_NOTSUPP = -2,
   LIBNETLINK_INVALIDPARMS = -3,
   LIBNETLINK_INTERR = -4,
} eLibnetlinkStatus_t;

enum {
	IPSEC_PFD_POLICY_IN		= 0,
	IPSEC_PFD_POLICY_OUT	= 1,
	IPSEC_PFD_POLICY_FWD	= 2,
	IPSEC_PFD_POLICY_MASK	= 3,
	IPSEC_PFD_POLICY_MAX	= 3
};

typedef struct _BrcmIPSecAddSA_t_
{
	unsigned char	ucProto;	//ESP;
	unsigned char	ucMode;		//XFRM_MODE_TRANSPORT;	//XFRM_MODE_TUNNEL;
	unsigned int	usSrcPort;
	unsigned int	usDestnPort;
	unsigned int	usFamily;	//AF_INET/ AF_INET6
	unsigned int	uiSpi;
	unsigned char	srcIP[IP_ADDR_LEN];
	unsigned char	destnIP[IP_ADDR_LEN];
} BrcmIPSecAddSA_t, BrcmIPSecAddSP_t, BrcmIPSecDeleteSP_t;


typedef struct _BrcmIPSecKeys_t_
{
	unsigned char	auth[MAX_AUTH_LEN];
	unsigned char	IK[MAX_IKEY_LEN];
	unsigned int	uIKLength;
	unsigned char	encrypt[MAX_ENCRYPT_LEN];
	unsigned char	CK[MAX_CKEY_LEN];
	unsigned int	uCKLength;
} BrcmIPSecKeys_t;

typedef struct _BrcmIPSecDeleteSA_t_
{
	unsigned char	ucProto;	//ESP;
	unsigned int	usFamily;
	unsigned int	uiSpi;
	unsigned char	srcIP[IP_ADDR_LEN];
	unsigned char	destnIP[IP_ADDR_LEN];
} BrcmIPSecDeleteSA_t;

typedef struct _BrcmIPSecSktReq_t_{
	struct nlmsghdr			n;
	struct xfrm_usersa_info	xsinfo;
	char	buf[IPSEC_RTA_BUF_SIZE];
} BrcmIPSecSktReq_t;

typedef struct _BrcmIPSecDeleteSASktReq_t_
{
	struct nlmsghdr			n;
	struct xfrm_usersa_id	xsid;
	char	buf[IPSEC_RTA_BUF_SIZE_DELETE];
} BrcmIPSecDeleteSASktReq_t;

typedef struct _BrcmIPSecAddSPSktReq_t
{
	struct nlmsghdr				n;
	struct xfrm_userpolicy_info	xpinfo;
	char	buf[IPSEC_RTA_BUF_SIZE];
} BrcmIPSecAddSPSktReq_t;

typedef struct _BrcmRtnlDeleteSP_t_
{
	struct nlmsghdr				n;
	struct xfrm_userpolicy_id	xpid;
	char	buf[IPSEC_RTA_BUF_SIZE_DELETE];
} BrcmRtnlDeleteSP_t;

typedef struct _libnetlinkAppMsg_t_
{
   	eLibnetlinkMsgType_t msgType;
	
	union
	{
       BrcmIPSecAddSA_t ipSecAddSa;
       BrcmIPSecAddSP_t ipSecAddSp;
       BrcmIPSecDeleteSA_t ipSecDelSa;
       BrcmIPSecDeleteSP_t ipSecDelSp;
	} libnetlinkMsgContent;

	BrcmIPSecKeys_t ipSecKeys;
    unsigned char		ucDir;

} libnetlinkAppMsg_t;

#endif
