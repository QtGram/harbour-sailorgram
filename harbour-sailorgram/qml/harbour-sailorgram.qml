import QtQuick 2.1
import Sailfish.Silica 1.0
import "pages"
import "cover"
import "model"

ApplicationWindow
{
    property Context context: Context { }

    id: mainwindow

    cover: CoverPage {
        context: mainwindow.context
    }

    initialPage: Component {
        MainPage {
            context: mainwindow.context
        }
    }
}

