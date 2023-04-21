cmake_minimum_required(VERSION 3.16)

project(Test_Language_Parser VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")

add_executable( ServerLang_Prototype
    src/main.cpp
)

target_include_directories( ServerLang_Prototype PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties( ServerLang_Prototype PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER dev.naisys.net
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE FALSE
    WIN32_EXECUTABLE FALSE # If you want to enable std::cout in Windows :^)
)

configure_file(test/sample.nsl ${CMAKE_BINARY_DIR}/sample.nsl)

install( TARGETS ServerLang_Prototype
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
