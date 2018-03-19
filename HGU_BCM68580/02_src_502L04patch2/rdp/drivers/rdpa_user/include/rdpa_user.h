#ifndef _RDPA_USER_H_
#define _RDPA_USER_H_

#include <bcmtypes.h> /* BCM_IOC_PTR */
#include <bdmf_system.h> /* bdmf_handles */
#include <bdmf_interface.h> /* bdmf_handles */

#define DRIVER_IOCTL 0x11
#define RDPA_USR_DEV_NAME "/dev/rdpa_user"

typedef struct {
    int32_t                         ret;     
    BCM_IOC_PTR(bdmf_type_handle,   drv);    
    BCM_IOC_PTR(bdmf_object_handle, mo);
    BCM_IOC_PTR(bdmf_object_handle, object);
    uint32_t                        cmd;
    uint32_t                        ptr;
    uint64_t                        parm;
    uint32_t                        ai_ptr;
    bdmf_index                      ai;
    uint32_t                        size;
        
} rdpa_ioctl_cmd_t;


typedef struct {
    int32_t                         ret;   
    BCM_IOC_PTR(bdmf_type_handle,   drv);
    BCM_IOC_PTR(bdmf_object_handle, mo_or_us);
    BCM_IOC_PTR(bdmf_object_handle, owner_or_ds);
    BCM_IOC_PTR(bdmf_mattr_handle,  mattr);
    BCM_IOC_PTR(bdmf_link_handle,   link);
    BCM_IOC_PTR(const char *,       str);
} bdmf_ioctl_t;


typedef union {
    bdmf_ioctl_t bdmf_pa;
    rdpa_ioctl_cmd_t rdpa_pa;
} ioctl_pa_t;

#define RDPA_PORT_IOCTL         _IOWR(DRIVER_IOCTL, 13, ioctl_pa_t)
#define RDPA_EGRESS_TM_IOCTL    _IOWR(DRIVER_IOCTL, 14, ioctl_pa_t)
#define RDPA_TCONT_IOCTL        _IOWR(DRIVER_IOCTL, 15, ioctl_pa_t)
#define RDPA_VLAN_IOCTL         _IOWR(DRIVER_IOCTL, 16, ioctl_pa_t)
#define RDPA_VLAN_ACTION_IOCTL  _IOWR(DRIVER_IOCTL, 17, ioctl_pa_t)
#define RDPA_SYSTEM_IOCTL       _IOWR(DRIVER_IOCTL, 18, ioctl_pa_t)
#define RDPA_IPTV_IOCTL         _IOWR(DRIVER_IOCTL, 19, ioctl_pa_t)
#define RDPA_TC_TO_QUEUE_IOCTL  _IOWR(DRIVER_IOCTL, 20, ioctl_pa_t)

#define PORT_OBJECT 1
#define SYSTEM_OBJECT 1
#define EGRESS_TM_OBJECT 1

#if defined(BCM_PON) || defined(CONFIG_BCM_PON)
#define VLAN_OBJECT 1
#define VLAN_ACTION_OBJECT 1
#define IPTV_OBJECT 1
#define TC_TO_QUEUE_OBJECT 1
#endif /* defined(BCM_PON) || defined(CONFIG_BCM_PON)*/

#if defined(BCM_PON) || defined(CONFIG_BCM_PON) || defined(CHIP_63158)
#define TCONT_OBJECT 1
#endif /* defined(BCM_PON) || defined(CONFIG_BCM_PON) || defined(CHIP_63158) */

#ifndef __KERNEL__ 

#define CC_RDPA_USR_DEBUG 1 

#if defined(CC_RDPA_USR_DEBUG)
#define rdpa_usr_debug(fmt, arg...) printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define rdpa_usr_debug(fmt, arg...)
#endif

#define rdpa_usr_error(fmt, arg...) printf("ERROR[%s.%u]: " fmt "\n", __FUNCTION__, __LINE__, ##arg)

#endif /* __KERNEL__ */
#endif /* _RDPA_USER_H_ */
