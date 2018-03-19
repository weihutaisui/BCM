/*
 *
 * <:copyright-BRCM:2012:proprietary:standard
 * 
 *    Copyright (c) 2012 Broadcom 
 *    All Rights Reserved
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
*/

#include <stdarg.h>
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include "tr64defs.h"

extern char *getsvcval(PService psvc, int i);
extern void soap_response(UFILE*, const char *, const char *, const char *, pvar_entry_t, int);

PService init_service(PServiceTemplate svctmpl, PDevice pdev, unsigned int dynsvcidx)
{
   PService psvc;
   PFSVCINIT func;
   int i, table_size;

   UPNP_TRACE(("Initializing service \"%s\".\r\n", svctmpl->name));

   if (svctmpl->schema == NULL)
      svctmpl->schema = TR64_DSLFORUM_SCHEMA;

   psvc = (PService) malloc(sizeof(Service));
   memset(psvc, 0, sizeof(Service));

   /* store pointers to the template and the device into the service. */
   psvc->template = svctmpl;
   psvc->device = pdev;
   if(dynsvcidx ==0)
   {
      psvc->instance = ++svctmpl->count;
   }
   else
   {
      psvc->instance = dynsvcidx;
   }

   /* allocate and initialize the service's state variables. */
   table_size = svctmpl->nvariables * sizeof(StateVar);
   psvc->vars = (StateVar *) malloc(table_size);
   memset(psvc->vars, 0, table_size);

   for (i = 0; i < svctmpl->nvariables; i++) 
   {
      strcpy(psvc->vars[i].value, svctmpl->variables[i].value);
      psvc->vars[i].flags = svctmpl->variables[i].flags;
      assert((psvc->vars[i].flags & VAR_CHANGED) == 0);
   }

    // call the service's intialization function, if defined.
    if ((func = svctmpl->svcinit) != NULL) 
    {
       (*func)(psvc, SERVICE_CREATE);
    }

    return psvc;
}

void destroy_service(PService psvc, int freeit)
{
    PFSVCINIT func;

    UPNP_TRACE(("Destroying service \"%s\".\r\n", psvc->template->name));

    if(psvc->template->name && freeit)
    {
       free(psvc->template->name);
    }

    // call the service's intialization function, if defined.
    if ((func = psvc->template->svcinit) != NULL) 
    {
	(*func)(psvc, SERVICE_DESTROY);
    }

    if(psvc->template && freeit)
    {
       free(psvc->template);
    }    

    free(psvc->vars);
    free(psvc);
}


void mark_changed(PService psvc, int varindex) 
{
    assert(varindex >= 0 && varindex <= psvc->template->nvariables);

    psvc->vars[varindex].flags |= VAR_CHANGED;
    psvc->flags |= VAR_CHANGED;
}


/* Print an XML device description for a device and all its subdevices.
   We used to just print the static XML device description from a file, but now that the 
   IGD is more dynamic and can adjust to different gateway configurations,
   we must dynamically generate the XML.
 */
