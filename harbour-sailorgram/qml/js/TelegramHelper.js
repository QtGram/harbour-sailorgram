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
