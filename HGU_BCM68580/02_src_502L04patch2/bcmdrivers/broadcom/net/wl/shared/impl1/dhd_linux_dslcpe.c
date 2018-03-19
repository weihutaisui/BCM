/*
    Copyright (c) 2017 Broadcom
    All Rights Reserved

    <:label-BRCM:2017:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

    :>
*/

/* This is to customize dhd_linux for DSLCPE */

#include <typedefs.h>
#include <linuxver.h>
#include <board.h>
#include <osl.h>

#if defined(BCM_BLOG)
#include <linux/blog.h>
#include <bcm_mcast.h>
#endif
#include <epivers.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmdevs.h>
#include <siutils.h>
#include <dngl_stats.h>
#include <pcie_core.h>
#include <dhd.h>
#include <dhd_bus.h>
#include <dhd_flowring.h>
#include <dhd_proto.h>
#include <dhd_dbg.h>
#include <dhdioctl.h>
#include <sdiovar.h>
#include <bcmmsgbuf.h>
#include <dhd_pcie.h>

#include <dhd_linux_dslcpe.h>

char mfg_firmware_path[MOD_PARAM_PATHLEN];

module_param_string(mfg_firmware_path, mfg_firmware_path, MOD_PARAM_PATHLEN, 0660);

#if defined(BCM_DHD_RUNNER)
extern void  dhd_clear_stats(struct net_device *net);
#endif /* BCM_DHD_RUNNER */

void
update_firmware_path(struct dhd_bus *bus, char *pfw_path)
{
	int nodhdhdr_flag = 0;
#if defined(BCM_DHD_RUNNER)
	dhd_helper_status_t  dhd_rnr_status;
#endif

	DHD_ERROR(("%s: pfw_path = %s\n", __FUNCTION__, pfw_path));
	if ((pfw_path == NULL) || (strstr(pfw_path, ".bea") != NULL)) {
		return;
	}

	switch(bus->sih->chip){
	CASE_BCM43602_CHIP: 
	    strcat((char *)pfw_path, "/43602");
	    break;
	case BCM4365_CHIP_ID:
	case BCM4366_CHIP_ID:
	case BCM43664_CHIP_ID:
	case BCM43666_CHIP_ID:
#if defined(BCM4363_CHIP_ID)
	case BCM4363_CHIP_ID:
#endif /* BCM4363_CHIP_ID */
	    strcat((char *)pfw_path, "/4366");
	    break;
	default: DHD_ERROR(("%s: no CHIP ID matches\n", __FUNCTION__));
	}

	switch(bus->sih->chiprev){
	case 0:
	    strcat((char *)pfw_path, "a0");
	    break;
	case 1:
	    strcat((char *)pfw_path, "a1");
	    break;
	case 2:
	    strcat((char *)pfw_path, "b0");
	    break;
	case 3:
	    strcat((char *)pfw_path, "a3");
	    break;
	case 4:
	    strcat((char *)pfw_path, "c0");
	    break;
	default: DHD_ERROR(("%s: no CHIP REV matches\n", __FUNCTION__));;
	}

#if defined(BCM_DHDHDR) && defined(BCM_DHD_RUNNER)
	if (dhd_runner_do_iovar(bus->dhd->runner_hlp,
				DHD_RNR_IOVAR_STATUS,
				0, (char*)(&dhd_rnr_status),
				sizeof(dhd_rnr_status)) == BCME_OK) {
		if ((dhd_rnr_status.sup_features.txoffl == 1) &&
		    (dhd_rnr_status.sup_features.dhdhdr == 0)) {
			DHD_ERROR(("%s: DHDHDR not supported by runner, using nodhdhdr dongle\n", __FUNCTION__));
			nodhdhdr_flag = 1;
		}
	}
#endif /* BCM_DHD_RUNNER && BCM_DHDHDR */

	if (BCM4365_CHIP(bus->sih->chip) ||
	    (BCM43602_CHIP(bus->sih->chip) && (bus->sih->chiprev == 3))) {
		if (nodhdhdr_flag)
			strcat((char*)pfw_path, "/rtecdc_nodhdhdr.bin");
		else
			strcat((char*)pfw_path, "/rtecdc.bin");
	} else {
		strcat((char*)pfw_path, "/rtecdc.bin");
	}
}

