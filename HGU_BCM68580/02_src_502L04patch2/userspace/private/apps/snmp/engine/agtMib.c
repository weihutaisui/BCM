/* ====================================================================
 * <:label-BRCM:1997:NONE:standard
 * 
 * :>
 * Copyright (c) 1997-2000
 *                      SMASH, Harrie Hazewinkel.  All rights reserved.
 *
 * This product is developed by Harrie Hazewinkel and updates the
 * original SMUT compiler made as his graduation project at the
 * University of Twente.
 *
 * SMASH is a software package containing an SNMP MIB compiler and
 * an SNMP agent system. The package can be used for development
 * of monolithic SNMP agents and contains a compiler which compiles
 * MIB definitions into C-code to developed an SNMP agent.
 * More information about him and this software product can
 * be found on http://www.simpleweb.org/packages/software/smash/.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by Harrie Hazewinkel"
 *
 * 4. The name of the Copyright holder must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Harrie Hazewinkel"
 *    Also acknowledged are:
 *    - The Simple group of the University of Twente,
 *          http://www.simpleweb.org/
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR, ITS DISTRIBUTORS
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "asn1.h"
#include "snmp.h"
#include "agtEngine.h"
#include "agtMib.h"
#include "cms_util.h"
#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif

oid null_oid[] = {0, 0};
long    long_return;
unsigned char   return_buf[MAX_OCTSTR_LEN];

SubTree *mib_tree = NULL;

int
internal_compare(int len1, oid *name1, int len2, oid *name2)
{
    register int    len;

    /* len = minimum of len1 and len2 */
    if (len1 < len2)
        len = len1;
    else
        len = len2;

    /* find first non-matching byte */
    while(len-- > 0)
    {
        if (*name1 < *name2)
            return -1;
        if (*name2++ < *name1++)
            return 1;
    }

    /* bytes match up to length of shorter string */
    if (len1 < len2)
        return -1;  /* name1 shorter, so it is "less" */
    if (len2 < len1)
        return 1;
    return 0;   /* both strings are equal */
}

int
compare(Oid *var1, Oid *var2)
{
    return(internal_compare(var1->namelen, var1->name, var2->namelen, var2->name));
}

int
internal_compare_tree(int len1, oid *name1, int len2, oid *name2)
{
    register int    len;

    /* len = minimum of len1 and len2 */
    if (len1 < len2)
    {
        len = len1;
    }
    else
    {
        len = len2;
    }

    /* find first non-matching byte */
    while(len-- > 0)
    {
        if (*name1 < *name2)
        {
            return -1;
        }
        if (*name2++ < *name1++)
        {
            return 1;
        }
    }

    /* bytes match up to length of shorter string */
    if (len1 < len2)
    {
        return -1;  /* name1 shorter, so it is "less" */
    }

    /* name1 matches name2 for length of name2, or they are equal */
    return 0;
}

int
compare_tree(Oid *var1, Oid *var2)
{
    return(internal_compare_tree(var1->namelen, var1->name, var2->namelen,
        var2->name));
}


int insert_group_in_mib(SubTree *tree)
{
    SubTree *current = mib_tree;
    SubTree *prev = NULL;
    int result;

    cmsLog_debug("tree(%p), mib_tree(%p)", tree, mib_tree);

    if (current == NULL)
    {
        mib_tree = tree;
        tree->next = NULL;
        return(0);
    }
    while (current)
    {
        result = internal_compare_tree(current->prefix_length, current->prefix,
            tree->prefix_length, tree->prefix);
        if (result < 0)
        {
            prev = current;
            current = current->next;
        }
        else
            if (result == 0)
            {
                cmsLog_error("Cannot insert existing subtree (skip)");
                return(1);
            }
            else
            {
                if (prev)
                {
                    tree->next = current;
                    prev->next = tree;
                }
                else
                {
                    mib_tree = tree;
                    tree->next = current;
                }
                return(0);
            }
    }
    prev->next = tree;
    return(0);
}

