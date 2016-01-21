import QtQuick 2.1

Item
{
    property Item item: {
        if(messagecontainer.children.length <= 0)
            return null;

        return messagecontainer.children[0];
    }

    id: messagecontainer
    width: item ? item.width : 0;
    height: item ? item.height : 0;
}

