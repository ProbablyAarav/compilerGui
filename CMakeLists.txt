cmake_minimum_required(VERSION 3.16)
project(NPAVC_GUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automatic MOC (Meta Object Compiler) for Qt
set(CMAKE_AUTOMOC ON)

# Create the executable
add_executable(npavc_gui
    npavGui.cpp
)

# Link Qt6 libraries
target_link_libraries(npavc_gui
    Qt6::Core
    Qt6::Widgets
)

# Set up Qt6 if found
if(Qt6_FOUND)
    qt6_standard_project_setup()
endif()

# Copy npavc executable to build directory (optional)
# Uncomment and adjust path as needed
# configure_file(${CMAKE_SOURCE_DIR}/npavc ${CMAKE_BINARY_DIR}/npavc COPYONLY)
