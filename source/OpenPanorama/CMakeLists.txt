set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Qml Quick Gui REQUIRED)

set(SOURCE_FILES 
    gui.qrc
    OpenPanorama.cpp
)

add_executable(OpenPanorama ${SOURCE_FILES})

target_link_libraries(OpenPanorama Qt5::Core Qt5::Qml Qt5::Quick Qt5::Gui)