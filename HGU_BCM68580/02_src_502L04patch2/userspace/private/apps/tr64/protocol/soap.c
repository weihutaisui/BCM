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
*/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "tr64defs.h"
#include <limits.h>

extern PService FindSvcByURL(char *fname);
extern const char* rfc1123_fmt;
extern void send_error( UFILE *, int err, char* title, char* extra_header, char* text );
extern char *strip_chars(char *str, char *reject);
extern char *getargval(char *name, var_entry_t *args, int nargs);
extern void soap_response(UFILE *, const char *, const char *,   const char *,  pvar_entry_t, int);

static void QueryStateVariable(UFILE *, PService , char *, pvar_entry_t , int );

void soap_error(UFILE *, soap_error_t);
int dispatch(UFILE*, PService, char *, pvar_entry_t, int);

Error *SoapErrors[10] = {0};

/*
  Parse up the "Body>".  The element immediately after
  that will be the name of the action to invoke,
  possibly preceded by a namespace.

  The action name may be followed by whitespace and
  other attributes, like namespace, etc., or it may
  immediatly by followed by the closing angle-bracket
  '>'.

  The elements contains within the action name are the
  arguments, also possibly preceded by a namespace
  (although the SOAP spec says they should not be)

  The list of argument will be terminated by a closing
  element that corresponds to the action name, also
  possibly preceded by a namespace.

*/
void soap_action(UFILE *up, PService psvc, char *soapaction, char *body)
{
   char *ac, *u, *actstart, *colon;
   var_entry_t args[60];
   int nargs = 0;
   char *argstart, *argend;
   char *valstart, *valend;
   char *sp, *eb;
   char pattern[80];

   /* split out the action name. */
   if (soapaction != NULL && (ac = index(soapaction, '#')) != NULL) 
   {
      ac++;

      snprintf(pattern, sizeof(pattern), ":Body>");
      if ((u = strstr(body, pattern)) != NULL) 
      {
         body = u + 1;
         if ((u = strstr(body, "<")) != NULL) 
         {
            body = u + 1;
            actstart = body;
            sp = index(body, ' ');
            eb = index(body, '>');
            if (eb) 
            {
               if (sp && sp < eb) 
                  *sp = '\0';
               body = eb + 1;
               if (body[-2] != '/') {
                  snprintf(pattern, sizeof(pattern), "</%s", actstart);
                  do 
                  {
                     /* parse each argument and put it into a list of <name, value> pointers. */
                     if ((argstart = strstr(body, "<")) == NULL) 
                        break;

                     /** if we are at the end of the argument list, 
                      * break out of the loop.
                      */
                     if (strncmp(argstart, pattern, strlen(pattern)) == 0) 
                        break;
			    
                     argstart++;  // advance over the '<'

                     /** although it is contrary to the spec, 
                      *  some argument elements may have a namespace prefix.
                      *  in partcular, QueryStateVariable actions from windows XP
                      *  appear to do this but actions from Messenger do not.
                      *  In any case, be prepared to skip the namespace prefix.
                      */
                     sp = index(argstart, ' ');
                     eb = index(argstart, '>');
                     if (eb) {
                         if (sp && sp < eb) 
                             *sp = '\0';
                         *eb = '\0';
                         argend = eb;
                     } else
                         goto arg_error;
			    
                     /* DO NOT move this above the setting of argend to '/0'!!!! */
                     if ((colon = index(argstart, ':')) != NULL) 
                        argstart = colon + 1;
			    
                     if (argend[-1] == '/') 
                     {
                        valstart = valend = argend;
                     } 
                     else
                     {
                        valstart = argend + 1;
                        if ((valend = index(valstart, '<')) == NULL) 
                           break;
                        *valend = '\0';
                     }

                     /* put this argument into the arg list. */
                     args[nargs].name = argstart;
                     args[nargs].value = valstart;
                     nargs++;
			    
                     /* point to the beginning of the next argument. */
                     body = valend + 1;
                  } while (*body);
               } /* body */
            } /* eb */
         } /* argstart */
      } /* body start */

      /* invoke that action */
      UPNP_ACTION(psvc, ac, args, nargs);

      if ( strcmp(ac, "QueryStateVariable") == 0 ) 
      {
         QueryStateVariable( up, psvc, ac, args, nargs);
      } 
      else
      {
         dispatch( up, psvc, ac, args, nargs );
      }
      if ((psvc->flags & VAR_CHANGED) == VAR_CHANGED) 
      {
         update_all_subscriptions(psvc);
      }
   }
   else 
   {
      soap_error( up, SOAP_INVALIDACTION );
   }
   return;

arg_error:
   soap_error( up, SOAP_INVALIDARGS );
}