void service_xml(PService psvc, UFILE *up)
{
    const char *type2str(vartype_t type);
    PFSVCXML func;
    PVarTemplate pv;
    PAction *ac;
    PParam pa;
    const char *tstr;

    UPNP_TRACE(("service_xml: psvc->template->name %s\n",psvc->template->name));

    uprintf(up, "<?xml version=\"1.0\"?>\r\n");
    uprintf(up, "<scpd xmlns=\"urn:%s:service-1-0\">\r\n",TR64_DSLFORUM_SCHEMA);
    uprintf(up, "  <specVersion>\r\n");
    uprintf(up, "    <major>1</major>\r\n");
    uprintf(up, "    <minor>0</minor>\r\n");
    uprintf(up, "  </specVersion>\r\n");

    /* call the service's xml function, if defined. */
    if ((func = psvc->template->svcxml) != NULL)
    {
       (*func)(psvc, up);
    }

    uprintf(up, "<actionList>\r\n");
    if (psvc->template->actions) {
       for (ac = psvc->template->actions; *ac; ac++)
       {
          uprintf(up, "<action>\r\n");
          uprintf(up, "<name>%s</name>\r\n", (*ac)->name);
          
          /* don't print any <argumentList> if there are no args. */
          if ((pa = (*ac)->params) && pa->name) 
          {
             uprintf(up, "<argumentList>\r\n");
             while (pa->name) 
             {
                uprintf(up, "<argument>\r\n");
                uprintf(up, "<name>%s</name>\r\n", pa->name);
                uprintf(up, "<relatedStateVariable>%s</relatedStateVariable>\r\n",
                        psvc->template->variables[pa->related].name);
                uprintf(up, "<direction>%s</direction>\r\n",
                        (pa->flags == VAR_OUT ? "out" : "in"));
                uprintf(up, "</argument>\r\n");
                
                pa++;
             }
             uprintf(up, "</argumentList>\r\n");
          }
          uprintf(up, "</action>\r\n");
       }
    }
    uprintf(up, "</actionList>\r\n");
    
    uprintf(up, "<serviceStateTable>\r\n");
    for (pv = psvc->template->variables; pv->name; pv++) {
       uprintf(up, "<stateVariable sendEvents=\"%s\">\r\n", 
               (pv->flags & VAR_EVENTED ? "yes" : "no"));
       uprintf(up, "  <name>%s</name>\r\n", pv->name);
       tstr = type2str(pv->flags&VAR_TYPE_MASK);
       if (tstr == NULL) 
       {
          UPNP_ERROR(("unknown type - %s \"%s\"\r\n", 
                      psvc->template->name, pv->name));
       } 
       else {
          uprintf(up, "  <dataType>%s</dataType>\r\n", tstr);
       }

       if ((pv->flags & VAR_LIST) && pv->allowed.list ) {
          char ** avl;
          uprintf(up, "    <allowedValueList>\r\n");
          for (avl = pv->allowed.list; *avl; avl++) {
             uprintf(up, "      <allowedValue>%s</allowedValue>\r\n", *avl);
          }
          uprintf(up, "    </allowedValueList>\r\n");
       } else if ((pv->flags & VAR_RANGE) && pv->allowed.range ) {
          PallowedValueRange avr =  pv->allowed.range;
          uprintf(up, "    <allowedValueRange>\r\n");
          uprintf(up, "    <minimum>%s</minimum>\r\n", avr->minimum);
          uprintf(up, "    <maximum>%s</maximum>\r\n", avr->minimum);
          uprintf(up, "    <step>%s</step>\r\n", avr->step);
          uprintf(up, "    </allowedValueRange>\r\n");
       }

       uprintf(up, "</stateVariable>\r\n");
    }
    uprintf(up, "</serviceStateTable>\r\n");
    
    uprintf(up, "</scpd>\r\n");
}


const char *type2str(vartype_t type)
{
   char *str;

   switch (type) 
   {
   case VAR_UBYTE:
      str = "ui1";
      break;
   case VAR_USHORT:
      str = "ui2";
      break;
   case VAR_ULONG:
      str = "ui4";
      break;
   case VAR_BYTE:
      str = "i1";
      break;
   case VAR_SHORT:
      str = "i2";
      break;
   case VAR_LONG:
      str = "i4";
      break;
   case VAR_STRING:
      str = "string";
      break;
   case VAR_BOOL:
      str = "boolean";
      break;
   case VAR_UUID:
      str = "uuid";
      break;
   default:
      str = NULL;
   }
   return str;
}



// NOT IMPLEMENTED
int NotImplemented(UFILE *up, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
    UPNP_ERROR(("Action \"%s\" not implemented, svc \"%s\"\r\n", ac->name, psvc->template->name));

    soap_error( up, SOAP_INVALIDACTION );

    /* indicate that we have already handled the response.
       (see the handling of return code from 'func' in invoke(). */
    return FALSE;
}



/* The default action is to reflect the contents of the related variables to all OUT parameters.
   That is, if an action has an OUT param, return the value of the <relatedVariable> for that param.
   Do the same for any other out params.

   The value of related variables is obtained by calling getsvcval().
*/
int DefaultAction(UFILE *up, PService psvc, PAction ac, 
		  pvar_entry_t args, int nargs)
{
    PParam pa;

    for (pa = ac->params; pa->name; pa++) 
    {
       if (pa->flags & VAR_OUT) 
       {
          pa->value = strdup(getsvcval(psvc, pa->related));
	}
    }

    return TRUE;
}

