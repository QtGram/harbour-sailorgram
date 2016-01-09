.pragma library

.import "Emoji.js" as Emoji

function linkify(s)
{
    //URLs starting with http://, https://, or ftp://
    var rgxhttpurl = /(\b(https?|ftp):\/\/[-A-Z0-9+&@#\/%?=~_|!:,.;]*[-A-Z0-9+&@#\/%=~_|])/gim;
    s = s.replace(rgxhttpurl, '<a href="$1" target="_blank">$1</a>');

    //URLs starting with "www." (without // before it, or it'd re-link the ones done above).
    var rgxwwwurl = /(^|[^\/])(www\.[\S]+(\b|$))/gim;
    s = s.replace(rgxwwwurl, '$1<a href="http://$2">$2</a>');

    //Change email addresses to mailto:: links.
    var rgxmailto = /(([a-zA-Z0-9\-\_\.])+@[a-zA-Z\_]+?(\.[a-zA-Z]{2,6})+)/gim;
    s = s.replace(rgxmailto, '<a href="mailto:$1">$1</a>');

    return s;
}

function mentionify(s, highlightcolor)
{
    return s.replace(/\B@[a-zA-Z0-9_]+/gi,
                     function(match) {
                         return "<font color=\"" + highlightcolor + "\">" + match + "</font>";
                     });
}

function mentionifyBold(s)
{
    return s.replace(/\B@[a-zA-Z0-9_]+/gi,
                     function(match) {
                         return "<b>" + match + "</b>";
                     });
}

function replaceLtGt(s)
{
    return s.replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

function replaceNewlines(s)
{
    return s.replace(/(?:\r\n|\r|\n)/g, "<br/>");
}

function elaborate(s, emojipath, height, highlightcolor)
{
    var res = replaceLtGt(s);
    res = replaceNewlines(res);
    res = Emoji.emojify(res, height, emojipath, false);
    res = linkify(res);
    res = mentionify(res, highlightcolor);
    return "<span>" + res + "</span>";
}

function elaborateNotify(s, emojipath, height)
{
    return s; // NOTE: Lipstick doesn't support markup!
    //var res = Emoji.emojify(s, height, emojipath, true);
    //res = linkify(res);
    //res = mentionifyBold(res);
    //return res;
}
