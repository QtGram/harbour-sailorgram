import QtQuick 2.1
import Sailfish.Silica 1.0
import "pages"
import "cover"

ApplicationWindow
{
    id: mainwindow
    cover: CoverPage { }

    initialPage: Component {
        MainPage { }
    }
}

