import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../js/TextElaborator.js" as TextElaborator

Label
{
    property string emojiPath
    property string rawText

    id: messagetextcontent
    clip: true
    textFormat: Text.StyledText
    text: TextElaborator.elaborate(rawText, emojiPath)
}
