import QtQuick 2.1

Image
{
    id: messagethumbnail
    asynchronous: true
    visible: (status === Image.Ready)
    fillMode: Image.PreserveAspectFit
}