int dhd_vars_adjust(struct dhd_bus *bus, char *memblock, uint *total_len)
{
	int bcmerror = BCME_OK;
	if(bus->vars) {
		uint len=*total_len;	
		uint i=len-1;
		char *locbufp=bus->vars;
		while(i>=0 && memblock[i]=='\0') i--;
		len=i+2;

		for(i=0; i < bus->varsz; i++, locbufp++){
		        if(*locbufp == '\0')
			        break;
			while(*locbufp != '\0') {
			        i++;
				locbufp++;
			}
		}

		if ((len+i) < MAX_NVRAMBUF_SIZE) {
			memcpy(memblock+len,bus->vars,i);
			*total_len=len+i;
		} else {
			DHD_ERROR(("%s:nvram size %d is bigger than max:%d \n",__FUNCTION__,len+bus->varsz,MAX_NVRAMBUF_SIZE));
			bcmerror = BCME_ERROR;
		}
	}
	return bcmerror;
}


/*
 * Get the mutxmax nvram variable
 * For 6838/6848/47189 platforms set the default value (0) if not available
 */
char*
dhd_get_nvram_mutxmax(dhd_pub_t *dhd)
{
	char *var = nvram_get("mutxmax");

#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848) || defined(CONFIG_BCM947189)
	/* Add a check for 4366chip if needed */
	if (var == NULL) {
	    /* Update with default value, so later code can access it */
	    nvram_set("mutxmax", "0");
	    DHD_ERROR(("mutxmax nvram is not set, using platform default \r\n"));
	    var = nvram_get("mutxmax");
	}
#endif /* CONFIG_BCM96838 || CONFIG_BCM96848 || CONFIG_BCM947189 */

	return var;
}

/*
 * Check and update mutxmax var for dongle
 */
int 
dhd_check_and_set_mutxmax(dhd_pub_t *dhd, char *memblock, uint* len)
{
#define MUTXMAXBUFLEN        12
	char *var = NULL;
	char mutxmaxbuf[MUTXMAXBUFLEN]="mutxmax=";
	char *locbufp;
	int mutxmax;

	/* If mutxmax nvram is present, need to update the nvramvars */
	/* skip if it is set to auto mode (default) */
	var = nvram_get("mutxmax");
	if (var != NULL) {
	    mutxmax = bcm_strtoul(var, NULL, 0);
	    if (mutxmax != -1) {
	        if((*len)+12 >= MAX_NVRAMBUF_SIZE) {
	            DHD_ERROR(("%s: nvram size is too big \n", __FUNCTION__));
	            return BCME_ERROR;
	        }

	        /* memblock delimiter is END\0 string */
	        /* Move END by 12bytes to fit the mutxmax var */
	        locbufp = memblock+(*len)-4;
	        memcpy(locbufp+MUTXMAXBUFLEN, locbufp, 4);

	        /* append mutxmax var to the list */
	        sprintf(mutxmaxbuf+strlen(mutxmaxbuf), "0x%1x", mutxmax);
	        memcpy(locbufp, mutxmaxbuf, MUTXMAXBUFLEN);
	        (*len) = (*len) + MUTXMAXBUFLEN;
	    }
	}

	return  BCME_OK;
}

int 
dhd_check_and_set_mac(dhd_pub_t *dhd, char *memblock, uint* len)
{
	char *locbufp=memblock;
	const char macaddr00[]="macaddr=00:00:00:00:00:00";
	const char macaddr11[]="macaddr=11:11:11:11:11:11";
	char macaddrbuf[26]="macaddr=";
	int i,j;
	int appendMac=1;
	int replaceMac=0;
	struct ether_addr mac;

	for(i=0; i < (*len); i++, locbufp++){
		if(*locbufp == '\0')
			break;
		if(memcmp(locbufp, macaddrbuf, 7) == 0) {
			appendMac = 0;
			if(memcmp(locbufp, macaddr00, 25) == 0 || memcmp(locbufp, macaddr11, 25) == 0)
				replaceMac = 1;
			break;
		}
		while(*locbufp != '\0') {
			i++;
			locbufp++;
		}
	}
	
	if(appendMac && (*len)+26>= MAX_NVRAMBUF_SIZE) {
		DHD_ERROR(("%s: nvram size is too big \n", __FUNCTION__));
		return BCME_ERROR;
	} else if(appendMac || replaceMac) {
		printf("Replace or append with internal Mac Address\n");
		dhd_get_cfe_mac(dhd, &mac);
		for (j = 0; j < (ETH_ALEN-1); j++)
			sprintf(macaddrbuf+strlen(macaddrbuf),"%2.2X:",mac.octet[j]);
		sprintf(macaddrbuf+strlen(macaddrbuf),"%2.2X",mac.octet[j]);
		memcpy(memblock+i, macaddrbuf, 25);
		if(appendMac)
			(*len) = (*len) + 26;
	}

	return  BCME_OK;
}

