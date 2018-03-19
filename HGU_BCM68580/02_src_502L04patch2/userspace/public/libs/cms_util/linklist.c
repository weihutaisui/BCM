/***********************************************************************
 *
<:copyright-BRCM:2010:DUAL/GPL:standard

   Copyright (c) 2010 Broadcom 
   All Rights Reserved

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
 *
 ************************************************************************/

#include "cms.h"
#include "cms_util.h"
#include "cms_linklist.h"

/* add entry to the end of the list */
CmsRet addEnd(pENTRY_TYPE pEntry, pLIST_TYPE pList)
{
   if (pEntry == NULL)
   {
      cmsLog_debug("Entry is NULL, do nothing. \n");
      return CMSRET_INVALID_ARGUMENTS;
   }
   pEntry->next = NULL;
   if (pList->head == NULL)
   {
      pList->head = pEntry;
      pList->tail = pEntry;
   }
   else
   {
      /* always add to the end of the list */
      pList->tail->next = pEntry;
      pList->tail = pEntry;
   }
   return CMSRET_SUCCESS;
}

/* this is called with list, allocated entry */
CmsRet addFront(pENTRY_TYPE pEntry, pLIST_TYPE pList)
{
   if (pEntry == NULL)
   {
      cmsLog_debug("Entry is NULL, do nothing. \n");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (pList->head == NULL)
   {
      pEntry->next = NULL;
      pList->head = pEntry;
      pList->tail = pEntry;
   }
   else
   {
      /* always add to the front of the list */
      pEntry->next = pList->head;
      pList->head = pEntry;
   }
   return (CMSRET_SUCCESS);
}

/* always remove first entry of the list, and return it caller */
void *removeFront(pLIST_TYPE pList)
{
   pENTRY_TYPE ptr=NULL;

   if (pList->head == NULL)
   {
      return((void*)ptr);
   }
   
   ptr = pList->head;
   pList->head = pList->head->next;
   
   return ((void*)ptr);
}

/* always remove last entry of the list, and return it caller */
void *removeEnd(pLIST_TYPE pList)
{
   pENTRY_TYPE ptr = NULL;
   pENTRY_TYPE prevPtr = NULL;

   if (pList->head == NULL)
   {
      return ((void*)ptr);
   }

   if (pList->head == pList->tail)
   {
      /* this is the only entry left */
      ptr = pList->head;
      pList->head = pList->tail = NULL;
   }
   else
   {
      ptr = pList->head;
      prevPtr = ptr;
      while (ptr != NULL)
      {
         if (prevPtr->next == pList->tail)
         {
            break;
         }
         prevPtr = ptr;
         ptr = ptr->next;
      }
      ptr = pList->tail;
      pList->tail = prevPtr;
   }
   return (ptr);
}

/* remove a particular entry with input key */
void *removeEntry(pLIST_TYPE pList, const void *key, LIST_KEY_TYPE type)
{
   void *ptr=NULL;

   if (pList == NULL)
   {
      return ptr;
   }

   if (type == KEY_INT)
   {
      ptr = removeIntEntry(pList, (*(int*)key));
   }
   else
   {
      ptr = removeStrEntry(pList, (const char*)key);
   }
   return (ptr);
}

void *removeFoundEntry(pENTRY_TYPE ptr, pENTRY_TYPE prevPtr, pLIST_TYPE pList)
{
   if (ptr == NULL)
   {
      /* entry is the last one */
      pList->tail = prevPtr;
   }
   else if (ptr == prevPtr)
   {
      /* entry is the first one in the list */
      pList->head = ptr->next;
   }
   else
   {
      /* entry is in the middle */
      prevPtr->next = ptr->next;
   }
   return (ptr);
}

int findIntEntry(pLIST_TYPE pList, int key, pENTRY_TYPE *prevPtr, pENTRY_TYPE *ptr)
{
   int found=0;
   pENTRY_TYPE p, prev;
   
   p = pList->head;
   prev = pList->head;
   
   while (p != NULL)
   {
      if (*((int*)p->key) == key)
      {
         found = 1;
         break;
      }
      if (p->next != NULL)
      {
         prev = p;
      }
      p = p->next;
   }
   if (!found)
   {
      *ptr = NULL;
      *prevPtr = NULL;
   }
   else
   {
      *ptr = p;
      *prevPtr = prev;
   }

   return (found);
}

int findStrEntry(pLIST_TYPE pList, const char *key, pENTRY_TYPE *prevPtr, pENTRY_TYPE *ptr)
{
   int found=0;
   pENTRY_TYPE prev, p;

   p = pList->head;
   prev = pList->head;

   while (p != NULL)
   {
      if ((strcmp((const char*)(p->key),key)) == 0)
      {
         found = 1;
         break;
      }
      if (p->next != NULL)
      {
         prev = p;
      }
      p = p->next;
   }      
   if (!found)
   {
      *ptr = NULL;
      *prevPtr = NULL;
   }
   else
   {
      *ptr = p;
      *prevPtr = prev;
   }
   return (found);
}


void *removeIntEntry(pLIST_TYPE pList, int key)
{
   pENTRY_TYPE ptr;
   pENTRY_TYPE prevPtr;
   int found = 0;

   found = findIntEntry(pList,key,&prevPtr,&ptr);

   if (found)
   {
      ptr = removeFoundEntry(ptr,prevPtr, pList);
   }
   else
   {
      ptr = NULL;
   }
   return (ptr);
}

void *removeStrEntry(pLIST_TYPE pList, const char *key)
{
   pENTRY_TYPE ptr, prevPtr;
   int found = 0;

   found = findStrEntry(pList,key,&ptr,&prevPtr);
   if (found)
   {
      ptr = removeFoundEntry(ptr,prevPtr, pList);
   }
   else
   {
      ptr = NULL;
   }
   return (ptr);
}

/* sort the list in ascending order */
void sortIntList(pLIST_TYPE pList)
{
   pENTRY_TYPE smallestPtr;
   pENTRY_TYPE ptr;
   void *saveNumber;

   smallestPtr = pList->head;
   ptr = pList->head->next;

   if ((smallestPtr == NULL) || (ptr == NULL))
   {
      /* there is no entry or only 1 entry */
      cmsLog_debug("smallestPtr is NULL or ptr is NULL\n");
      return;
   }
   while ( smallestPtr != NULL)
   {
      while (ptr != NULL)
      {
         /* comparing to smallest element */
         if (*((int*)ptr->key) < *((int*)smallestPtr->key))
         {
            saveNumber = smallestPtr->key;
            smallestPtr->key = ptr->key;
            ptr->key = saveNumber;
         }
         ptr = ptr->next;
      }
      smallestPtr = smallestPtr->next;
      if (smallestPtr != NULL)
      {
         ptr = smallestPtr->next;
      }
   }  /* loop through all */
}

void printList(const pLIST_TYPE pList, LIST_KEY_TYPE type)
{
   pENTRY_TYPE ptr=pList->head;
   cmsLog_debug("\nThe List: pList->head %p, pList->tail %p\n",pList->head, pList->tail);
   
   while (ptr != NULL)
   {
      if (ptr->key != NULL) 
      {
         if (type == KEY_INT)
         {
            cmsLog_debug("ptr %p, %d\n",ptr,*((int*)(ptr->key)));
         }
         else
         {
            cmsLog_debug("%s\n",(char*)ptr->key);
         }
      }
      else
      {
         /* key is NULL, there is nothing to display, just exit */
         break;
      }
      ptr = ptr->next;
   } /* while */
}

/* find a particular entry with input key, return found or not found */
int findEntry(pLIST_TYPE pList, const void *key, LIST_KEY_TYPE type, pENTRY_TYPE *prevPtr, pENTRY_TYPE *ptr)
{
   int found = 0;

   if (pList == NULL)
   {
      return found;
   }

   if (type == KEY_INT)
   {
      found = findIntEntry(pList, (*(int*)key),prevPtr,ptr);
   }
   else
   {
      found = findStrEntry(pList, (const char*)key,prevPtr,ptr);
   }
   return (found);
}

#if 0
main(int argc, char **argv)
{
   int i;
   int number;
   int *pData, *pKey;
   int array[11];
   pENTRY_TYPE pTestEntry;
   LIST_TYPE  testList = {NULL,NULL};
   
   for (i =1; i<=10; i++)
   {
      pTestEntry = (pENTRY_TYPE)malloc(sizeof(ENTRY_TYPE));
      if (pTestEntry == NULL)
      {
         cmsLog_debug("error allocating pTestEntry\n");
      }
      pData = malloc(sizeof(int));
      if (pData == NULL)
      {
         cmsLog_debug("error allocating data\n");
         free(pTestEntry);
         return;
      }
      *pData = i;
      pKey = malloc(sizeof(int));
      if (pKey == NULL)
      {
         cmsLog_debug("error allocating Key\n");
         free(pTestEntry);
         free(pData);
         return;
      }
      *pKey = i;
      pTestEntry->next=NULL;
      pTestEntry->data = (void*) pData;
      pTestEntry->keyType = KEY_INT;
      pTestEntry->key = (void*)pKey;
      addEnd(pTestEntry,&testList);
   }
   printList(&testList,KEY_INT);

   cmsLog_debug("\n========Removing entries ===========\n");
   
   while (1)
   {
      //      pTestEntry = removeFront(&testList);
      pTestEntry = removeEnd(&testList);
      if (pTestEntry == NULL)
      {
         cmsLog_debug("there is no more entry to remove\n");
         break;
      }

      cmsLog_debug("removed ptr %p, entry->data: %d\n",pTestEntry,*((int*)(pTestEntry->key)));
      free(pTestEntry->data);
      free(pTestEntry->key);
      free(pTestEntry);
   } /* while */

   cmsLog_debug("\n---------Add Random number to List -----------\n");

   for (i =0; i<=10; i++)
   {
      array[i] = 0;
   }
   for (i =1; i<=10; i++)
   {
      number = rand() % 10 + 1;
      while (array[number] != 0)
      {
         /* number is already used */
         number = rand() % 10 + 1;
      }
      array[number] = number;


      pTestEntry = (pENTRY_TYPE)malloc(sizeof(ENTRY_TYPE));
      if (pTestEntry == NULL)
      {
         cmsLog_debug("error allocating pTestEntry\n");
      }
      pData = malloc(sizeof(int));
      if (pData == NULL)
      {
         cmsLog_debug("error allocating data\n");
         free(pTestEntry);
         return;
      }
      *pData = number;
      pKey = malloc(sizeof(int));
      if (pKey == NULL)
      {
         cmsLog_debug("error allocating Key\n");
         free(pTestEntry);
         free(pData);
         return;
      }
      *pKey = number;
      pTestEntry->next=NULL;
      pTestEntry->data = (void*) pData;
      pTestEntry->keyType = KEY_INT;
      pTestEntry->key = (void*)pKey;
      addFront(pTestEntry,&testList);
   }
   printList(&testList,KEY_INT);


   cmsLog_debug("\n================= SORTED ================\n");
   sortIntList(&testList);

   printList(&testList,KEY_INT);
}

#endif
