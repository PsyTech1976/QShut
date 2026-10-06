cmake_minimum_required(VERSION 3.16)
project(qshutdown VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets DBus)

set(SOURCES
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/ShutdownManager.h
    src/ShutdownManager.cpp
    src/LanguageManager.h
    src/LanguageManager.cpp
    src/HelpDialog.h
    src/HelpDialog.cpp
    resources/resources.qrc
)

add_executable(qshutdown ${SOURCES})

target_link_libraries(qshutdown PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
)

install(TARGETS qshutdown RUNTIME DESTINATION bin)
install(FILES resources/qshutdown.desktop DESTINATION share/applications)
install(FILES resources/qshutdown.svg DESTINATION share/icons/hicolor/scalable/apps)
