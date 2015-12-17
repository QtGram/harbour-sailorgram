/*
 * Copyright 2014 Canonical Ltd.
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TLVALUES_H
#define TLVALUES_H

enum TLValues {
    // https://core.telegram.org/mtproto/auth_key
    TL_ReqPQ = 0x60469778,
    TL_ResPQ = 0x05162463,
    TL_ReqDHParams = 0xd712e4be,
    TL_PQInnerData = 0x83c95aec,
    TL_PQInnerDataTemp = 0x3c6a84d4,
    TL_ServerDHParamsOK = 0xd0e8075c,
    TL_ServerDHParamsFail = 0x79cb045d,
    TL_ServerDHInnerData = 0xb5890dba,
    TL_ClientDHInnerData = 0x6643b654,
    TL_SetClientDHParams = 0xf5045f1f,
    TL_DHGenOK = 0x3bcbf734,

    // https://core.telegram.org/mtproto/service_messages
    TL_RpcResult = 0xf35c6d01,
    TL_RpcError = 0x2144ca19,
    TL_RpcDropAnswer = 0x58e4a740,
    TL_RpcAnswerUnknown = 0x5e2ad36e,
    TL_RpcAnswerDroppedRunning = 0xcd78e586,
    TL_RpcAnswerDropped = 0xa43ad8b7,
    TL_GetFutureSalts = 0xb921bd04,
    TL_FutureSalt = 0x0949d9dc,
    TL_FutureSalts = 0xae500895,
    TL_Ping = 0x7abe77ec,
    TL_Pong = 0x347773c5,
    TL_PingDelayDisconnect = 0xf3427b8c,
    TL_DestroySession = 0xe7512126,
    TL_DestroySessionOk = 0xe22045fc,
    TL_DestroySessionNone = 0x62d350c9,
    TL_NewSessionCreated = 0x9ec20908,
    TL_MsgContainer = 0x73f1f8dc,
    TL_MsgCopy = 0xe06046b2,
    TL_GZipPacked = 0x3072cfa1,
    TL_HttpWait = 0x9299359f,

    // https://core.telegram.org/mtproto/service_messages_about_messages
    TL_MsgsAck = 0x62d6b459,
    TL_BadMsgNotification = 0xa7eff811,
    TL_BadServerSalt = 0xedab447b,
    TL_MsgsStateReq = 0xda69fb52,
    TL_MsgsStateInfo = 0x04deb57d,
    TL_MsgsAllInfo = 0x8cc0d131,
    TL_MsgDetailedInfo = 0x276d3ec6,
    TL_MsgNewDetailedInfo = 0x809db6df,
    TL_MsgResendReq = 0x7d861a08,

    // https://core.telegram.org/schema by generator
    TL_DecryptedMessageLayer = 0x1be31789,
    TL_DecryptedMessage_layer8 = 0x1f814f1f,
    TL_DecryptedMessage = 0x204d3878,
    TL_DecryptedMessageService_layer8 = 0xaa48327d,
    TL_DecryptedMessageService = 0x73164160,
    TL_DecryptedMessageMediaEmpty = 0x89f5c4a,
    TL_DecryptedMessageMediaPhoto = 0x32798a8c,
    TL_DecryptedMessageMediaVideo_layer8 = 0x4cee6ef3,
    TL_DecryptedMessageMediaVideo = 0x524a415d,
    TL_DecryptedMessageMediaGeoPoint = 0x35480a59,
    TL_DecryptedMessageMediaContact = 0x588a0a97,
    TL_DecryptedMessageMediaAudio_layer8 = 0x6080758f,
    TL_DecryptedMessageMediaAudio = 0x57e0a9cb,
    TL_DecryptedMessageMediaExternalDocument = 0xfa95b0dd,
    TL_DecryptedMessageActionSetMessageTTL = 0xa1733aec,
    TL_DecryptedMessageActionReadMessages = 0xc4f40be,
    TL_DecryptedMessageActionDeleteMessages = 0x65614304,
    TL_DecryptedMessageActionScreenshotMessages = 0x8ac1f475,
    TL_DecryptedMessageActionFlushHistory = 0x6719e45c,
    TL_DecryptedMessageActionResend = 0x511110b0,
    TL_DecryptedMessageActionNotifyLayer = 0xf3048883,
    TL_DecryptedMessageActionTyping = 0xccb27641
};

#endif // TLVALUES_H
