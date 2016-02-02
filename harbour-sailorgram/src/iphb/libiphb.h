/**
   @brief This is the interface to IP heartbeat service (via libiphb).

   @file libiphb.h

   This is the interface to IP heartbeat service (via libiphb).

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
#ifndef IPHB_H
#define IPHB_H

#include <time.h>


/**
   Handle to IP heartbeat service (NULL is invalid handle)
*/
typedef void * iphb_t;




/**
   Open iphb service.

   @param       dummy (for compatibility), can be NULL

   @return	handle for iphb, NULL if error (check errno).
                If error, behave just like before (i.e. no heartbeat service)

*/

iphb_t iphb_open(int *dummy);







/**
   "Global sync" predefined values (slots), see iphb_wait() function.
   The timeline is divided into "fixed global slots (GS)" (all waiters for a certain slot
   are woken up at the same time (also the lower-value waiters).
 */
#define IPHB_GS_WAIT_30_SEC         30   //!< 30 second wakeup slot
#define IPHB_GS_WAIT_2_5_MINS (2*60+30)  //!< 2.5 minute wakeup slot, the users of the previous slots wake here as well
#define IPHB_GS_WAIT_5_MINS   (5*60)     //!< 5 minute wakeup slot, the users of the previous slots wake here as well
#define IPHB_GS_WAIT_10_MINS  (10*60)    //!< 10 minute wakeup slot, the users of the previous slots wake here as well;
                                         //      you can use any multiplication of IPHB_GS_WAIT_10_MINS, although it
                                         //      is recommended to use these predefined values
#define IPHB_GS_WAIT_30_MINS  (30*60)    //!< 30 minute wakeup slot, the users of the previous slots wake here as well
#define IPHB_GS_WAIT_1_HOUR   (60*60)    //!< 1 hour wakeup slot, the users of the previous slots wake here as well
#define IPHB_GS_WAIT_2_HOURS  (2*60*60)  //!< 2 hours wakeup slot, the users of the previous slots wake here as well
#define IPHB_GS_WAIT_10_HOURS (10*60*60) //!< 10 hours wakeup slot, the users of the previous slots wake here as well



/**
   Wait for the next heartbeat.


   @param iphbh		Handle got from iphb_open
   @param mintime	Time in seconds that MUST be waited before heartbeat is reacted to.
                        Value 0 means 'wake me up when someboy else is woken'
   @param maxtime	Time in seconds when the wait MUST end. It is wise to have maxtime-mintime quite big so all users of this service get synced.
                        It is also possible to use "global synchronization" by setting a IPHB_GS_WAIT_* value to
                        both mintime and maxtime (same value for both) - you will be waken up when the next proper slot
                        is fired.
   @param must_wait	1 if this functions waits for heartbeat, 0 if you are going to use select/poll (see iphb_get_fd)

   @return		Time waited, (time_t)-1 if error (check errno)


*/
time_t
iphb_wait(iphb_t iphbh, unsigned short mintime, unsigned short maxtime, int must_wait);






/**
   This function should be called if the application
   has woken up by some other method than via iphb.

   @param iphbh		Handle got from iphb_open

   @return		>=0 if OK (number of bytes wakeup bytes discarded), -1 if error (check errno)


*/
int
iphb_I_woke_up(iphb_t iphbh);




/**
   This function is private for Qt wrapper.
   has woken up by some other method than via iphb.

   @param iphbh		Handle got from iphb_open

   @return		>=0 if OK (number of bytes discarded), -1 if error (check errno)


*/
int
iphb_discard_wakeups(iphb_t iphbh);











/**
   Get file descriptor for iphb (for use with select()/poll())

   @param iphbh	Handle got from iphb_open

   @return	Descriptor that can be used for select/poll, -1 if error (check errno)
*/

int iphb_get_fd(iphb_t iphbh);





/** iphbd statistics
   - unsigned int clients: number of active IPHB clients
   - unsigned int waiting: number of IPHB clients that are waiting for heartbeat
   - unsigned int next_hb: number of seconds after the next heartbeat shall occur, 0 if there are nobody waiting
*/

struct iphb_stats {
  unsigned int     clients;
  unsigned int     waiting;
  unsigned int     next_hb;
};


/**
   Get statistics. Struct iphb_stats is filled as follows:<br>
   - unsigned int clients: number of active IPHB clients
   - unsigned int waiting: number of IPHB clients that are waiting for heartbeat
   - unsigned int next_hb: number of seconds after the next heartbeat shall occur, 0 if there are nobody waiting


   @param iphbh	Handle got from iphb_open
   @param stats Statistics placeholder (filled when success)

   @return	0 if OK, -1 if error (check errno)
*/

int iphb_get_stats(iphb_t iphbh, struct iphb_stats *stats);





/**
   Close iphb service.

   @param iphbh	Handle got from iphb_subscribe

   @return	NULL always (so it is nice to set local handle to NULL)
*/
iphb_t iphb_close(iphb_t iphbh);



#endif  /* IPHB_H */
