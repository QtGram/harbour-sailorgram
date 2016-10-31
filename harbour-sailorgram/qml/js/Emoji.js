.pragma library

function surrogatePairToCodePoint(hi, lo)
{
    if (hi >= 0xD800 && hi <= 0xDBFF && lo >= 0xDC00 && lo <= 0xDFFF)
        return ((hi - 0xD800) * 0x400) + (lo - 0xDC00) + 0x10000;
    else
        console.log("Invalid surrogate pair");

    return 0;
}

function emojiName(emojipath, cp1, cp2)
{
    var name = emojipath + cp1.toString(16).toLowerCase();

    if(cp2)
        name += "-" + cp2.toString(16).toLowerCase();

    return name + ".png";
}

function emojify(s, height, emojipath, notification)
{
    if(!emojipath.length)
    {
        console.log("Invalid emoji path");
        return;
    }

    var ranges = [ "\ud83c[\udf00-\udfff](?:\ud83c[\udffb-\udfff])*",    // U+1F300 to U+1F3FF (with skin tone, if any)
                   "\ud83d[\udc00-\ude4f](?:\ud83c[\udffb-\udfff])*",    // U+1F400 to U+1F64F (with skin tone, if any)
                   "\ud83d[\ude80-\udeff](?:\ud83c[\udffb-\udfff])*" ];  // U+1F680 to U+1F6FF (with skin tone, if any)

    return s.replace(new RegExp(ranges.join("|"), "g"),
                     function(match) {
                         var cp1 = surrogatePairToCodePoint(match.charCodeAt(0), match.charCodeAt(1));
                         var cp2 = match.length > 2 ? surrogatePairToCodePoint(match.charCodeAt(2), match.charCodeAt(3)) : 0;
                         var emoji = emojiName(emojipath, cp1, cp2);

                         if(notification === true)
                             return "<img alt=\"" + match + "\" src=\"" + emoji + "\">";

                         return "<img alt=\"" + match + "\" align=\"middle\" width=\"" + height + "\" height=\"" + height + "\" src=\"" + emoji + "\">";
                     });
}
