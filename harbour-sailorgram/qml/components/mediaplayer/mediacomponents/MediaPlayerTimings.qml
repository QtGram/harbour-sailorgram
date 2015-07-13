import QtQuick 2.1

QtObject
{
    id: mediaplayertimings

    function pad(num, size)
    {
        var s = num + "";

        while(s.length < size)
            s = "0" + s;

        return s;
    }

    function displayDuration(duration)
    {
        var numdays = Math.floor(duration / 86400);
        var numhours = Math.floor((duration % 86400) / 3600);
        var numminutes = Math.floor(((duration % 86400) % 3600) / 60);
        var numseconds = ((duration % 86400) % 3600) % 60;

        var videoduration = "";

        if(numdays > 0)
            videoduration += pad(numdays, 2) + ":";

        if(numhours > 0)
            videoduration += pad(numhours, 2) + ":";

        videoduration += pad(numminutes, 2) + ":";
        videoduration += pad(numseconds, 2);

        return videoduration;
    }
}
