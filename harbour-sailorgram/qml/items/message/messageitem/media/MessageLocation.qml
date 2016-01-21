import QtQuick 2.1

MessageMediaItem
{
    readonly property string appId: "MqR7KyY6dZpTbKiFwc3h"
    readonly property string appCode: "zfYp6V9Ou_wDQn4NVqMofA"

    function locationThumbnail(latitude, longitude)
    {
        return "https://maps.nlp.nokia.com/mia/1.6/mapview?" + "app_id=" + appId + "&"
                                                             + "app_code=" + appCode + "&"
                                                             + "ord&f=0&poithm=1&poilbl=0&"
                                                             + "ctr=" + latitude + "," + longitude + "&"
                                                             + "w=" + width + "&h=" + height + "&z=1";
    }

    id: messagelocation
    contentWidth: maxWidth
    contentHeight: maxWidth

    Image
    {
        id: mapthumbnail
        asynchronous: true
        cache: true
        anchors.fill: parent
        source: locationThumbnail(message.media.geo.lat, message.media.geo.longitude);
    }
}
