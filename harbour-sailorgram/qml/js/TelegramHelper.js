.pragma library

.import harbour.sailorgram.TelegramCalendar 1.0 as TelegramCalendar
.import "TelegramConstants.js" as TelegramConstants

function completeName(user)
{
    return user.firstName + " " + user.lastName;
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
            return qsTr("Last Seen %1").arg(TelegramCalendar.TelegramCalendar.timeToString(user.status.wasOnline));

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

function isActionMessage(message) {
    if(message.classType === TelegramConstants.typeMessageService)
        return true;

    if(message.action && (message.action.classType !== TelegramConstants.typeMessageActionEmpty))
        return true;

    return false;
}

function peerId(dialog)
{
    if(dialog.peer.classType === TelegramConstants.typePeerChat)
        return dialog.peer.chatId;

    return dialog.peer.userId;
}

function messageContent(message)
{
    if(message.media)
    {
        switch(message.media.classType)
        {
            case TelegramConstants.typeMessageMediaDocument:
                return qsTr("Document");

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
                return qsTr("Geo");

            default:
                break;
        }
    }

    return message.message;
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
    if(phonenumber[0] !== '+')
        return "+" + phonenumber;

    return phonenumber;
}

function isTelegramUser(user) // https://core.telegram.org/constructor/updateServiceNotification
{
    return user.id === 777000;
}
