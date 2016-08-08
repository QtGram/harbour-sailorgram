.pragma library

.import harbour.sailorgram.TelegramCalendar 1.0 as TelegramCalendar
.import harbour.sailorgram.TelegramQml 2.0 as Telegram
.import "TelegramConstants.js" as TelegramConstants

function completeName(user) {
    var name = user.firstName;

    if(user.lastName.length > 0)
        name += " " + user.lastName;

    return name;
}

function userDisplayName(user) {
    if(user.firstName.length > 0)
        return user.firstName;

    if(user.lastName.length > 0)
        return user.lastName;

    return qsTr("Unknown");
}

function phoneNumber(user) {
    return completePhoneNumber(user.phone);
}

function fallbackText(title) {
    var words = title.split(" ");

    if(words.length >= 2)
        return words[0].slice(0, 1).toUpperCase() + words[1].slice(0, 1).toUpperCase();

    return words[0][0].toUpperCase();
}

function isChat(dialog) {
    if(!dialog)
        return false;

    if(dialog.encrypted)
        return false;

    return dialog.peer.classType === TelegramConstants.typePeerChat;
}

function printableDate(timestamp, full) {
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

function typingUsers(context, typing) {
    if(typing.length === 1)
        return qsTr("%1 is typing...").arg(userDisplayName(typing[0]));
    else if(typing.length === 2)
        return qsTr("%1 and %2 are typing...").arg(userDisplayName(typing[0])).arg(userDisplayName(typing[1]));
    else if(typing.length > 2)
        return qsTr("%n member(s) are typing...", "", typing.length);

    return "";
}

function webPageHasThumb(message) {
    if(!message)
        return false;

    if(message.media.classType !== TelegramConstants.typeMessageMediaWebPage)
        return false;

    return message.media.webpage.photo.classType !== TelegramConstants.typePhotoEmpty;
}

function isWebPage(message) {
    if(!message)
        return false;

    return message.media.classType === TelegramConstants.typeMessageMediaWebPage;
}

function isMediaMessage(message) {
    if(!message || !message.media)
        return false;

    if(message.media.classType === TelegramConstants.typeMessageMediaEmpty)
        return false;

    return true;
}

function isSticker(context, message) {
    if(!isMediaMessage(message))
        return false;

    if(message.media.classType !== TelegramConstants.typeMessageMediaDocument)
        return false;

    if(context.telegram.documentIsSticker(message.media.document))
        return true;

    return false;
}

function encryptedUserId(context, encryptedchat) {
    if(encryptedchat.adminId === context.telegram.me)
        return encryptedchat.participantId;

    return encryptedchat.adminId;
}

function peerId(dialog) {
    if(!dialog.encrypted && (dialog.peer.classType === TelegramConstants.typePeerChat))
        return dialog.peer.chatId;

    return dialog.peer.userId;
}

function isTextMessage(messagetype) {
    return (messagetype === Telegram.Enums.TypeTextMessage) || (messagetype === Telegram.Enums.TypeWebPageMessage);
}

function mediaType(messagetype) {
    if(messagetype === Telegram.Enums.TypeAudioMessage)
        return qsTr("Audio");
    else if(messagetype === Telegram.Enums.TypeContactMessage)
        return qsTr("Contact");
    else if(messagetype === Telegram.Enums.TypeDocumentMessage)
        return qsTr("Document");
    else if((messagetype === Telegram.Enums.TypeGeoMessage) || (messagetype === Telegram.Enums.TypeVenueMessage))
        return qsTr("Position");
    else if(messagetype === Telegram.Enums.TypePhotoMessage)
        return qsTr("Photo");
    else if(messagetype === Telegram.Enums.TypeStickerMessage)
        return qsTr("Sticker");
    else if(messagetype === Telegram.Enums.TypeUnsupportedMessage)
        return qsTr("Unsupported media");
    else if(messagetype === Telegram.Enums.TypeVideoMessage)
        return qsTr("Video");
    else if(messagetype === Telegram.Enums.TypeAnimatedMessage)
        return qsTr("GIF");

    return qsTr("Unknown media")
}

function firstMessageLine(context, message) {
    return message.split("\n")[0];
}

function formatBytes(bytes, decimals) {
   if(bytes === 0)
       return "0 Byte";

   var k = 1024;
   var dm = decimals + 1 || 3;
   var sizes = ["Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"];
   var i = Math.floor(Math.log(bytes) / Math.log(k));

   return (bytes / Math.pow(k, i)).toPrecision(dm) + " " + sizes[i];
}

function completePhoneNumber(phonenumber) {
    if(phonenumber.length <= 0)
        return "";

    if(phonenumber[0] !== '+')
        return "+" + phonenumber;

    return phonenumber;
}
