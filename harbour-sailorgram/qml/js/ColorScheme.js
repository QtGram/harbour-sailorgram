.pragma library

.import Sailfish.Silica 1.0 as Silica
.import "TelegramHelper.js" as TelegramHelper

function colorizeText(message, context)
{
    if(TelegramHelper.isServiceMessage(message))
        return Silica.Theme.primaryColor;

    if(context.bubbleshidden) {
        if(TelegramHelper.isServiceMessage(message))
            return Silica.Theme.primaryColor;

        if(message.out)
            return Silica.Theme.highlightColor;

        return Silica.Theme.primaryColor;
    }

    if(message.out)
        return Silica.Theme.highlightDimmerColor;

    return Silica.Theme.primaryColor;
}

function colorizeTick(message, context)
{
    if(!message.sent)
        return "gray";

    if(context.bubbleshidden)
        return Silica.Theme.highlightColor;

    return Silica.Theme.highlightDimmerColor;
}

function colorizeBubble(message, context)
{
    if(context.bubbleshidden || TelegramHelper.isServiceMessage(message))
        return "transparent"

    if(message.out)
        return Silica.Theme.rgba(Silica.Theme.secondaryColor, context.bubblesopacity / 100.0);

    return Silica.Theme.rgba(Qt.tint(Silica.Theme.secondaryHighlightColor, Silica.Theme.rgba(Silica.Theme.highlightDimmerColor, 0.3)), context.bubblesopacity / 100.0);
}

function colorizeLink(message, context)
{
    if(context.bubbleshidden)
    {
        var linkcolor = Silica.Theme.rgba(Silica.Theme.highlightColor, 1.0);
        return Qt.lighter(Qt.rgba(1.0 - linkcolor.r, 1.0 - linkcolor.g, 1.0 - linkcolor.b, 1.0), 2.5);
    }

    var bubblecolor = Silica.Theme.rgba(colorizeBubble(message,  context), 1.0);
    return Qt.lighter(Qt.rgba(1.0 - bubblecolor.r, 1.0 - bubblecolor.g, 1.0 - bubblecolor.b, 1.0), 1.5);
}
