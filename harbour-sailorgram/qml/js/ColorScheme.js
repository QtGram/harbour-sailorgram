.pragma library

.import Sailfish.Silica 1.0 as Silica
.import "TelegramHelper.js" as TelegramHelper

function colorize(message, context)
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
        return Silica.Theme.secondaryColor;

    return Silica.Theme.rgba(Qt.tint(Silica.Theme.secondaryHighlightColor, Silica.Theme.rgba(Silica.Theme.highlightDimmerColor, 0.3)), 0.7);
}