#ifdef BCMDBG
void upnp_dumpaction(PService psvc, char *ac, pvar_entry_t args, int nargs)
{
    int i;

    fprintf(stderr, "%s: action \"%s\"\n", psvc->template->name, ac);

    if (UPNP_DUMP_ACTION_ON()) {
	for (i = 0; i < nargs; i++) {
	    fprintf(stderr, "\t%s = \"%s\"\n", args[i].name, args[i].value);
	}
    }
}

void upnp_dumpresponse(const char *namespace, const char *ac, pvar_entry_t args, int nargs)
{
    int i;
    char svcname[50], *psvcname, *p;

    strcpy(svcname, namespace);
    psvcname = svcname;
    p = rindex(psvcname, ':');
    if (p != 0) {
	*p = '\0';
	p = rindex(psvcname, ':');
	if (p != 0)
	    psvcname = p+1;
    }

    fprintf(stderr, "%s: response \"%s\"\n", psvcname, ac);

    if (UPNP_DUMP_ACTION_ON()) {
	for (i = 0; i < nargs; i++) {
	    fprintf(stderr, "\t%s = \"%s\"\n", args[i].name, args[i].value);
	}
    }
}
#endif

char *getsvcval_by_name(PService psvc, const char *name)
{
   PServiceTemplate template = psvc->template;
   char *value = NULL;
   int i;

   for (i = 0; i < template->nvariables; i++) 
   {
      if (strcmp(name, template->variables[i].name) == 0) 
      {
         if (template->getvars)
            (*(template->getvars))(psvc, i);
         value = psvc->vars[i].value;
         break;
      }
   }

   return value;
}

char *getsvcval(PService psvc, int i)
{
   PServiceTemplate template = psvc->template;

   assert(i < template->nvariables);
   if (template->getvars)
      (*(template->getvars))(psvc, i);

   return  psvc->vars[i].value;
}

char *getargval(char *name, var_entry_t *args, int nargs)
{
   int i;

   for (i = 0; i < nargs; i++) 
      if (strcmp(name, args[i].name) == 0) 
         return args[i].value;

   return NULL;
}


void soap_register_errors(Error *errors)
{
   int i;

   for (i = 0; i < ARRAYSIZE(SoapErrors); i++) 
   {
      if (SoapErrors[i] == 0) 
      {
         SoapErrors[i] = errors;
         break;
      }
   }
}

static const char *soap_errorstr(soap_error_t error_code) 
{
   static Error last_ditch = (Error) { 900, "Unknown SOAP error!" };
   PError pe = NULL;
   int i;

   for (i = 0; i < ARRAYSIZE(SoapErrors); i++) 
   {
      if (!SoapErrors[i]) 
         break;
	
      for (pe = SoapErrors[i]; pe->error_string; pe++) 
      {
         if (pe->error_code == error_code)
            break;
      }

      if (pe->error_string)
         break;
   }
    
   if (pe && !pe->error_string) 
   {
      pe = &last_ditch;
   }

   return pe->error_string;
}


void soap_error( UFILE *up, soap_error_t error_code)
{
   const char *error_string = soap_errorstr(error_code);
   char buffer[1024];
   UFILE *ubody;

#ifdef BCMDBG
   if (UPNP_RESPONSE_ON()) 
      printf("SOAP error: %d \"%s\"\n", error_code, error_string);
#endif

   if ((ubody = usopen(buffer, sizeof(buffer)))) {
      uprintf( ubody,
               "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n"
               " <s:Body>\r\n"
               "  <s:Fault>\r\n"
               "   <faultcode>s:Client</faultcode>\r\n"
               "   <faultstring>UPnPError</faultstring>\r\n"
               "   <detail>\r\n"
               "    <UPnPError xmlns=\"urn:%s:control-1-0\">\r\n"
               "     <errorCode>%ld</errorCode>\r\n"
               "     <errorDescription>%s</errorDescription>\r\n"
               "    </UPnPError>\r\n"
               "   </detail>\r\n"
               "  </s:Fault>\r\n"
               " </s:Body>\r\n"
               "</s:Envelope>\r\n",
               TR64_DSLFORUM_SCHEMA, error_code, error_string);
	
      http_response(up, HTTP_SERVER_ERROR, ubuffer(ubody), utell(ubody));
      
      uclose(ubody);
   }
}

