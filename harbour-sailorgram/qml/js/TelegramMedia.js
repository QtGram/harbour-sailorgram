.pragma library

.import Sailfish.Silica 1.0 as Silica
.import "TelegramConstants.js" as TelegramConstants

function defaultThumbnail(message)
{
    if(!message || !message.media)
        return "image://theme/icon-m-other";

    var mediatype = message.media.classType;

    if(mediatype === TelegramConstants.typeMessageMediaPhoto)
        return "image://theme/icon-m-image";

    if(mediatype === TelegramConstants.typeMessageMediaAudio)
        return "image://theme/icon-m-music";

    if(mediatype === TelegramConstants.typeMessageMediaVideo)
        return "image://theme/icon-m-video";

    if((mediatype === TelegramConstants.typeMessageMediaGeo) || (mediatype === TelegramConstants.typeMessageMediaVenue))
        return "image://theme/icon-m-location";

    if(mediatype === TelegramConstants.typeMessageMediaDocument)
    {
        var document = message.media.document;
        var attributes = document.attributes;

        for(var i = 0; i < attributes.length; i++)
        {
            var attribute = attributes[i].classType;

            if(attribute === TelegramConstants.typeDocumentAttributeSticker)
                return ""; // No default thumbnail for stickers

            if(attribute === TelegramConstants.typeDocumentAttributeVideo)
                return "image://theme/icon-m-video";

            if(attribute === TelegramConstants.typeDocumentAttributeAudio)
                return "image://theme/icon-m-music";

            if(attribute === TelegramConstants.typeDocumentAttributeImageSize)
                return "image://theme/icon-m-image";
        }

        return "image://theme/icon-m-document";
    }

    return "image://theme/icon-m-other";
}
