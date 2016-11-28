import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"
import "../../js/Settings.js" as Settings

Dialog
{
    property Context context

    id: chatsettingspage
    acceptDestinationAction: PageStackAction.Pop

    onAccepted: {
        context.sendwithreturn = tssendwithreturn.checked;
        context.chatheaderhidden = !tsshowchatheader.checked;
        context.defaultemojiset = tsdefaultemojiset.checked;
        context.autoloadimages = tsautoloadimages.checked;
        context.bubbleshidden = !tsbubblesvisible.checked;
        context.angledbubbles = tsangledbubbles.checked;
        context.bubblesopacity = slbubblesopacity.value;

        Settings.set("sendwithreturn", tssendwithreturn.checked);
        Settings.set("chatheaderhidden", !tsshowchatheader.checked);
        Settings.set("defaultemojiset", tsdefaultemojiset.checked);
        Settings.set("autoloadimages", tsautoloadimages.checked);
        Settings.set("hidebubbles", !tsbubblesvisible.checked);
        Settings.set("angledbubbles", tsangledbubbles.checked);
        Settings.set("bubblesopacity", slbubblesopacity.value);
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            DialogHeader { acceptText: qsTr("Save") }
            SectionHeader { text: qsTr("Appearance") }

            TextSwitch
            {
                id: tsshowchatheader
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Chat header visible")
                checked: !context.chatheaderhidden
            }

            TextSwitch
            {
                id: tsdefaultemojiset
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Use default emoji set")
                checked: context.defaultemojiset
            }

            TextSwitch
            {
                id: tsautoloadimages
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Load images automatically")
                checked: context.autoloadimages
            }

            TextSwitch
            {
                id: tsbubblesvisible
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Bubbles visible")
                checked: !context.bubbleshidden
            }

            TextSwitch
            {
                id: tsangledbubbles
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Bubbles with angle")
                visible: tsbubblesvisible.checked
                checked: context.angledbubbles
            }

            Slider
            {
                id: slbubblesopacity
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                visible: tsbubblesvisible.checked
                label: qsTr("Bubble opacity")
                minimumValue: 1
                maximumValue: 100
                stepSize: 1
                value: context.bubblesopacity
                valueText: value + "%"
            }

            SectionHeader { text: qsTr("Keyboard") }

            TextSwitch
            {
                id: tssendwithreturn
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Send Messages by pressing Return Key")
                checked: context.sendwithreturn
            }
        }
    }
}