void soap_response(UFILE *uclient, const char *actname,  const char *schema,
                   const char *namespace, pvar_entry_t args, int nargs)
{
   UFILE *ubody;
   int i;
   
   UPNP_RESPONSE(namespace, actname, args, nargs);
   
   if ((ubody = usopen(NULL, 0))) 
   {
      uprintf(ubody, 
              "<?xml version=\"1.0\"?>\r\n"
              "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
              " s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
              "<s:Body><m:%sResponse xmlns:m=\"urn:%s:service:%s\">", actname, schema, namespace);
      for (i = 0; i < nargs; i++) 
      {
         if(args[i].value==NULL)
         {
            printf("soap_response args[%d].value=null name=%s!!!\n", i, args[i].name);
         }
         assert(args[i].value);
         uprintf(ubody, "<%s>%s</%s>", 
                 args[i].name, (args[i].value?args[i].value:""), args[i].name);
      }
      uprintf(ubody, "</m:%sResponse></s:Body></s:Envelope>\r\n", actname);
	
      http_response(uclient, HTTP_OK, ubuffer(ubody), utell(ubody));

      uclose(ubody);
   }
}



/* 
   QueryStateVariable is an odd bird.  It looks like a normal UPNP
   action request, but it not explicitly part of any service - yet it
   is applicable to every service.  For lack of a better place to put
   it, it lives here.  

   Note that the namespace used in responses is not the same as the
   one used in responses to standard sevice actions.  
*/
static void QueryStateVariable( UFILE *up, PService psvc, char *ac, pvar_entry_t args, int nargs)
{
   char *VariableName = getargval("varName", args, nargs);
   char *value;

   if (!VariableName) 
   {
      soap_error(up, SOAP_INVALIDARGS );
   }
   else
   {
      if ((value = getsvcval_by_name(psvc, VariableName)) != NULL) 
      {
         /* found the variable. */
         args[0] = (var_entry_t) { "return", value };
	    
         soap_response(up, ac, TR64_DSLFORUM_SCHEMA, "control-1-0", args, 1);
      } 
      else
      {
         soap_error(up, SOAP_INVALIDVAR);
      }
   }
}


static bool IsUnsigned(const VarTemplate *pVT)
{
   return (bool) (pVT->flags & (VAR_USHORT|VAR_ULONG|VAR_UBYTE));
}

static bool ValidateInteger(const char *valuestr, const VarTemplate *pVT) 
{
   char *str, *strend;
   bool valid = FALSE;
   unsigned int uvalue, umin, umax, ustep;
   int value, min, max, step;
   allowedValueRange *range = pVT->allowed.range;
   
   if (IsUnsigned(pVT)) 
   {
      /* we are checking the validity of an unsigned integer. */
      str = (char *) valuestr;
      uvalue = strtoul(str, &strend, 10);
      if (strend == str || *strend != '\0')
      {
         valid = FALSE;
         if (!valid) printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_UBYTE)
      {
         valid = (uvalue >= 0 && uvalue <= UCHAR_MAX);
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_USHORT)
      {
         valid = (uvalue >= 0 && uvalue <= USHRT_MAX);
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_ULONG)
      {
         valid = (uvalue >= 0 && uvalue <= ULONG_MAX);
         if (!valid) 
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      }
      else
      {
         valid = FALSE;
         if (!valid) printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      }
	
      /* now check against the specified range. */
      if (valid && (pVT->flags & VAR_RANGE) && range)
      {
         str = range->minimum;
         umin = strtoul(str, &strend, 10);
         
         str = range->maximum;
         umax = strtoul(str, &strend, 10);
	    
         str = range->step;
         ustep = strtoul(str, &strend, 10);
	    
         valid = (uvalue >= umin && uvalue <= umax);
         if (!valid) 
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);

         if (valid) 
         {
            uvalue -= umin;
		
            valid = ((uvalue % ustep) == 0);
            if (!valid) 
               printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
         }
      }
   } 
   else
   {
      /* we are checking the validity of a signed integer. */
      str = (char *) valuestr;
      value = strtol(str, &strend, 10);
      if (strend == str || *strend != '\0') 
      {
         valid = FALSE;
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_BYTE) 
      {
         valid = (value >= CHAR_MIN && value <= CHAR_MAX);
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_SHORT)
      {
         valid = (value >= SHRT_MIN && value <= SHRT_MAX);
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_ULONG)
      {
         valid = (value >= 0 && value <= ULONG_MAX);
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else if (pVT->flags & VAR_LONG)
      {
         valid = (value >= LONG_MIN && value <= LONG_MAX);
         if (!valid)
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
      } 
      else
      {
         valid = FALSE;
         if (!valid)
            printf("%s failed at line %d, name %s value %s pVT->flags %x\n",
                   __FUNCTION__,__LINE__,pVT->name,pVT->value,(unsigned int)(pVT->flags));
      }

      /* now check against the specified range. */
      if (valid && (pVT->flags & VAR_RANGE) && range)
      {
         str = range->minimum;
         min = strtol(str, &strend, 10);

         str = range->maximum;
         max = strtol(str, &strend, 10);

         str = range->step;
         step = strtol(str, &strend, 10);

         valid = (value >= min && value <= max);
         if (!valid) 
            printf("%s failed at line %d\n",__FUNCTION__,__LINE__);

         if (valid)
         {
            value -= min;
            valid = ((value % step) == 0);
            if (!valid) 
               printf("%s failed at line %d\n",__FUNCTION__,__LINE__);
         }
      }
   }

   return valid;
}

