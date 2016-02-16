import QtQuick 2.1
import Sailfish.Silica 1.0

Label
{
    readonly property real calculatedWidth: dummylabel.contentWidth
    readonly property real calculatedHeight: dummylabel.contentHeight * resizablelabel.lineCount

    Text
    {
        id: dummylabel
        visible: false
        wrapMode: resizablelabel.wrapMode
        maximumLineCount: resizablelabel.maximumLineCount
        textFormat: resizablelabel.textFormat
        text: resizablelabel.text

        font {
            bold: resizablelabel.font.bold
            italic: resizablelabel.font.italic
            family: resizablelabel.font.family
            pixelSize: resizablelabel.font.pixelSize
        }
    }

    id: resizablelabel
}

