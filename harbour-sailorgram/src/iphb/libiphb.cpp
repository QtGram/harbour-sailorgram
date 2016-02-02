/**
   @brief Implementation of libiphb

   @file libiphb.c

   Implementation of libiphb (see libiphb.h)

   <p>
   Copyright (C) 2008-2011 Nokia Corporation.

   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com>

   This file is part of Dsme.

   Dsme is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Dsme is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Dsme.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
/* socket transport */
#include <sys/socket.h>
#include <sys/un.h>
/* --- */

#include "libiphb.h"
#include "iphb_internal.h"


/**@brief  Allocated structure for handle to iphbd */
struct _iphb_t {
  int fd;  		/*!< Unix domain socket handle */
};

#define HB_INST(x) ((struct _iphb_t *) (x))



static int
suck_data(int fd)
{
  int bytes = -1;
  int st;


  /* suck away unread messages */

  st = ioctl(fd, FIONREAD, &bytes);

  if (st != -1 && bytes >= 0) {
      if (bytes) {
          char *b = (char*)malloc(bytes);
          if (!b) {
              errno = ENOMEM;
              return -1;
          }
          (void)recv(fd, b, bytes, MSG_WAITALL);
          free(b);
      }
      return bytes;

  }
  else
      return -1;
}


int
iphb_I_woke_up(iphb_t iphbh)
{
  int st;
  struct _iphb_req_t  req;
  req.cmd = IPHB_WAIT;

  if (!iphbh) {
    errno = EINVAL;
    return -1;
  }

  st = suck_data(HB_INST(iphbh)->fd);

  req.u.wait.pid = getpid();
  req.u.wait.mintime = 0;
  req.u.wait.maxtime = 0;

  if (send(HB_INST(iphbh)->fd, &req, sizeof(req), MSG_DONTWAIT|MSG_NOSIGNAL) == -1)
    return -1;

  return st;

}




iphb_t
iphb_open(int *dummy)
{
  int 		     	fd;
  struct sockaddr_un 	addr;
  iphb_t	       *iphbh;

  iphbh = (iphb_t*) malloc(sizeof(struct _iphb_t));
  if (iphbh == NULL) {
    errno = ENOMEM;
    return 0;
  }
  fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    free(iphbh);
    return 0;
  }
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, HB_SOCKET_PATH);
  if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
    close(fd);
    free(iphbh);
    return 0;
  }
  else {
    HB_INST(iphbh)->fd = fd;
    if (iphb_I_woke_up(iphbh)) {
      close(fd);
      free(iphbh);
      return 0;
    }
    else {
      if (dummy)
        *dummy = 30; // for compatibility
      return iphbh;
    }
  }
}


int
iphb_get_fd(iphb_t iphbh)
{
  if (iphbh)
    return HB_INST(iphbh)->fd;
  else {
    errno = EINVAL;
    return -1;
  }
}




time_t
iphb_wait(iphb_t iphbh, unsigned short mintime, unsigned short maxtime, int must_wait)
{
  struct _iphb_req_t  req;
  struct _iphb_wait_resp_t resp = {0};

  req.cmd = IPHB_WAIT;

  if (!iphbh || mintime > maxtime || maxtime == 0) {
    errno = EINVAL;
    return (time_t)-1;
  }

  (void)suck_data(HB_INST(iphbh)->fd);

  req.u.wait.mintime = mintime;
  req.u.wait.maxtime = maxtime;
  req.u.wait.pid = getpid();

  if (send(HB_INST(iphbh)->fd, &req, sizeof(req), MSG_DONTWAIT|MSG_NOSIGNAL) <= 0)
    return (time_t)-1;

  if (!must_wait)
    return (time_t)0;

  fd_set 		readfds;
  struct timeval        timeout;
  struct timespec       ts_then;
  struct timespec       ts_now;
  int    		st;


  (void)clock_gettime(CLOCK_MONOTONIC, &ts_then);

  timeout.tv_sec = maxtime;
  timeout.tv_usec = 0;

  for (;;) {
    FD_ZERO(&readfds);
    FD_SET(HB_INST(iphbh)->fd, &readfds);
    st = select(HB_INST(iphbh)->fd + 1, &readfds, NULL, NULL, &timeout);

    (void)clock_gettime(CLOCK_MONOTONIC, &ts_now);

    if (st == -1 && errno == EINTR) {
    if (ts_now.tv_sec - ts_then.tv_sec < maxtime) {
      timeout.tv_sec = maxtime - (ts_now.tv_sec - ts_then.tv_sec);
      continue;
    }
    }
    break;
  }
  if (st == 0) /* timeout */
    return ts_now.tv_sec - ts_then.tv_sec;

  if (recv(HB_INST(iphbh)->fd, &resp, sizeof(resp), MSG_WAITALL) > 0)
    return resp.waited;
  else
    return (time_t)-1;
}


int
iphb_discard_wakeups(iphb_t iphbh)
{
  if (!iphbh) {
    errno = EINVAL;
    return (time_t)-1;
  }

  return suck_data(HB_INST(iphbh)->fd);
}



int iphb_get_stats(iphb_t iphbh, struct iphb_stats *stats)
{
  struct _iphb_req_t  req;
  int bytes = -1;

  req.cmd = IPHB_WAIT;

  if (!iphbh) {
    errno = EINVAL;
    return -1;
  }

  /* suck away unread messages */
  if (ioctl(HB_INST(iphbh)->fd, FIONREAD, &bytes) != -1 && bytes > 0) {
    char *b = (char*)malloc(bytes);
    if (!b) {
      errno = ENOMEM;
      return (time_t)-1;
    }
    (void)recv(HB_INST(iphbh)->fd, b, bytes, MSG_WAITALL);
    free(b);

  }

  if (send(HB_INST(iphbh)->fd, &req, sizeof(req), MSG_DONTWAIT|MSG_NOSIGNAL) <= 0)
    return -1;

  if (recv(HB_INST(iphbh)->fd, stats, sizeof(*stats), MSG_WAITALL) > 0)
    return 0;
  else
    return (time_t)-1;
}



iphb_t
iphb_close(iphb_t iphbh)
{
  if (iphbh) {
    close(HB_INST(iphbh)->fd);
    HB_INST(iphbh)->fd = 0;
    free(iphbh);
  }
  return 0;
}