static bool MemberOfList(char *value, char **list)
{
   bool valid = FALSE;

   while (*list)
   {
      if (strcmp(value, *list) == 0)
      {
         valid = TRUE;
         break;
      }
      list++;
   }

   return valid;
}

static bool ValidParam(PService psvc, PParam pa)
{
   static char *upnpBoolList[] = { "0", "1", NULL };
   int related_var = pa->related;
   VarTemplate *pVT = &psvc->template->variables[related_var]; 
   bool valid = FALSE;
   
   if (pVT->flags & VAR_STRING) 
   {
      if (valid && (pVT->flags & VAR_LIST))
      {
         char **list = pVT->allowed.list;
		    
         valid = MemberOfList(pa->value, list);
      } 
      else
      {
         valid = TRUE;
      }
   } 
   else if (pVT->flags & VAR_BOOL)
   {
      valid = MemberOfList(pa->value, upnpBoolList);
   } 
   else 
   {
      valid = ValidateInteger(pa->value, pVT);
   }
   
   return valid;
}


static void invoke( UFILE *up, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   PParam pa;
   int badargs = FALSE;
   
   UPNP_TRACE(("invoking %s:%s\n", psvc->template->name, ac->name));
   for (pa = ac->params; pa->name; pa++) 
   {
      if (pa->flags & VAR_IN) 
      {
         pa->value = getargval(pa->name, args, nargs);
         if (pa->value == NULL)
         {
            UPNP_ERROR(("Expected parameter \"%s\" for action \"%s\"\n", pa->name, ac->name));
            badargs = TRUE;
         } 
         else if (!ValidParam(psvc, pa))
         {
            UPNP_ERROR(("Invalid parameter \"%s\" for Action \"%s:%s\" - \"%s\"\n", pa->name, psvc->template->name, ac->name, pa->value));
            badargs = TRUE;
         }
      }
   }

   if (badargs)
   {
      /* if we get here, we are missing some args that should have been there. */
      soap_error( up, SOAP_INVALIDARGS );
   } 
   else
   {
      /* if the func returns FALSE, it has taken care of the reply or the error response. */
      if ((ac->func)(up, psvc, ac, args, nargs)) 
      {
         soap_success(up, psvc, ac, args, nargs);
      }

      /* cleanup parameter pointers. */
      for (pa = ac->params; pa->name; pa++) 
      {
         if(pa->value)
         {
            if(pa->flags & VAR_OUT)
            {
               free(pa->value);
            }
            pa->value = NULL;
         }
      }
   }
}

/* 
   Return from a successful UPnP action.  The resturn arguments are created from the action signature. 
   This routine is for use by actions that must return the results before affecting any changes.
*/
void soap_success ( UFILE *up, PService psvc, PAction ac, pvar_entry_t args, int nargs )
{
   PParam pa;

   nargs = 0;
   for (pa = ac->params; pa->name; pa++)
   {
      if (pa->flags & VAR_OUT) 
      {
         args[nargs].name = pa->name;
         args[nargs].value = pa->value;
         nargs++;
      }
   }
   soap_response(up, ac->name, psvc->template->schema, psvc->template->name, args, nargs);
}

int dispatch( UFILE *up, PService psvc, char *ac, pvar_entry_t args, int nargs)
{
   PAction *pac;
   int dispatched = FALSE;

   if (psvc->template->actions)
   {
      for (pac = psvc->template->actions; *pac; pac++)
      {
         if (strcasecmp((*pac)->name, ac) == 0)
         {
            /* found the action - now invoke it. */
            invoke(up, psvc, *pac, args, nargs);
            dispatched = TRUE;
            break;
         }
      }
      if (!*pac) 
      {
         UPNP_ERROR(("%s: invalid action \"%s\"\n", psvc->template->name, ac));
         soap_error( up, SOAP_INVALIDACTION );
      }
   }

   return dispatched;
}