void printOid(char *pPrefix, Oid *pOid)
{
   int     i = 0;
   char    buf[100] = "\0";
   char   *pBuf = buf;

   if (pPrefix == NULL || pOid == NULL)
   {
      return;
   }

   pBuf += snprintf(pBuf, sizeof(buf) - strlen(pBuf), "%s", pPrefix);
   for( i = 0; i < pOid->namelen; i++ )
   {
      pBuf += snprintf(pBuf, sizeof(buf) - strlen(pBuf), "%d.", pOid->name[i]);
   }
   cmsLog_debug("%s", buf);
}

unsigned char  *getStatPtr(Oid *req_oid, unsigned char *type, int *len,
    unsigned short *acl, int searchType, WRITE_METHOD *write_method,
    snmp_info_t *mesg, int *noSuchObject)
{
    register SubTree   *curr_subtree;
    register Object    *curr_var;
    Oid                object;
    register unsigned char    *access = NULL;
    int                result, treeresult;
    int                prefix_length;

    cmsLog_debug("req_oid(%p), searchType(%d), mib_tree(%p)",
                 req_oid, searchType, mib_tree);
    printOid("req_oid oid: ", req_oid);

    *write_method = NULL;
    for(curr_subtree = mib_tree; curr_subtree; curr_subtree = curr_subtree->next)
    {
        treeresult = internal_compare_tree(req_oid->namelen, req_oid->name,
            curr_subtree->prefix_length, curr_subtree->prefix);

        cmsLog_debug("treeresult(%d)", treeresult);

        /* if (searchType == EXACT and treeresult == 0) OR
         *    (searchType == NEXT  and treeresult <= 0) 
         */
        if (treeresult == 0 || ((searchType == NEXT) && treeresult < 0))
        {
            prefix_length = curr_subtree->prefix_length;
            memcpy((char *)object.name, curr_subtree->prefix,
                (prefix_length*sizeof(oid)));
            object.namelen = prefix_length;
            for(curr_var = curr_subtree->variables; curr_var->syntax; curr_var++)
            {
                memcpy((char *)(object.name + prefix_length),
                    curr_var->oidSuffix.name,
                    curr_var->oidSuffix.namelen * sizeof(oid));
                object.namelen = prefix_length + curr_var->oidSuffix.namelen;
                if (curr_var->acl_sort & SCALAR)
                {
                    /* SCALAR type */

                    result = compare(req_oid, &object);

                    /* if (searchType == EXACT AND result == 0 ) OR
                     *    (searchType == NEXT  AND result <  0 ) 
                     */
                    if (((searchType == EXACT) && (result == 0)) ||
                        ((searchType == NEXT)  && (result <  0)))
                    {
#ifdef BUILD_SNMP_DEBUG
                        if ((snmpDebug.level & SNMP_DEBUG_LEVEL_SNMP) == SNMP_DEBUG_LEVEL_SNMP)
                        {
                           printOid("scalar oid: ", req_oid);
                        }
#endif
                        access = (*(curr_var->findVar))(len, mesg, write_method);
                    }
                }
                else
                {
                    /* COLUMN type */

                    result = compare_tree(req_oid, &object);

                    /* if (searchType == EXACT and result == 0) OR
                     *    (searchType == NEXT  and result <= 0) 
                     */
                    if (result == 0 || ((searchType == NEXT) && result < 0))
                    {
#ifdef BUILD_SNMP_DEBUG
                        if ((snmpDebug.level & SNMP_DEBUG_LEVEL_SNMP) == SNMP_DEBUG_LEVEL_SNMP)
                        {
                           printOid("sequence oid: ", req_oid);
                        }
#endif
                        access = (*(curr_var->findVar))(len, &object, req_oid,
                                                        searchType, mesg, write_method);
                    }
                }
                if (access)
                {
                    memcpy(req_oid->name, object.name,
                         (int)(object.namelen*sizeof(oid)));
                    req_oid->namelen = object.namelen;
                    *type = curr_var->syntax;
                    *acl = curr_var->acl_sort;
                    *noSuchObject = FALSE;

                    cmsLog_debug("Exiting loop");
                    printOid("req_oid oid: ", req_oid);

                    return access;
                }
                else
                {
                    *noSuchObject = TRUE;
                }
            }
        }
    }

    return access;
}

