# TarotClub project file
#
# TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
# Copyright (C) 2003-2999 - Anthony Rabine
# anthony@tarotclub.fr
#
# TarotClub is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TarotClub is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
#

# ------------------------------------------------------------------------------
# Directories for generated files
# ------------------------------------------------------------------------------
UI_DIR = ./ui
UI_HEADERS_DIR = ./include
UI_SOURCES_DIR = ./src
OBJECTS_DIR = ./obj
DESTDIR = ./bin

# ------------------------------------------------------------------------------
# The search path to find supplied files
# ------------------------------------------------------------------------------
VPATH += $${PWD}/../src
VPATH += $${PWD}/../src/library
VPATH += $${PWD}/../src/desktop
VPATH += $${PWD}/../src/server
VPATH += $${PWD}/../src/jsengine
VPATH += $${PWD}/../src/json
VPATH += $${PWD}/../lib
VPATH += $${PWD}/../ai
VPATH += $${PWD}/../ai/tarotlib

# ------------------------------------------------------------------------------
# Where to find header files
# ------------------------------------------------------------------------------
INCLUDEPATH += $${PWD}/../src
INCLUDEPATH += $${PWD}/../src/library
INCLUDEPATH += $${PWD}/../src/desktop
INCLUDEPATH += $${PWD}/../src/server
INCLUDEPATH += $${PWD}/../src/jsengine
INCLUDEPATH += $${PWD}/../src/json

# ------------------------------------------------------------------------------
# Compiler definitions
# ------------------------------------------------------------------------------
QT += xml svg help qml
RESOURCES = $${PWD}/../src/data/data.qrc
CONFIG += qt warn_on
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS_DEBUG +=  -O0  -ggdb -pedantic -std=c99 -fstrict-aliasing

# Specific OS stuff
win32 {
    RC_FILE = icon.rc
    LIBS +=  libws2_32
    DEFINES += USE_WINDOWS_OS
}
unix {
    DEFINES += USE_UNIX_OS
}

debug {
    DEFINES += TAROT_DEBUG
    DEFINES += DUK_OPT_DEBUG
}

# ------------------------------------------------------------------------------
# JavaScript files
# ------------------------------------------------------------------------------
OTHER_FILES = beginner.js \
              system.js \
              card.js \
              util.js \
              deck.js \
              game.js \
              player.js \
              unit_test.js

# ------------------------------------------------------------------------------
# Translation files
# ------------------------------------------------------------------------------
TRANSLATIONS = tarotclub_fr.ts \
               tarotclub_en.ts

# ------------------------------------------------------------------------------
# Library files
# ------------------------------------------------------------------------------
HEADERS += Log.h \
    Observer.h \
    Util.h \
    ThreadQueue.h \
    ByteStreamReader.h \
    ByteStreamWriter.h \
    ByteArray.h \
    TcpSocket.h \
    TcpServer.h \
    TcpClient.h \
    UniqueId.h \
    Common.h

SOURCES += Log.cpp \
    Util.cpp \
    ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp \
    TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    UniqueId.cpp \
    Common.cpp

# ------------------------------------------------------------------------------
# JSEngine and JSON files
# ------------------------------------------------------------------------------

HEADERS += duktape.h \
    JsonWriter.h \
    JsonReader.h \
    JSEngine.h

SOURCES += duktape.c \
    JsonWriter.cpp \
    JsonReader.cpp \
    JSEngine.cpp

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
HEADERS += ServerConfig.h \
    DealFile.h \
    TarotDeck.h \
    Deck.h \
    Card.h \
    Player.h \
    Client.h \
    Bot.h \
    defines.h \
    TarotEngine.h \
    Deal.h \
    Identity.h \
    Game.h \
    Score.h \
    Protocol.h \
    Controller.h \
    Table.h

SOURCES += ServerConfig.cpp \
    DealFile.cpp \
    TarotDeck.cpp \
    Deck.cpp \
    Card.cpp \
    Player.cpp \
    Client.cpp \
    Bot.cpp \
    TarotEngine.cpp \
    Deal.cpp \
    Game.cpp \
    Protocol.cpp \
    Controller.cpp \
    Table.cpp \
    Score.cpp

# ------------------------------------------------------------------------------
# Desktop client files
# ------------------------------------------------------------------------------
HEADERS += AboutWindow.h \
    MainWindow.h \
    OptionsWindow.h \
    EditorWindow.h \
    ResultWindow.h \
    HelpWindow.h \
    ScoresDock.h \
    InfosDock.h \
    ChatDock.h \
    DebugDock.h \
    Canvas.h \
    TextBox.h \
    ClientConfig.h \
    JoinWizard.h \
    GfxCard.h \
    MenuItem.h \
    TarotClub.h \
    PlayerBox.h \
    CustomTypes.h \
    PopupItem.h

FORMS += AboutUI.ui \
    AvatarsUI.ui \
    InfosDockUI.ui \
    OptionsUI.ui \
    ResultUI.ui \
    HelpUI.ui \
    NumberedDealUI.ui \
    DealEditorUI.ui \
    WinUI.ui \
    JoinWizardPage1.ui \
    JoinWizardPage2.ui \
    QuickJoin.ui

SOURCES += AboutWindow.cpp \
    MainWindow.cpp \
    OptionsWindow.cpp \
    EditorWindow.cpp \
    ResultWindow.cpp \
    HelpWindow.cpp \
    ScoresDock.cpp \
    InfosDock.cpp \
    ChatDock.cpp \
    DebugDock.cpp \
    Canvas.cpp \
    TextBox.cpp \
    ClientConfig.cpp \
    main.cpp \
    TarotClub.cpp \
    JoinWizard.cpp \
    GfxCard.cpp \
    MenuItem.cpp \
    PlayerBox.cpp \
    PopupItem.cpp

# End of project file
