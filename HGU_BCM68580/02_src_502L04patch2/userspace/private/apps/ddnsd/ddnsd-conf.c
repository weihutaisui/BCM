/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ddnsd-conf.h"

#define LINE_SIZE 1024

typedef struct BcmDDnsListItems { char **vals;
                                  struct BcmDDnsListItems *next; } BcmDDnsListItems;

typedef BcmDDnsListItems * pBcmDDnsListItems;

typedef struct BcmDDnsList { int num_vals;
                             pBcmDDnsListItems items; } BcmDDnsList;

typedef BcmDDnsList * pBcmDDnsList;

static pBcmDDnsList ddnsConfig = NULL;
static pBcmDDnsList ddnsCache = NULL;

static int BcmDDnsList_add( pBcmDDnsList, char **vals );
static void BcmDDnsList_close( pBcmDDnsList );

int
BcmDDnsConfig_init( char *pszFilename ) {
   FILE *fp;
   int val_idx;
   char line[LINE_SIZE], *value, *variable;
   int line_num = 0;
   char *vals[5];
//   char *val_names[] = { "interface", "service", "hostname", "username", "password", NULL };

   if( ( fp = fopen( pszFilename, "r" ) ) == NULL ) {
     return -1;
   }

   if ( ( ddnsConfig = malloc( sizeof( BcmDDnsList ) ) ) == NULL ) {
     fclose( fp );
     return -1;
   }

   ddnsConfig->num_vals = 5;
   ddnsConfig->items = NULL;
   
   memset( vals, 0, sizeof( vals ) );

   while( fgets( line, LINE_SIZE, fp ) != NULL ) {
     line_num++;
     switch( line[0] ) {
     case '\n':
     case '#':
     case ';':
       continue;  // Skip comments
     case '[': // New interface
       // Verify the last interface had all the values
       if( !vals[IFACE] || !vals[HOSTNAME] || !vals[SERVICE] || !vals[USERNAME] || !vals[PASSWORD] ) {
         // fprintf( stderr, "Discarding interface with missing information: Missing " );

         for( val_idx = 0; val_idx < 5; val_idx++ ) {
           if( vals[val_idx] )
             free( vals[val_idx] );
         }
       } else {
         BcmDDnsList_add( ddnsConfig, vals );
//         memset( vals, 0, sizeof( vals ) );
       }

       memset( vals, 0, sizeof( vals ) );

       value = strtok( line+1, "]\n" );

       if( value && !strlen( value ) )
         continue;

       vals[HOSTNAME] = strdup( value );
       break;
     default:
       variable = strtok( line, "=\n" );
       value = strtok( NULL, "\n" );
       if ( value && strlen( value ) ) {
         if ( !strcmp( variable, "interface" ) ) {
           if( vals[IFACE] )
             free( vals[IFACE] );
           vals[IFACE] = strdup( value );
         } else if( !strcmp( variable, "username" ) ) {
           if( vals[USERNAME] )
             free( vals[USERNAME] );
           vals[USERNAME] = strdup( value );
         } else if ( !strcmp( variable, "password" ) ) {
           if( vals[PASSWORD] )
             free( vals[PASSWORD] );
           vals[PASSWORD] = strdup( value );
         } else if ( !strcmp( variable, "service" ) ) {
           if( !strcmp( value, "tzo" ) && !strcmp( value, "dyndns" ) ) {
             fprintf( stderr, "Service %s not supported. (Line %d)\n", value, line_num );
             continue;
           }
           if( vals[SERVICE] )
             free( vals[SERVICE] );
           vals[SERVICE]= strdup( value );
         } else {
           fprintf( stderr, "Invalid key %s on line %d.\n", variable, line_num );
         }
       }
     }
   }

   // P.T. clean up resource
   fclose( fp );

   if( !vals[IFACE] || !vals[HOSTNAME] || !vals[SERVICE] || !vals[USERNAME] || !vals[PASSWORD] ) {
     // fprintf( stderr, "Discarding interface with missing information: Missing " );

     for( val_idx = 0; val_idx < 5; val_idx++ ) {
       if( vals[val_idx] )
         free( vals[val_idx] );
     }

   } else {
     BcmDDnsList_add( ddnsConfig, vals );
   }
   return 0;
}

