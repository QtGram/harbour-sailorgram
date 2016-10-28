import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../js/TextElaborator.js" as TextElaborator

Label
{
    property bool openUrls: true
    property string emojiPath
    property string rawText

    id: messagetext
    textFormat: Text.StyledText
    text: TextElaborator.elaborate(rawText, emojiPath, font.pixelSize, "red", openUrls)
    verticalAlignment: Text.AlignTop

    onLinkActivated: {
        if(!openUrls)
            return;

        Qt.openUrlExternally(link);
    }
}
