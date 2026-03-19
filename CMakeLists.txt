cmake_minimum_required(VERSION 3.19)
project(UnvisualKode LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

qt_add_executable(UnvisualKode
    WIN32 MACOSX_BUNDLE
    main.cpp
    unvisualkode.cpp
    unvisualkode.h
    unvisualkode.ui
    aboutwindow.cpp
    aboutwindow.h
    aboutwindow.ui
)

target_link_libraries(UnvisualKode
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS UnvisualKode
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET UnvisualKode
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
