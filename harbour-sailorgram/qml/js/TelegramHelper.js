.pragma library

.import harbour.sailorgram.Telegram 1.0 as Telegram
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

function isMediaMessage(message) {
    if(!message || !message.media)
        return false;

    if(message.media.classType === TelegramConstants.typeMessageMediaEmpty)
        return false;

    return true;
}

function peerId(dialog) {
    if(!dialog.encrypted && (dialog.peer.classType === TelegramConstants.typePeerChat))
        return dialog.peer.chatId;

    return dialog.peer.userId;
}

function mediaType(messagetype) {
    if(messagetype === Telegram.SailorgramEnums.MessageTypeAudio)
        return qsTr("Audio recording");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypeContact)
        return qsTr("Contact");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypeDocument)
        return qsTr("Document");
    else if((messagetype === Telegram.SailorgramEnums.MessageTypeGeo) || (messagetype === Telegram.SailorgramEnums.MessageTypeVenue))
        return qsTr("Position");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypePhoto)
        return qsTr("Photo");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypeSticker)
        return qsTr("Sticker");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypeUnsupported)
        return qsTr("Unsupported media");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypeVideo)
        return qsTr("Video");
    else if(messagetype === Telegram.SailorgramEnums.MessageTypeAnimated)
        return qsTr("GIF");

    return qsTr("Unknown media: %1").arg(messagetype);
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
