import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"

PullDownMenu
{
    property Context context

    id: telegrampulldownmenu
    enabled: context.heartbeat.connected
}
