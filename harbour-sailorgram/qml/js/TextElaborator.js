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

function replaceNewlines(s)
{
    return s.replace(/(?:\r\n|\r|\n)/g, "<br/>");
}

function elaborate(s, emojipath)
{
    var res = replaceNewlines(s);
    res = emojify(res, emojipath);
    return res;
}