static int
BcmDDnsList_add( pBcmDDnsList ddnsList, char **vals ) {
  pBcmDDnsListItems to_add, list;

  if( ( to_add = malloc( sizeof( BcmDDnsListItems ) ) ) == NULL ) {
    return -1;
  }
  if( ( to_add->vals = malloc( sizeof( char * ) * ddnsList->num_vals ) ) == NULL ) {
    free( to_add );
    return -1;
  }
  to_add->next = NULL;
  memcpy( to_add->vals, vals, ddnsList->num_vals * sizeof( char * ) );
  if( ddnsList->items == NULL ) {
    /* Add the first item */
    ddnsList->items = to_add;
  } else {
    for( list = ddnsList->items; list->next != NULL; list = list->next );
    /* Get to the end of the list to append */
    list->next = to_add;
  }
  return 0;
}

void
BcmDDnsConfig_iter( void (*func)(char *) ) {
  pBcmDDnsListItems list;

  for( list = ddnsConfig->items; list != NULL; list = list->next ) {
    if( list->vals )
      func( list->vals[HOSTNAME] );
  }
}

void
BcmDDnsConfig_remove( char *hostname ) {
  pBcmDDnsListItems list;
  int i;
  pBcmDDnsListItems prev = NULL;
                                                                                
  for (list = ddnsConfig->items; list != NULL; prev = list, list = list->next ) {
    if ( list->vals && list->vals[HOSTNAME] && !strcmp( hostname, list->vals[HOSTNAME] ) ) {
      for( i = 0; i < ddnsConfig->num_vals; i++ ) {
        free( list->vals[i] );
      }
      free( list->vals );
      list->vals = NULL;
      if (prev) 
        prev->next = list->next;
      else
        ddnsConfig->items = list->next;
      free( list );
      return;
    }
  } 
}

char *
BcmDDnsConfig_getInterface( char *hostname ) {
  pBcmDDnsListItems list;

  for (list = ddnsConfig->items; list != NULL; list = list->next ) {
    if ( list->vals && !strcmp( hostname, list->vals[HOSTNAME] ) ) {
      return list->vals[IFACE];
    }
  }
  return NULL;
}

char *
BcmDDnsConfig_getService( char *hostname ) {
  pBcmDDnsListItems list;

  for (list = ddnsConfig->items; list != NULL; list = list->next ) {
    if ( list->vals && !strcmp( hostname, list->vals[HOSTNAME] ) ) {
      return list->vals[SERVICE];
    }
  }
  return NULL;
}

char *
BcmDDnsConfig_getUsername( char *hostname ) {
  pBcmDDnsListItems list;

  for (list = ddnsConfig->items; list != NULL; list = list->next ) {
    if ( list->vals && !strcmp( hostname, list->vals[HOSTNAME] ) ) {
      return list->vals[USERNAME];
    }
  }
  return NULL;
}

char *
BcmDDnsConfig_getPassword( char *hostname ) {
  pBcmDDnsListItems list;

  for (list = ddnsConfig->items; list != NULL; list = list->next ) {
    if ( list->vals && !strcmp( hostname, list->vals[HOSTNAME] ) ) {
      return list->vals[PASSWORD];
    }
  }
  return NULL;
}


void
BcmDDnsConfig_close( void ) {
   BcmDDnsList_close( ddnsConfig );
   ddnsConfig = NULL;
}


void
BcmDDnsList_close( pBcmDDnsList ddnsList ) {
  pBcmDDnsListItems prev=NULL, list;
  int i;

  for( list = ddnsConfig->items; list != NULL; list = list->next ) {
    if (prev != NULL )
      free( prev );
    prev = list;
    if (list->vals) {
      for( i = 0; i < ddnsList->num_vals; i++ )
        if( list->vals[i] != NULL )
          free( list->vals[i] );
      free( list->vals );
    }
  }
  if( prev != NULL )
    free( prev );
  ddnsList->items = NULL;
  free( ddnsList );
}

