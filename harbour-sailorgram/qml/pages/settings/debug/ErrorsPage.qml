import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../models"
import "../../../components"

Page
{
    property Context context

    id: errorspage
    allowedOrientations: defaultAllowedOrientations

    SilicaListView
    {
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Clear errors")

                onClicked: {
                    remorsepopup.execute(qsTr("Deleting errors"), function () {
                        context.errors.clear();
                    });
                }
            }
        }

        ViewPlaceholder
        {
            anchors.fill: parent
            enabled: !context.errors.count
            text: qsTr("Error log is empty")
        }

        RemorsePopup { id: remorsepopup }

        header: PageHeader { title: qsTr("Error log") }
        model: context.errors

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: content.height + Theme.paddingLarge

            Column {
                id: content
                width: parent.width

                DebugLabel {
                    title: qsTr("Error code:")
                    text: errorcode
                }

                DebugLabel {
                    title: qsTr("Function:")
                    text: functionname
                }

                Row
                {
                    x: Theme.paddingMedium
                    width: parent.width - (Theme.paddingMedium * 2)
                    spacing: Theme.paddingSmall

                    Label {
                        id: lbltitle
                        color: Theme.highlightColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                        verticalAlignment: Text.AlignTop
                        wrapMode: Text.NoWrap
                        elide: Text.ElideRight
                        text: qsTr("Message:")
                    }

                    Label {
                        text: errortext
                        width: parent.width - lbltitle.contentWidth
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }
                }
            }
        }
    }
}
