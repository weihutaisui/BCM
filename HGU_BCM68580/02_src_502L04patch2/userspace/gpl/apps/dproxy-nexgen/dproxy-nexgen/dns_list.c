#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dns_list.h"
#include "dns_io.h"
#include "dns_mapping.h"

dns_request_t *dns_request_list;
/* Last request in dns_request_list */
dns_request_t *dns_request_last;
static int dns_list_count=0;

/*****************************************************************************/
int dns_list_add(dns_request_t *r)
{
  dns_request_t *new;

  if (NULL == r) {
    debug("null request passed in!");
    return 0;
  }

  if (!(new = malloc(sizeof(*new)))) {
    debug("malloc() failed");
    return 0;
  }

  memcpy(new, r, sizeof(*r));
  new->expiry = time(NULL) + DNS_TIMEOUT;
  new->retx_count = 0;
  new->prev = NULL;
  new->next = dns_request_list;

  if (dns_request_list)
    dns_request_list->prev = new;
  if (dns_request_last == NULL)
    dns_request_last = new;
  dns_request_list = new;

  dns_list_count++;
  return 1;
}

/*****************************************************************************/
dns_request_t *dns_list_find_by_id(dns_request_t *req)
{ 
  dns_request_t *r;

  if (NULL == req) {
     debug("null request passed in!");
     return NULL;
  }

  for(r = dns_request_list; r; r = r->next){
     if(req->message.header.id == r->message.header.id)
         return r;
  }
  return NULL;
}




/*****************************************************************************/
int dns_list_remove(dns_request_t *r)
{
  if (NULL == r) {
    debug("null request passed in!");
    return 0;
  }

  if (r == dns_request_list)
    dns_request_list = r->next;
  else
    r->prev->next = r->next;

  if (r == dns_request_last)
    dns_request_last = r->prev;
  else
    r->next->prev = r->prev;
 
  free(r);

  dns_list_count--;
  return 1;
}


/*****************************************************************************/
int dns_list_remove_related_requests_and_swap(dns_request_t *r)
{
  UBOOL8 dnsSwapNeeded = FALSE;
  char dnsUsed[INET6_ADDRSTRLEN]={0};
  dns_request_t *headPtr, *next;
  
  if (NULL == r) {
    debug("null request passed in!");
    return 0;
  }

  /* For the first time call dns_mapping_is_dns_swap_needed, dnsUsed is null
  * and if dnsSwapNeeded TRUE, the subsequent calls will use dnsUsde to match
  * for the requests with the same dns for removing them in the request list.
  */
  dnsSwapNeeded = dns_mapping_is_dns_swap_needed(r, dnsUsed);
  if (dnsSwapNeeded)
  {
    dns_list_remove(r);  
    headPtr = dns_request_list;
    while (headPtr) {
       next = headPtr->next;
       cmsLog_notice("next request, dnsUsed %s", dnsUsed);
       if (dns_mapping_is_dns_swap_needed(headPtr, dnsUsed)) {
          dns_list_remove(headPtr);  
       }        
       headPtr = next;
    }
  }

  if (dnsSwapNeeded) {
    dns_mapping_do_dns_swap(dnsUsed);
  }
  
  return 1;
}

/*****************************************************************************/
void dns_list_print(void)
{
  dns_request_t *r;

  printf("\n===========Dumping request list (time=%d):\n", (int) time(NULL));
  printf("dns_request_list=%p dns_request_last=%p dns_list_count=%d\n",
         dns_request_list, dns_request_last, dns_list_count);

  for (r = dns_request_list; r; r = r->next) {
     printf("    (%p) ID: 0x%x type=%u Name=%s IP=%s "
            "expiry=%d switch_on_timeout=%d retx_count=%d next=%p prev=%p\n",
            r, r->message.header.id,
            (unsigned int) r->message.question[0].type,
            r->cname, r->ip, (int) r->expiry,
            r->switch_on_timeout, r->retx_count, r->next, r->prev);
  }

  printf("=============================================\n\n");
}

/*****************************************************************************/
int dns_list_next_time(void)
{
  int timeout = 0;

  if (dns_request_last) {
    timeout = dns_request_last->expiry - time(NULL);
    if (timeout < 1)
      timeout = 1;
  }
  return timeout;
}