void 
dhd_get_cfe_mac(dhd_pub_t *dhd, struct ether_addr *mac)
{
	unsigned long ulId = (unsigned long)('w'<<24) + (unsigned long)('l'<<16) + dhd_get_instance(dhd);	
	kerSysGetMacAddress(mac->octet, ulId);
}

/* reset DHD counters
 * - Reset interface counters
 * - Reset DHD public counters
 * - Reset bus counters
 * - Reset net rx_dropped count
*/
void
dhd_reset_cnt(struct net_device *net) 
{
	dhd_pub_t *dhdp = dhd_dev_get_dhdpub(net);
	int ifidx;

	/* Get the interface index */
	ifidx = dhd_dev_get_ifidx(net);
	if (ifidx == DHD_BAD_IF) {
		/* Set to main interface */
		ifidx = 0;
	}

	/* Clear the interface statistics */
	dhd_if_clear_stats(dhdp, ifidx);
#if defined(BCM_DHD_RUNNER)
	/* Clear interface b, c, d, m stats */
	dhd_clear_stats(net); 
#endif /* BCM_DHD_RUNNER */

	/* Clear the net stack rx_dropped pkts count as well */
	/* This count gets added by the network stack for stats */ 
	atomic_long_set(&net->rx_dropped, 0);

	/* Clear DHD public counters */
	dhdp->tx_packets = dhdp->rx_packets = 0;
	dhdp->tx_errors = dhdp->rx_errors = 0;
	dhdp->tx_ctlpkts = dhdp->rx_ctlpkts = 0;
	dhdp->tx_ctlerrs = dhdp->rx_ctlerrs = 0;
	dhdp->tx_dropped = 0;
	dhdp->rx_dropped = 0;
	dhdp->tx_pktgetfail = 0;
	dhdp->rx_pktgetfail = 0;
	dhdp->rx_readahead_cnt = 0;
	dhdp->tx_realloc = 0;
	dhdp->wd_dpc_sched = 0;
	memset(&dhdp->dstats, 0, sizeof(dhdp->dstats));
	dhd_bus_clearcounts(dhdp);
#ifdef PROP_TXSTATUS
	/* clear proptxstatus related counters */
	dhd_wlfc_clear_counts(dhdp);
#endif /* PROP_TXSTATUS */

	return;
}

/* Process DSLCPE private IOCTLS:
 * 
 * returns  not supported for unknown IOCTL's
*/
int 
dhd_priv_ioctl(struct net_device *net, struct ifreq *ifr, int cmd) 
{
	int	error = 0;
	int	isup = 0;
	
	switch (cmd) {
		case SIOCGLINKSTATE:
			if (net->flags&IFF_UP) isup = 1;
			if (copy_to_user((void*)(int*)ifr->ifr_data, (void*)&isup,
				sizeof(int))) {
				error = -EFAULT;
			}
			break;
			
		case SIOCSCLEARMIBCNTR:
			dhd_reset_cnt(net);
 			break;

		default:
			/* All other private IOCTLs not supported yet */
			error = -EOPNOTSUPP;
			break;
	}

	return error;
}


enum {
#if defined(PKTC)
	IOV_PKTC = 1,
	IOV_PKTCBND,
#endif /* PKTC */
	IOV_DSLCPE_END
};

const bcm_iovar_t dhd_dslcpe_iovars[] = {
#if defined(PKTC)
	{"pktc", 		IOV_PKTC,	0, IOVT_BOOL, 0	},
	{"pktcbnd",		IOV_PKTCBND,0, IOVT_UINT32, 0 },
#endif
	{NULL, 0, 0, 0, 0 }
};


