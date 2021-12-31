set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS QuickControls2 Core Qml Quick Gui REQUIRED)

set(SOURCE_FILES 
    gui.qrc
    OpenPanorama.cpp
    ImageProviderWrapper.cpp
    ImageProviderWrapper.hpp
    PanoramaImage.cpp
    PanoramaImage.hpp
    PanoramaImageModel.cpp
    PanoramaImageModel.hpp
    PanoramaPane.cpp
    PanoramaPane.hpp
    HeavyDutyWorker.cpp
    HeavyDutyWorker.h)

add_executable(OpenPanorama ${SOURCE_FILES})
target_link_libraries(OpenPanorama CV Qt5::QuickControls2 Qt5::Core Qt5::Qml Qt5::Quick Qt5::Gui)