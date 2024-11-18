cmake_minimum_required(VERSION 3.16)
project(Watchlist VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Your existing output directory settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

# Your existing build type settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/Debug)
    set(AUTOGEN_OUTPUT_DIR ${OUTPUT_DIR}/include)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/Release)
    set(AUTOGEN_OUTPUT_DIR ${OUTPUT_DIR}/include)
endif()

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOGEN_BUILD_DIR ${AUTOGEN_BUILD_DIR})

# Find Qt packages
find_package(Qt6 COMPONENTS Core Gui Widgets Network Charts WebSockets Multimedia REQUIRED)

# Get Qt plugins directory for deployment
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake LOCATION)
execute_process(
    COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_PLUGINS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Define source files directory structure
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/h")
set(UI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/uif")

# Collect UI files
file(GLOB_RECURSE UI_FILES 
    "${UI_DIR}/*.ui"
)

# Tell CMake where to find the UI files
set(CMAKE_AUTOUIC_SEARCH_PATHS ${UI_DIR})

# Separate core source files (non-UI related)
file(GLOB_RECURSE CORE_SOURCE_FILES 
    "${SOURCE_DIR}/core/*.cpp"
)

file(GLOB_RECURSE CORE_HEADER_FILES 
    "${HEADER_DIR}/core/*.h"
)

# Separate GUI source files
file(GLOB_RECURSE GUI_SOURCE_FILES 
    "${SOURCE_DIR}/ui/*.cpp"
    "${SOURCE_DIR}/main.cpp"
)

file(GLOB_RECURSE GUI_HEADER_FILES 
    "${HEADER_DIR}/ui/*.h"
)

# Core library
add_library(${PROJECT_NAME}_core STATIC
    ${CORE_SOURCE_FILES}
    ${CORE_HEADER_FILES}
)

target_include_directories(${PROJECT_NAME}_core PUBLIC
    ${HEADER_DIR}
)

target_link_libraries(${PROJECT_NAME}_core PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

# GUI executable
add_executable(${PROJECT_NAME}
    ${GUI_SOURCE_FILES}
    ${GUI_HEADER_FILES}
    ${UI_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${HEADER_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}_core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Charts
)

# Link libraries to GUI executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}_core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)

# Enhanced installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install required Qt plugins
install(DIRECTORY 
    "${QT_PLUGINS_DIR}/platforms"
    "${QT_PLUGINS_DIR}/multimedia"
    "${QT_PLUGINS_DIR}/tls"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/plugins
)

# Install application resources
install(FILES "${CMAKE_SOURCE_DIR}/resources/config.yaml"
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources/styles"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/styles
)

# Create a qt.conf file to help Qt find its plugins
file(WRITE "${CMAKE_BINARY_DIR}/qt.conf"
"[Paths]
Plugins = ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/qt6/plugins
")

install(FILES "${CMAKE_BINARY_DIR}/qt.conf"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CPack configuration
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "stinkbot")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Stinkbot App.")
set(CPACK_PACKAGE_VENDOR "Robert J Szigeti III")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/stinkbot")

include(CPack)