int
BcmDDnsCache_init( char *filename ) {
  char line[LINE_SIZE], *vals[2];
  FILE *fp;

  if ( (ddnsCache = malloc( sizeof( pBcmDDnsList ) ) ) == NULL ) {
    return -1;
  }
  ddnsCache->num_vals = 2;
  ddnsCache->items = NULL;

  if ((fp = fopen( filename, "r" ) ) == NULL )
    return 0;

  while((fgets(line, LINE_SIZE, fp ) ) != NULL ) {
    vals[HOSTNAME] = strdup( strtok(line, "," ) );
    vals[CACHE_IP] = strdup( strtok( NULL, "\n" ) );
    BcmDDnsList_add( ddnsCache, vals );
  }

  // P.T. clean up resource
  fclose( fp );
  return 0;
}

char *
BcmDDnsCache_get( char *hostname ) {
  pBcmDDnsListItems list;

  if (ddnsCache == NULL ) {
    return NULL;
  }

  for( list = ddnsCache->items; list != NULL; list = list->next ) {
    if ( !strcmp( hostname, list->vals[HOSTNAME] ) ) {
      return list->vals[CACHE_IP];
    }
  }
  return NULL;
}

void
BcmDDnsCache_update( char *hostname, char *address ) {
  pBcmDDnsListItems list;
  char *vals[2];

  if (ddnsCache == NULL )
    return;

  for( list = ddnsCache->items; list != NULL; list = list->next ) {
    if (!strcmp( hostname, list->vals[HOSTNAME] ) ) {
      if (list->vals[CACHE_IP] )
        free(list->vals[CACHE_IP]);
      list->vals[CACHE_IP] = strdup( address );
      return;
    }
  }
  vals[HOSTNAME] = strdup( hostname );
  vals[CACHE_IP] = strdup( address );
  BcmDDnsList_add( ddnsCache, vals );
}

void
BcmDDnsCache_save( char *pszFilename ) {
  pBcmDDnsListItems list;
  FILE *fp;

  if (ddnsCache == NULL || ddnsCache->items == NULL ) {
    return;
  }

  if( ( fp = fopen( pszFilename, "w" ) ) == NULL ) {
    return;
  }

  for (list = ddnsCache->items; list != NULL; list = list->next ) {
    fprintf( fp, "%s,%s\n", list->vals[HOSTNAME], list->vals[CACHE_IP] );
  }
  fclose( fp );
}

void
BcmDDnsCache_close() {
   BcmDDnsList_close( ddnsCache );
   ddnsCache = NULL;
}


#ifdef CONFIG_MAIN
int main(int argc, char **argv ) {
  pBcmDDnsListItems list;
  int i;

  BcmDDnsConfig_init( argv[1] );
  if (ddnsConfig == NULL || ddnsConfig->items == NULL) {
    printf("No interfaces defined.\n");
  } else {
    for( i = 0, list = ddnsConfig->items; list != NULL; list = list->next, i++ ) {
      printf( "Interface (%2d): %s\n", i, list->vals[IFACE] );
      printf( "Hostname      : %s\n", list->vals[HOSTNAME] );
      printf( "Service       : %s\n", list->vals[SERVICE] );
      printf( "Username      : %s\n", list->vals[USERNAME] );
      printf( "Password      : %s\n", list->vals[PASSWORD] );
      printf( "\n" );
    }
  }
}
#elif CACHE_MAIN
int main(int argc, char **argv ) {
  pBcmDDnsListItems list;

  BcmDDnsCache_init( argv[1] );  
  if (ddnsCache == NULL ) {
    puts( "Problem." );
    exit(1);
  }
  if( ddnsCache->items == NULL ) {
    puts("No interfaces defined.");
  } else {
    for (list = ddnsCache->items; list != NULL; list = list->next ) {
      printf( "%s cached as %s.\n", list->vals[HOSTNAME], list->vals[CACHE_IP] );
    }
  }
  BcmDDnsCache_update( "somewhere.not.who", "209.1.1.1" );
  for (list = ddnsCache->items; list != NULL; list = list->next ) {
    printf( "%s cached as %s.\n", list->vals[HOSTNAME], list->vals[CACHE_IP] );
  }

}
#endif
