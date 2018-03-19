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

/******************************************************************************
//
//  Filename:       main.c
//  Author:         Paul J.Y. Lahaie
//  Creation Date:  14/06/04
//
//  Description:
//      TODO
//
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <string.h>
#include <signal.h>

#include "conn.h"
#include "printers.h"
#ifdef DEBUG_MEM
#include <dmalloc.h>
#endif

#define LISTEN_BACKLOG 5

#define IPP_PORT 631

#define MAX_CLIENTS 20

extern int ipp_add_tag( IPP *ipp, char tag_type, char *name,  void *value, short val_len, int set );
extern int ipp_copy_tag( IPP *dst, IPP *src, char seq, char *name );
extern int ipp_write( IPP *ipp, char *buffer, int len );
extern void ipp_free( IPP *ipp );


extern ARRAY *printers;
Printer *printer;

void quit_handler(int a __attribute__ ((unused))) {
  exit(1);
}

int
StartListening(void) {
  int lis;
  int val = 1;
  struct sockaddr_in lis_addr;

  if((lis = socket(AF_INET, SOCK_STREAM, 0 )) == -1 ) {
    fprintf(stderr,"Unable to open listening socket.\n");
    exit(1);
  }

  memset( &lis_addr, 0, sizeof( lis_addr ) ); // Zero out server addr
  lis_addr.sin_family = AF_INET;
  lis_addr.sin_port = ntohs( IPP_PORT );

  fcntl( lis, F_SETFD, fcntl(lis, F_GETFD ) | FD_CLOEXEC );

  setsockopt( lis, SOL_SOCKET, SO_REUSEADDR, &val, sizeof( val ));

  if(bind(lis, (struct sockaddr *)&(lis_addr), sizeof(lis_addr)) < 0 ) {
    fprintf( stderr, "Unable to bind socket - %s\n", strerror( errno ));
    exit(1);
  }

  if( listen(lis, LISTEN_BACKLOG) < 0 ) {
    fprintf( stderr, "Unable to listen for clients - %s\n", strerror(errno));
    exit(1);
  }
  return lis;
}

int
main( int argc, char **argv ) {
   int num_polled = 1;
   struct pollfd *polls;
   struct sockaddr_in incoming;
   int i, n;
   int new_fd;
   socklen_t addr_len;
   Connection *conn;

   signal( SIGUSR1, quit_handler );

   // Temp printer for now
   if( argc < 2 ) {
     fprintf(stderr, "Missing argument pointing to location of printer definition file.\n" );
     exit(1);
   }

   if(!init_printers( argv[1] )) {
     fprintf( stderr, "Unable to load printer definition file %s\n", argv[1] );
     exit(1);
   }

   new_fd = StartListening();

   polls = malloc( sizeof( struct pollfd )  * MAX_CLIENTS);
   polls[0].fd = new_fd;
   polls[0].events = POLLIN;

   for(;;) {
     // printf(".");
     for( conn = list_conn( NULL ), num_polled = 1; conn != NULL; conn = list_conn( conn ) ) {
       if( conn->state == CONN_BEGIN || conn->state == CONN_PRINTING_READ ) {
         polls[num_polled].fd = conn->fd;
         polls[num_polled++].events = POLLIN;
       } else if( conn->state == CONN_OUTPUT ) {
         polls[num_polled].fd = conn->fd;
         polls[num_polled++].events = POLLOUT;
       }
     }
     for( i = 0; i < array_len( printers ); i++ ) {
       printer = array_get( printers, i );
       if( printer->state == PRINTER_PRINTING_WRITE ) {
         polls[num_polled].fd = printer->fd;
         polls[num_polled++].events = POLLOUT;
       }
     }

     poll( polls, num_polled, -1 );
     // printf("o" );
     for( i = 0; i < num_polled; i++ ) {
       if( polls[i].revents & POLLIN ) {
         // Read
         if( i == 0 ) { // Special case..
           addr_len = sizeof( struct sockaddr_in );
           new_fd = accept( polls[i].fd, (struct sockaddr *)&(incoming), &addr_len );
#ifdef IPP_DEBUG
           printf("Adding connection with descriptor: %d\n", new_fd);
#endif
           add_conn( new_fd );
         } else {
           if( ( conn = get_conn( polls[i].fd ) ) ) {
             process_conn( conn );
           }
         }
       } else if ( polls[i].revents & POLLNVAL ) {
         if( ( conn = get_conn( polls[i].fd ) ) )
           remove_conn( conn );
       } else if ( polls[i].revents & POLLOUT ) {
         if( ( conn = get_conn( polls[i].fd ) ) ) {
           if( ( n = write( conn->fd, conn->buffer+conn->buf_ptr, conn->used-conn->buf_ptr ) ) ) {
             conn->buf_ptr += n;
             if( conn->buf_ptr == conn->used ) {
               close( conn->fd );
               remove_conn( conn );
             }
           }
         } else {
           int e;

           for( e = 0; e < array_len( printers ); e++ ) {
             printer = array_get( printers, e );
             if( printer->fd == polls[i].fd ) { 
               if( ( n = write( printer->fd, printer->buffer+printer->buf_ptr, printer->used-printer->buf_ptr ) ) < 0 ) {
                 // penguin 20050916, if write process was error, it must be something wrong with printer, close it.
                 printf("ippd: ERROR WRITING TO PRINTER! Close it\n");

                 // Simple error support -- close printer fd
                 if( ( conn = get_conn( printer->jobs->job.fd ) ) ) {
                   int pos, len=0;
                   struct _ipp_jobs *old_job;
                   IPP *response;

                   fprintf( stderr, "Print job error.\n" );
                   response = ipp_new();
                   response->response = 0x0504;
                   response->version = 256; // IPP 1.0
                   response->request_id = conn->ipp->request_id;
                   ipp_add_tag( response, IPP_TAG_OPERATIONS, NULL, NULL, 0, 0 );
                   ipp_copy_tag( response, conn->ipp, IPP_TAG_OPERATIONS, "attributes-charset" );
                   ipp_copy_tag( response, conn->ipp, IPP_TAG_OPERATIONS, "attributes-natural-language" );
                   len = ipp_write( response, NULL, len );
                   pos = sprintf( conn->buffer, "HTTP/1.1 200 OK\r\nContent-Type: application/ipp\r\nContent-Length: %d\r\n\r\n", len );
                   ipp_write( response, conn->buffer+pos, len );
                   conn->used = len + pos;
                   conn->buf_ptr = 0;
                   conn->state = CONN_OUTPUT;
                   ipp_free( response );
                   // The JOB being is printed is ALWAYS the first one
                   old_job = printer->jobs;
                   printer->jobs = old_job->next;
                   free( old_job ); // Free the job

                   printf( "Going to next job...\n");
                 }
                 fprintf( stderr, "Marking printer closed.\n" );
                 close(printer->fd );
                 printer->state = PRINTER_CLOSED;
               } else {
                 printer->buf_ptr += n;
                 if( printer->buf_ptr == printer->used ) {
                   printer->state = PRINTER_PRINTING_WAIT;
                   printer->buf_ptr = printer->used = 0;
                   if( ( conn = get_conn( printer->jobs->job.fd ) ) == NULL ) {
                     printf( "Bad printer state.\n" );
                   }
                   if( conn->ipp->data_left )
                     conn->state = CONN_PRINTING_READ;
                   else {
                     int pos, len=0, val;
                     struct _ipp_jobs *old_job;
                     IPP *response;

                     // Generate a done message
                     response = ipp_new();
                     response->response = 0x0000; // Success
                     response->request_id = conn->ipp->request_id;
                     response->version = 256;
                     ipp_add_tag( response, IPP_TAG_OPERATIONS, NULL, NULL, 0, 0 );
                     ipp_copy_tag( response, conn->ipp, IPP_TAG_OPERATIONS, "attributes-charset" );
                     ipp_copy_tag( response, conn->ipp, IPP_TAG_OPERATIONS, "attributes-natural-language" );
                     /* 2004/11/8 : Added to make XP printer utility report success status*/
                     ipp_add_tag( response, IPP_TAG_TEXT_WO_LANG, "status-message", "successful-ok", strlen( "successful-ok" ), 0 );
                     ipp_add_tag( response, IPP_TAG_JOBS, NULL, NULL, 0, 0 );
                     val = printer->jobs->job.id;
                     val = htonl(val);
                     ipp_add_tag( response, IPP_TAG_INTEGERS, "job-id", &val, 4, 0 );
                     val = 9;
                     //penguin 20050922, correct the order
                     val = htonl(val);
                     ipp_add_tag( response, IPP_TAG_ENUM, "job-state", &val, 4, 0 ); /* 9 = COMPLETED */
                     // Done
                     len = ipp_write( response, NULL, len );
                     pos = sprintf( conn->buffer, "HTTP/1.1 200 OK\r\nContent-Type: application/ipp\r\nContent-Length: %d\r\n\r\n", len );
                     ipp_write( response, conn->buffer + pos, len );
                     conn->used = len + pos;
                     conn->buf_ptr = 0;
                     conn->state = CONN_OUTPUT;
                     /* 2004/11/8: added to free response */
                     ipp_free( response );
                     // The JOB being is printed is ALWAYS the first one
                     old_job = printer->jobs;
                     printer->jobs = old_job->next;
                     free( old_job ); // Free the job

                     printf( "Going to next job...\n" );
                     if( printer->jobs ) {
                       if( ( conn = get_conn( printer->jobs->job.fd ) ) ) {
                         if( conn->buf_ptr < conn->used ) {
                           memcpy( printer->buffer, conn->buffer + conn->buf_ptr, conn->used - conn->buf_ptr );
                           printer->used = conn->used - conn->buf_ptr;
                           printer->buf_ptr = 0;
                           printer->state = PRINTER_PRINTING_WRITE;
                           conn->state = CONN_PRINTING_WAIT;
                           conn->ipp->data_left -= conn->used - conn->buf_ptr;
                         } else {
                           if( conn->ipp->data_left ) {
                             printer->state = PRINTER_PRINTING_WAIT;
                             conn->state = CONN_PRINTING_READ;
                           }
                         }
                       } else {
                         printf( "Unable to get connection for FD - %d.\n", printer->jobs->job.fd );
                       }
                     } else {
                        /* printer->state = PRINTER_OPEN; */
                     	/* 2004/11/5 : put back into closed state so that kernel can remove lp0 when printer unplugged */
  	  					close(printer->fd);
  	  					printer->fd = 0;
  	  					printer->state = PRINTER_CLOSED;
                     }
                   }
                 } 
               }
             }
           }
         }  
       }
     }
  }
}