int
dhd_dslcpe_iovar_op(dhd_pub_t *dhdp, const char *name,
                 void *params, int plen, void *arg, int len, bool set)
{
	const bcm_iovar_t *vi = NULL;
	int bcmerror = 0;
	int32 int_val = 0;
	int32 int_val2 = 0;
	int32 int_val3 = 0;
	bool bool_val = 0;
	int val_size;
	uint32 actionid;

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	ASSERT(name);
	ASSERT(len >= 0);

	/* Get MUST have return space */
	ASSERT(set || (arg && len));

	/* Set does NOT take qualifiers */
	ASSERT(!set || (!params && !plen));

	DHD_INFO(("%s: %s %s, len %d plen %d\n", __FUNCTION__,
	         name, (set ? "set" : "get"), len, plen));

	/* Look up var locally; if not found pass to host driver */
	if ((vi = bcm_iovar_lookup(dhd_dslcpe_iovars, name)) == NULL) {
		bcmerror = BCME_NOTFOUND;
		goto exit;
	}

	/* set up 'params' pointer in case this is a set command so that
	 * the convenience int and bool code can be common to set and get
	 */
	if (params == NULL) {
		params = arg;
		plen = len;
	}

	if (vi->type == IOVT_VOID)
		val_size = 0;
	else if (vi->type == IOVT_BUFFER)
		val_size = len;
	else
		/* all other types are integer sized */
		val_size = sizeof(int);

	actionid = set ? IOV_SVAL(vi->varid) : IOV_GVAL(vi->varid);
	
	if ((bcmerror = bcm_iovar_lencheck(vi, arg, len, IOV_ISSET(actionid))) != 0)
		goto exit;

	if (plen >= (int)sizeof(int_val))
		bcopy(params, &int_val, sizeof(int_val));

	if (plen >= (int)sizeof(int_val) * 2)
		bcopy((void*)((uintptr)params + sizeof(int_val)), &int_val2, sizeof(int_val2));

	if (plen >= (int)sizeof(int_val) * 3)
		bcopy((void*)((uintptr)params + 2 * sizeof(int_val)), &int_val3, sizeof(int_val3));

	bool_val = (int_val != 0) ? TRUE : FALSE;

	switch (actionid) {
#if defined(PKTC)
	case IOV_SVAL(IOV_PKTC):
		dhdp->pktc = int_val;
		break;
	case IOV_GVAL(IOV_PKTC):
		int_val = (int32)dhdp->pktc;
		bcopy(&int_val, arg, val_size);
		break;
	case IOV_SVAL(IOV_PKTCBND):
		dhdp->pktcbnd = int_val;
		break;
	case IOV_GVAL(IOV_PKTCBND):
		int_val = (int32)dhdp->pktcbnd;
		bcopy(&int_val, arg, val_size);
		break;
#endif /* PKTC */
	default:
		bcmerror = BCME_UNSUPPORTED;
		break;
	}

exit:
	return bcmerror;
} /* dhd_dslcpe_iovar_op */


int
dhd_dslcpe_event(dhd_pub_t *dhd_pub, int *ifidx, void *pktdata,	wl_event_msg_t *event, void **data_ptr)
{
	bcm_event_t *pvt_data;
	uint8 *event_data;
	uint32 type, status, datalen;
	uint16 flags;
	int evlen;

	/* make sure it is a BRCM event pkt and record event data */
	int ret = wl_host_event_get_data(pktdata, event, data_ptr);
	if (ret != BCME_OK) {
		return ret;
	}

	pvt_data = (bcm_event_t *)pktdata;
	event_data = *data_ptr;

	type = ntoh32_ua((void *)&event->event_type);
	flags = ntoh16_ua((void *)&event->flags);
	status = ntoh32_ua((void *)&event->status);
	datalen = ntoh32_ua((void *)&event->datalen);
	evlen = datalen + sizeof(bcm_event_t);

	switch (type) {
	case WLC_E_LINK:
	case WLC_E_DEAUTH:
	case WLC_E_DEAUTH_IND:
	case WLC_E_DISASSOC:
	case WLC_E_DISASSOC_IND:
		if (type != WLC_E_LINK) {
#if defined(BCM_BLOG)
			/* before calling system function to make action, first release prim lock,
			 * as tx direction will require this lock which on that cpu, the lock in the action handler may be holded */
			DHD_PERIM_UNLOCK_ALL((dhd_pub->fwder_unit % FWDER_MAX_UNIT));
			bcm_mcast_wlan_client_disconnect_notifier(
					(struct net_device *)dhd_idx2net(dhd_pub, dhd_ifname2idx(dhd_pub->info, event->ifname)), 
					event->addr.octet);
			DHD_PERIM_LOCK_ALL((dhd_pub->fwder_unit % FWDER_MAX_UNIT));
#endif
		}
#if defined(BCM_BLOG)
		/* also destroy all fcache flows */
		{
			struct net_device *dev = dhd_idx2net(dhd_pub, dhd_ifname2idx(dhd_pub->info, event->ifname));
			DHD_PERIM_UNLOCK_ALL((dhd_pub->fwder_unit % FWDER_MAX_UNIT));
			blog_lock();
			if (dev != NULL)
				blog_notify( DESTROY_NETDEVICE, dev, 0, 0 );
			blog_unlock();
			DHD_PERIM_LOCK_ALL((dhd_pub->fwder_unit % FWDER_MAX_UNIT));
		}
#endif
		break;	

		default:
			break;
	}
	
	return (BCME_OK);
}
