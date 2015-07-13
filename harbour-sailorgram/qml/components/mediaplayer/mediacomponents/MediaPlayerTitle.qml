import QtQuick 2.1
import QtMultimedia 5.0
import Sailfish.Silica 1.0

Label
{
    function keepVisible(keep)
    {
        if(!keep)
        {
            timerdissolve.restart();
            return;
        }

        timerdissolve.stop()
        lblmptitle.opacity = 1.0;
    }

    function restoreOpacity()
    {
        lblmptitle.opacity = 1.0;
        timerdissolve.restart();
    }

    id: lblmptitle
    visible: opacity > 0.0 && (Qt.application.state === Qt.ApplicationActive)
    elide: Text.ElideRight
    verticalAlignment: Text.AlignHCenter
    horizontalAlignment: Text.AlignRight
    z: 10

    Behavior on opacity { NumberAnimation { duration: 800; easing.type: Easing.Linear } }

    Timer
    {
        id: timerdissolve
        interval: 2000

        onTriggered: {
            lblmptitle.opacity = 0.0;
        }
    }
}
