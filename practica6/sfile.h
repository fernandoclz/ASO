/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* sfile.h
   Header file for file_server.c and file_client.c.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <linux/limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define SERVER_QUEUE_NAME   "/file_server_q"
#define MAX_MSG_SIZE        8192
#define MAX_CLIENTS         10

typedef struct {
    char filename[256];
    char client_queue_name[64];
} client_request_t;