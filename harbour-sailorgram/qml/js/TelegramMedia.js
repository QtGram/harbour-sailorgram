.pragma library

.import Sailfish.Silica 1.0 as Silica
.import "TelegramConstants.js" as TelegramConstants
.import "ColorScheme.js" as ColorScheme

function defaultThumbnail(message, context)
{
    var color = context ? ColorScheme.colorizeText(message, context) : Silica.Theme.primaryColor;

    if(!message || !message.media)
        return "image://theme/icon-m-other?" + color;

    var mediatype = message.media.classType;

    if(mediatype === TelegramConstants.typeMessageMediaPhoto)
        return "image://theme/icon-m-image?" + color;

    if(mediatype === TelegramConstants.typeMessageMediaAudio)
        return "image://theme/icon-m-music?" + color;

    if(mediatype === TelegramConstants.typeMessageMediaVideo)
        return "image://theme/icon-m-video?" + color;

    if((mediatype === TelegramConstants.typeMessageMediaGeo) || (mediatype === TelegramConstants.typeMessageMediaVenue))
        return "image://theme/icon-m-location?" + color;

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
                return "image://theme/icon-m-video?" + color;

            if(attribute === TelegramConstants.typeDocumentAttributeAudio)
                return "image://theme/icon-m-music?" + color;

            if(attribute === TelegramConstants.typeDocumentAttributeImageSize)
                return "image://theme/icon-m-image?" + color;
        }

        return "image://theme/icon-m-document?" + color;
    }

    return "image://theme/icon-m-other?" + color;
}
