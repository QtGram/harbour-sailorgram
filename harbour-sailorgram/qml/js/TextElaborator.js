.pragma library

function surrogatePairToUnicodeScalar(hi, lo)
{
    if (hi >= 0xD800 && hi <= 0xDBFF && lo >= 0xDC00 && lo <= 0xDFFF)
        return ((hi - 0xD800) * 0x400) + (lo - 0xDC00) + 0x10000;
    else
        console.log("Invalid surrogate pair");

    return 0;
}

function emojify(s, emojipath)
{
    if(!emojipath.length)
    {
        console.log("Invalid emoji path");
        return;
    }

    var ranges = [ "\ud83c[\udf00-\udfff]",    // U+1F300 to U+1F3FF
                   "\ud83d[\udc00-\ude4f]",    // U+1F400 to U+1F64F
                   "\ud83d[\ude80-\udeff]" ];  // U+1F680 to U+1F6FF

    return s.replace(new RegExp(ranges.join("|"), "g"),
                     function(match) {
                         var uniscalar = surrogatePairToUnicodeScalar(match.charCodeAt(0), match.charCodeAt(1));
                         return "<img src=\"" + emojipath + uniscalar.toString(16).toLowerCase() + ".png\">";
                     });
}

function linkify(s)
{
    //URLs starting with http://, https://, or ftp://
    var rgxhttpurl = /(\b(https?|ftp):\/\/[-A-Z0-9+&@#\/%?=~_|!:,.;]*[-A-Z0-9+&@#\/%=~_|])/gim;
    s = s.replace(rgxhttpurl, '<a href="$1" target="_blank">$1</a>');

    //URLs starting with "www." (without // before it, or it'd re-link the ones done above).
    var rgxwwwurl = /(^|[^\/])(www\.[\S]+(\b|$))/gim;
    s = s.replace(rgxwwwurl, '$1<a href="http://$2" target="_blank">$2</a>');

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

function replaceLtGt(s)
{
    return s.replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

function replaceNewlines(s)
{
    return s.replace(/(?:\r\n|\r|\n)/g, "<br/>");
}

function elaborate(s, emojipath, highlightcolor)
{
    var res = replaceLtGt(s);
    res = replaceNewlines(res);
    res = emojify(res, emojipath);
    res = linkify(res);
    res = mentionify(res, highlightcolor);
    return "<span>" + res + "</span>";
}
