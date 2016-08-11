.pragma library

.import Sailfish.Silica 1.0 as Silica
.import harbour.sailorgram.TelegramQml 2.0 as Telegram

function reverseColor(color, light) {
    if(light === undefined)
        light = 3.0;

    return Qt.lighter(Qt.rgba(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, 1.0), light);
}

function colorizeText(isactionmessage, messageout, context) {
    if(isactionmessage)
        return Silica.Theme.primaryColor;

    if(context.bubbleshidden) {
        if(isactionmessage)
            return Silica.Theme.primaryColor;

        if(messageout)
            return Silica.Theme.highlightColor;

        return Silica.Theme.primaryColor;
    }

    if(messageout)
        return Silica.Theme.primaryColor;

    if((context.bubblesopacity / 100.0) < 0.45)
        return Silica.Theme.highlightColor;

    return Silica.Theme.highlightDimmerColor;
}

function colorizeTick(message, context) {
    if(!message.sent)
        return "gray";

    if(context.bubbleshidden)
        return Silica.Theme.highlightColor;

    return Silica.Theme.highlightDimmerColor;
}

function colorizeBubble(messagetype, messageout, context) {
    if(context.bubbleshidden || (messagetype === Telegram.Enums.TypeActionMessage))
        return "transparent"

    if(messageout)
        return Silica.Theme.rgba(Qt.tint(Silica.Theme.secondaryHighlightColor, Silica.Theme.rgba(Silica.Theme.highlightDimmerColor, 0.3)), 1.0);

    return Silica.Theme.rgba(Silica.Theme.secondaryColor, 1.0);
}

function colorizeLink(messagetype, messageout, context, ignorebubble) {
    if(ignorebubble || context.bubbleshidden) {
        var linkcolor = Silica.Theme.rgba(Silica.Theme.highlightColor, 1.0);
        return reverseColor(linkcolor, 2.5);
    }

    var bubblecolor = Silica.Theme.rgba(colorizeBubble(messagetype, messageout, context), 1.0);
    return reverseColor(bubblecolor, 1.5);
}
