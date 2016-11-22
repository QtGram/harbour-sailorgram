import QtQuick 2.1
import Sailfish.Silica 1.0

GlassItem
{
    id: connectionstatus

    color: {
        if(context.telegram.syncing)
            return "yellow";

        if(context.telegram.connected)
            return "lime";

        return "red";
    }
}
