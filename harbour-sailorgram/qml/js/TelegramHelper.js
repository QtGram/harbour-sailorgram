.pragma library

.import harbour.sailorgram.TelegramCalendar 1.0 as TelegramCalendar

var typeUserStatusOffline = 0x8c703f;
var typeUserStatusEmpty = 0x9d05049;
var typeUserStatusOnline = 0xedb93949;
var typeUserStatusRecently = 0xe26f42f1;
var typeUserStatusLastWeek = 0x7bf09fc;
var typeUserStatusLastMonth = 0x77ebc742;

function userName(user)
{
    return ((user.username.length > 0) ? user.username : (user.firstName + " " + user.lastName));
}

function phoneNumber(user)
{
    return completePhoneNumber(user.phone);
}

function userStatus(user)
{
    switch(user.status.classType)
    {
        case typeUserStatusRecently:
            return qsTr("Recently");

        case typeUserStatusLastMonth:
            return qsTr("Last Month");

        case typeUserStatusLastWeek:
            return qsTr("Last Week");

        case typeUserStatusOnline:
            return qsTr("Online");

        case typeUserStatusOffline:
            return qsTr("Last Seen at %1").arg(TelegramCalendar.TelegramCalendar.timeToString(user.status.wasOnline));

        default:
            break;
    }

    return qsTr("Unknown");
}

function formatBytes(bytes, decimals)
{
   if(bytes === 0)
       return "0 Byte";

   var k = 1024;
   var dm = decimals + 1 || 3;
   var sizes = ["Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"];
   var i = Math.floor(Math.log(bytes) / Math.log(k));

   return (bytes / Math.pow(k, i)).toPrecision(dm) + " " + sizes[i];
}

function completePhoneNumber(phonenumber)
{
    if(phonenumber[0] !== '+')
        return "+" + phonenumber;

    return phonenumber;
}
