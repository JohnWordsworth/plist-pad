#-------------------------------------------------
#
# Project created by QtCreator 2013-07-13T17:32:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlistPad
TEMPLATE = app


SOURCES += src/main.cpp\
    src/dialogs/AboutDialog.cpp \
    src/dialogs/FindReplaceDialog.cpp \
    src/ComboBoxDelegate.cpp \
    src/MainWindow.cpp \
    src/model/PlistTreeWriter.cpp \
    src/model/PlistTreeItem.cpp \
    src/model/PlistTreeModel.cpp \
    src/model/PlistTreeReader.cpp

HEADERS  += \
    src/dialogs/AboutDialog.h \
    src/dialogs/FindReplaceDialog.h \
    src/ComboBoxDelegate.h \
    src/MainWindow.h \
    src/model/PlistTreeModel.h \
    src/model/PlistTreeItem.h \
    src/model/PlistTreeReader.h \
    src/model/PlistTreeWriter.h

FORMS    += \
    src/dialogs/AboutDialog.ui \
    src/dialogs/FindReplaceDialog.ui \
    src/MainWindow.ui

OTHER_FILES +=
    res/icon.ico

RESOURCES += \
    res/resources.qrc

#------------------------
# Icon File Definitions
#------------------------

ICON = res/icon.icns
RC_FILE = res/icon.rc


#------------------------
# Copy Windows DLLs
#------------------------

CONFIG(debug, debug|release) {
    CURBUILD = debug
} else {
    CURBUILD = release
}

DESTDIR = $${OUT_PWD}/$${CURBUILD}


# "Method" (Actually a build test) to copy a DLL from the Qt folder to the DESTDIR
defineTest(copyToDestDir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$DESTDIR

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY \"$$quote($$FILE)\" \"$$quote($$DDIR)\" $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

# When targetting windows and in Release mode, copy DLLs to output directory.
win32:CONFIG(release, debug|release) {
    copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\Qt5Core.dll);
    copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\Qt5Gui.dll);
    copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\Qt5Widgets.dll);
    copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\icudt51.dll);
    copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\icuin51.dll);
    copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\icuuc51.dll);

    # Needed for ANGLE build
    #copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\libEGL.dll);
    #copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\libGLESv2.dll);
    #copyToDestDir($$[QT_INSTALL_LIBS]\\..\\bin\\d3dcompiler_46.dll);

    QMAKE_POST_LINK += mkdir \"$$DESTDIR\\platforms\\\" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$QMAKE_COPY \"$$[QT_INSTALL_LIBS]\\..\\plugins\\platforms\\qminimal.dll\" \"$$DESTDIR\\platforms\\\" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$QMAKE_COPY \"$$[QT_INSTALL_LIBS]\\..\\plugins\\platforms\\qwindows.dll\" \"$$DESTDIR\\platforms\\\" $$escape_expand(\\n\\t)
}
