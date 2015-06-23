/*
 * Copyright 2014 Canonical Ltd.
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
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 */

#include "audio.h"
#include "chat.h"
#include "chatfull.h"
#include "chatlocated.h"
#include "chatparticipant.h"
#include "chatparticipants.h"
#include "chatphoto.h"
#include "contact.h"
#include "contactblocked.h"
#include "contactlink.h"
#include "contactslink.h"
#include "contactstatus.h"
#include "dcoption.h"
#include "decryptedmessage.h"
#include "decryptedmessageaction.h"
#include "decryptedmessagemedia.h"
#include "documentattribute.h"
#include "dialog.h"
#include "privacyrule.h"
#include "inputprivacyrule.h"
#include "accountdaysttl.h"
#include "stickerpack.h"
#include "privacykey.h"
#include "inputprivacykey.h"
#include "document.h"
#include "inputencryptedchat.h"
#include "inputencryptedfile.h"
#include "filelocation.h"
#include "encryptedchat.h"
#include "encryptedfile.h"
#include "encryptedmessage.h"
#include "geochatmessage.h"
#include "geopoint.h"
#include "importedcontact.h"
#include "inputaudio.h"
#include "inputchatphoto.h"
#include "inputcontact.h"
#include "inputdocument.h"
#include "inputfile.h"
#include "inputfilelocation.h"
#include "inputgeopoint.h"
#include "inputmedia.h"
#include "inputnotifypeer.h"
#include "inputpeer.h"
#include "inputpeernotifysettings.h"
#include "inputphoto.h"
#include "inputphotocrop.h"
#include "inputuser.h"
#include "inputvideo.h"
#include "message.h"
#include "messageaction.h"
#include "messagemedia.h"
#include "messagesfilter.h"
#include "notifypeer.h"
#include "peer.h"
#include "peernotifysettings.h"
#include "photo.h"
#include "photosize.h"
#include "storagefiletype.h"
#include "update.h"
#include "updatesstate.h"
#include "user.h"
#include "userprofilephoto.h"
#include "userstatus.h"
#include "video.h"
#include "wallpaper.h"
