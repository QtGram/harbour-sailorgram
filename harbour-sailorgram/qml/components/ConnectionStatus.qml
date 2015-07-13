import QtQuick 2.1
import Sailfish.Silica 1.0
import "../models"

GlassItem
{
    property Context context

    id: connectionstatus
    color: context.heartbeat.connected ? "lime" : "red"
}
