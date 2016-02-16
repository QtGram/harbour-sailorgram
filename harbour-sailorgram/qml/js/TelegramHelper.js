.pragma library

.import harbour.sailorgram.TelegramCalendar 1.0 as TelegramCalendar
.import "TelegramConstants.js" as TelegramConstants

function completeName(user)
{
    var name = user.firstName;

    if(user.lastName.length > 0)
        name += " " + user.lastName;

    return name;
}

function userDisplayName(user)
{
    if(user.firstName.length > 0)
        return user.firstName;

    if(user.lastName.length > 0)
        return user.lastName;

    return qsTr("Unknown");
}

function phoneNumber(user)
{
    return completePhoneNumber(user.phone);
}

function fallbackText(dialog, chatoruser) {
    if(isChat(dialog)) {
        var chat = chatoruser;
        var splittitle = chat.title.split(" ");

        if(splittitle.length >= 2)
            return splittitle[0].slice(0, 1).toUpperCase() + splittitle[1].slice(0, 1).toUpperCase();

        return splittitle[0][0].toUpperCase();
    }

    var user = chatoruser;
    var splitname = user.firstName.slice(0, 1).toUpperCase();

    if(user.lastName.length > 0)
        splitname += user.lastName.slice(0, 1).toUpperCase();

    return splitname;
}

function userStatus(user)
{
    switch(user.status.classType)
    {
        case TelegramConstants.typeUserStatusRecently:
            return qsTr("Recently");

        case TelegramConstants.typeUserStatusLastMonth:
            return qsTr("Last Month");

        case TelegramConstants.typeUserStatusLastWeek:
            return qsTr("Last Week");

        case TelegramConstants.typeUserStatusOnline:
            return qsTr("Online");

        case TelegramConstants.typeUserStatusOffline:
            return qsTr("Last Seen %1").arg(printableDate(user.status.wasOnline));

        default:
            break;
    }

    return qsTr("Unknown");
}

function isChat(dialog)
{
    if(!dialog)
        return false;

    if(dialog.encrypted)
        return false;

    return dialog.peer.classType === TelegramConstants.typePeerChat;
}

function printableDate(timestamp, full)
{
    var date = new Date(timestamp * 1000);
    var now = new Date(Date.now());

    if(full === true) // We want the entire date
        return Qt.formatDateTime(date, "dd MMM yy");

    if(now === date)
        return Qt.formatDateTime(date, "HH:mm");

    var daydiff = (now - date) / 86400000; // ms per day = 1000 * 60 * 60 * 24

    if(daydiff < 7)
        return Qt.formatDateTime(date, "ddd HH:mm");
    else if(date.getYear() === now.getYear())
        return Qt.formatDateTime(date, "dd MMM");

    return Qt.formatDateTime(date, "dd MMM yy");
}

function typingUsers(context, dialog)
{
    if(!isChat(dialog))
        return qsTr("Typing...");

    if(dialog.typingUsers.length === 1) {
        var user = context.telegram.user(dialog.typingUsers[0]);
        return qsTr("%1 is typing...").arg(userDisplayName(user));
    }
    else if(dialog.typingUsers.length === 2) {
        var user1 = context.telegram.user(dialog.typingUsers[0]);
        var user2 = context.telegram.user(dialog.typingUsers[1]);
        return qsTr("%1 and %2 are typing...").arg(userDisplayName(user1)).arg(userDisplayName(user2));
    }
    else if(dialog.typingUsers.length > 2)
        return qsTr("%1 members are typing...").arg(dialog.typingUsers.length);

    return "";
}

function isForwardedMessage(message)
{
    if(!message)
        return false;

    return message.fwdFromId !== 0;
}

function isServiceMessage(message)
{
    if(!message)
        return false;

    if(message.classType === TelegramConstants.typeMessageService)
        return true;

    if(message.action && (message.action.classType !== TelegramConstants.typeMessageActionEmpty))
        return true;

    return false;
}

function isMediaMessage(message)
{
    if(!message || !message.media)
        return false;

    if(message.media.classType === TelegramConstants.typeMessageMediaEmpty)
        return false;

    return true;
}

function isSticker(context, message)
{
    if(!isMediaMessage(message))
        return false;

    if(message.media.classType !== TelegramConstants.typeMessageMediaDocument)
        return false;

    if(context.telegram.documentIsSticker(message.media.document))
        return true;

    return false;
}

function encryptedUserId(context, encryptedchat)
{
    if(encryptedchat.adminId === context.telegram.me)
        return encryptedchat.participantId;

    return encryptedchat.adminId;
}

function peerId(dialog)
{
    if(!dialog.encrypted && (dialog.peer.classType === TelegramConstants.typePeerChat))
        return dialog.peer.chatId;

    return dialog.peer.userId;
}

function messageContent(context, message)
{
    if(!message)
        return "";

    if(message.media)
    {
        if(message.media.caption.length > 0)
            return message.media.caption;

        switch(message.media.classType)
        {
            case TelegramConstants.typeMessageMediaDocument: {
                if(context.telegram.documentIsSticker(message.media.document))
                    return qsTr("Sticker");

                return qsTr("Document");
            }

            case TelegramConstants.typeMessageMediaContact:
                return qsTr("Contact");

            case TelegramConstants.typeMessageMediaVideo:
                return qsTr("Video");

            case TelegramConstants.typeMessageMediaUnsupported:
                return qsTr("Unsupported");

            case TelegramConstants.typeMessageMediaAudio:
                return qsTr("Audio");

            case TelegramConstants.typeMessageMediaPhoto:
                return qsTr("Photo");

            case TelegramConstants.typeMessageMediaGeo:
            case TelegramConstants.typeMessageMediaVenue:
                return qsTr("Position");

            default:
                break;
        }
    }

    return message.message;
}

function firstMessageLine(context, message)
{
    var content = messageContent(context, message)
    return content.split("\n")[0];
}

function formatBytes(bytes, decimals)
{
   if(bytes === 0)
       return "0 Byte";

   var k = 1024;
   var dm = decimals + 1 || 3;
   var sizes = ["Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"];
   var i = Math.floor(Math.log(bytes) / Math.log(k));

   return (bytes / Math.pow(k, i)).toPrecision(dm) + " " + sizes[i];
}

function completePhoneNumber(phonenumber)
{
    if(phonenumber.length <= 0)
        return "";

    if(phonenumber[0] !== '+')
        return "+" + phonenumber;

    return phonenumber;
}

function isTelegramUser(user) // https://core.telegram.org/constructor/updateServiceNotification
{
    return user.id === 777000;
}
