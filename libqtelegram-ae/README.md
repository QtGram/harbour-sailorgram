# libqtelegram-aseman-edition
It's a fork of libqtelegram by Aseman Team which is porting to windows and mac alongside linux support. It's also build using qmake instead of cmake.

### How to Compile
#### Install dependencies

Install gcc, g++, openssl, git, Qt5Core, Qt5DBus, Qt5Gui, Qt5Multimedia, Qt5MultimediaQuick_p, Qt5Network, Qt5PrintSupport, Qt5Qml, Qt5Quick, Qt5Sql, Qt5Svg, and Qt5Widgets.
on Ubuntu:

    sudo apt-get install g++ gcc git qtbase5-dev libqt5sql5-sqlite libqt5multimediaquick-p5 libqt5multimedia5-plugins libqt5multimedia5 libqt5qml5 libqt5qml-graphicaleffects libqt5qml-quickcontrols qtdeclarative5-dev libqt5quick5 

on Fedora (tested on Fedora 20):

    yum install qt5-qtbase qt5-qtbase-devel qt5-qtdeclarative qt5-qtquick1 qt5-qtquick1-devel kde-plasma-applicationname kde-plasma-nm qt5-qtdeclarative-devel qt5-qtdeclarative-static qt5-qtgraphicaleffects qt5-qtquickcontrols openssl-devel libappindicator-devel

For other distributions search for the corresponding packages.

#### Available qmake keywords
    
There are some available keywords, you can use it as qmake flags on build step on each project:

    OPENSSL_LIB_DIR
    OPENSSL_INCLUDE_PATH
    LIBQTELEGRAM_LIB_DIR
    LIBQTELEGRAM_INCLUDE_PATH
    TELEGRAMQML_LIB_DIR
    TELEGRAMQML_INCLUDE_PATH

#### Get libqtelegram

Get libqtelegram using below command

    git clone https://github.com/Aseman-Land/libqtelegram-aseman-edition.git
    
And:

    cd libqtelegram-aseman-edition
    mkdir build && cd build
    qmake -r PREFIX=/usr  ..
    
And then start building:

    make
    sudo make install
