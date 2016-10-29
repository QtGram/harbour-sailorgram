.pragma library

.import Sailfish.Silica 1.0 as Silica

function reverseColor(color, light) {
    if(light === undefined)
        light = 3.0;

    return Qt.lighter(Qt.rgba(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, 1.0), light);
}

function colorizeBubble(ismessageservice, ismessageout, context) {
    if(context.bubbleshidden || ismessageservice)
        return "transparent";

    if(ismessageout)
        return Silica.Theme.rgba(Qt.tint(Silica.Theme.secondaryHighlightColor, Silica.Theme.rgba(Silica.Theme.highlightDimmerColor, 0.3)), 1.0);

    return Silica.Theme.rgba(Silica.Theme.secondaryColor, 1.0);
}

function colorizeText(ismessageservice, ismessageout, context) {
    if(ismessageservice)
        return Silica.Theme.primaryColor;

    if(context.bubbleshidden) {
        if(ismessageservice)
            return Silica.Theme.primaryColor;

        if(ismessageout)
            return Silica.Theme.highlightColor;

        return Silica.Theme.primaryColor;
    }

    if(ismessageout)
        return Silica.Theme.primaryColor;

    if((context.bubblesopacity / 100.0) < 0.45)
        return Silica.Theme.highlightColor;

    return Silica.Theme.highlightDimmerColor;
}

function colorizeLink(ismessageservice, ismessageout, context, ignorebubble) {
    if(ignorebubble || context.bubbleshidden) {
        var linkcolor = Silica.Theme.rgba(Silica.Theme.highlightColor, 1.0);
        return reverseColor(linkcolor, 2.5);
    }

    var bubblecolor = Silica.Theme.rgba(colorizeBubble(ismessageservice, ismessageout, context), 1.0);
    return reverseColor(bubblecolor, 1.5);
}
