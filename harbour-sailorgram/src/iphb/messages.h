/**
   @file messages.h

   Message type definitions for DSME.
   <p>
   Copyright (C) 2004-2009 Nokia Corporation.

   @author Ari Saastamoinen
   @author Ismo Laitinen <ismo.laitinen@nokia.com>
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

#ifndef DSME_MESSAGES_H
#define DSME_MESSAGES_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DSME_MSG_NEW(T) \
  (T*)dsmemsg_new(DSME_MSG_ID_(T), sizeof(T), 0)

#define DSME_MSG_NEW_WITH_EXTRA(T, E) \
  (T*)dsmemsg_new(DSME_MSG_ID_(T), sizeof(T), (E))

#ifdef __cplusplus
#define DSME_MSG_INIT(T)               \
  (T){                                 \
    /* line_size_ = */ sizeof(T),      \
    /* size_      = */ sizeof(T),      \
    /* type_      = */ DSME_MSG_ID_(T) \
  }
#else
#define DSME_MSG_INIT(T)         \
  (T){                           \
    .line_size_ = sizeof(T),      \
    .size_      = sizeof(T),      \
    .type_      = DSME_MSG_ID_(T) \
  }
#endif


#define DSME_MSG_ID_(T)      T ## _ID_
#define DSME_MSG_ENUM(T, ID) DSME_MSG_ID_(T) = ID


/* TODO: either rename DSMEMSG->DSME_MSG or DSME_MSG->DSMEMSG */
#define DSMEMSG_EXTRA(M)                                              \
  (((const dsmemsg_generic_t*)(M))->line_size_ -                      \
   ((const dsmemsg_generic_t*)(M))->size_ > 0 ?                       \
     (void*)(((char*)(M)) + ((const dsmemsg_generic_t*)(M))->size_) : \
     (void*)0)

#define DSMEMSG_EXTRA_SIZE(M)                       \
  (((const dsmemsg_generic_t*)(M))->line_size_ -    \
    ((const dsmemsg_generic_t*)(M))->size_ > 0 ?    \
      ((const dsmemsg_generic_t*)(M))->line_size_ - \
        ((const dsmemsg_generic_t*)(M))->size_ :    \
      0)

#define DSMEMSG_CAST(T, M)                                       \
 ((((const dsmemsg_generic_t*)(M))->size_ == sizeof(T)) ?        \
    (((const dsmemsg_generic_t*)(M))->type_ == DSME_MSG_ID_(T) ? \
      (T*)(M) : 0) : 0)


/**
 * @defgroup message_if Message passing interface for modules
 * In addition to these functions, dsmesock_send() can be used from modules.
 */

#define DSMEMSG_PRIVATE_FIELDS \
  u_int32_t line_size_;        \
  u_int32_t size_;             \
  u_int32_t type_;

/**
   Generic message type
   @ingroup message_if
*/
typedef struct dsmemsg_generic_t {
  DSMEMSG_PRIVATE_FIELDS
} dsmemsg_generic_t;


/**
   Specific message type that is sent when socket is closed
   @ingroup message_if
*/
typedef struct {
  DSMEMSG_PRIVATE_FIELDS
  u_int8_t reason;
} DSM_MSGTYPE_CLOSE;

/**
   Close reasons
   @ingroup dsmesock_client
*/
enum {
  TSMSG_CLOSE_REASON_OOS = 0, /* Protocol out of sync (local) */
  TSMSG_CLOSE_REASON_EOF = 1, /* EOF read from socket (local) */
  TSMSG_CLOSE_REASON_REQ = 2, /* Peer requests close */
  TSMSG_CLOSE_REASON_ERR = 3  /* Undefined error conditions or read after close */
};


typedef dsmemsg_generic_t DSM_MSGTYPE_GET_VERSION;
typedef dsmemsg_generic_t DSM_MSGTYPE_DSME_VERSION;

/* TA stands for Type Approval: */
typedef dsmemsg_generic_t DSM_MSGTYPE_SET_TA_TEST_MODE;


enum {
    /* DSME Protocol messages 000000xx */
    DSME_MSG_ENUM(DSM_MSGTYPE_CLOSE,            0x00000001),

    /* misc */
    DSME_MSG_ENUM(DSM_MSGTYPE_GET_VERSION,      0x00001100),
    DSME_MSG_ENUM(DSM_MSGTYPE_DSME_VERSION,     0x00001101),
    DSME_MSG_ENUM(DSM_MSGTYPE_SET_TA_TEST_MODE, 0x00001102),
};

void* dsmemsg_new(u_int32_t id, size_t size, size_t extra);

u_int32_t dsmemsg_id(const dsmemsg_generic_t* msg);

#ifdef __cplusplus
}
#endif

#endif
