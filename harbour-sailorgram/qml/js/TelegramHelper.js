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

    var MS_PER_DAY = 1000 * 60 * 60 * 24;
    var daydiff = (now - date) / MS_PER_DAY;

    if(daydiff < 7)
        return Qt.formatDateTime(date, "ddd HH:mm");
    else if(date.getYear() === now.getYear())
        return Qt.formatDateTime(date, "dd MMM");

    return Qt.formatDateTime(date, "dd MMM yy");
}

function isServiceMessage(message)
{
    if(message.classType === TelegramConstants.typeMessageService)
        return true;

    if(message.action && (message.action.classType !== TelegramConstants.typeMessageActionEmpty))
        return true;

    return false;
}

function isMediaMessage(message)
{
    if(!message.media)
        return false;

    if(message.media.classType === TelegramConstants.typeMessageMediaEmpty)
        return false;

    return true;
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

function firstMessageLine(message)
{
    var content = messageContent(message)
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
    if(phonenumber[0] !== '+')
        return "+" + phonenumber;

    return phonenumber;
}

function isTelegramUser(user) // https://core.telegram.org/constructor/updateServiceNotification
{
    return user.id === 777000;
}
