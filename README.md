qt-creator-coolscroll-plugin
============================

Code minimap as your Qt Creator scroll bar. A tweaked version based on the pretty snappy CoolScroll
by Egor Zhuk. The outlook has been modified to resemble the Productivity Power Tools minimap of
Visual Studio 2010.

Compiling and Installation
============================

Just generate build directory and relevant makefiles with QMake. Customize
input variables QTC_SOURCE and LIBSROOT for your particular setup.

    cd <path-to-coolscroll-sources>
    mkdir build
    cd build
    qmake -makefile QTC_SOURCE=<path-to-qt-creator-src> LIBSROOT=<path-to-qt-creator-libs-dir> ../
    make

The default project settings will generate the plugin under Qt Creator's user profile plugin directory.
