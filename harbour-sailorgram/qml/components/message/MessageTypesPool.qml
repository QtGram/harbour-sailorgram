import QtQuick 2.1
import "../../items/message/messageitem/media"
import "../../items/message/messageitem"

Item
{
    property Component documentComponent: Component { MessageDocument { } }
    property Component photoComponent: Component { MessagePhoto { } }
    property Component audioComponent: Component { MessageAudio { } }
    property Component videoComponent: Component { MessageVideo { } }
    property Component locationComponent: Component {  MessageLocation { } }
    property Component webpageComponent: Component {  MessageWebPage { } }
    property Component stickerComponent: Component { MessageSticker { } }
    property Component contactComponent: Component { MessageContact { } }

    property Component messagePreview: Component { MessagePreview { } }

    id: messagetypespool
}

