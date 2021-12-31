find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)

add_library(CV SIFTDescriptorStrategy.cpp SIFTDescriptorStrategy.h)
target_link_libraries(CV Qt5::Gui ${OpenCV_LIBS})