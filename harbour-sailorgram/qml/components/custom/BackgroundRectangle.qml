import QtQuick 2.1
import Sailfish.Silica 1.0

Rectangle
{
    id: backgroundrectangle

    gradient: Gradient {
        GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.15) }
        GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.3) }
    }
}
