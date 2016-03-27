# Set Minimum Version
cmake_minimum_required (VERSION 3.5)

# Create Project Name Environmental Variable
set (PROJNAME "drm")

# Create Project
project (${PROJNAME})

# Version Number
set (DRM_VERSION_MAJOR 0)
set (DRM_VERSION_MINOR 1)

# Set Up Directories
set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define All Include Directories
SET (INCLUDE
    ${INC_DIR}
)

# Define Where Header Files Are Located
SET (INC
    ${INC_DIR}/Repository.h
)

# Define Where Source Files Are Located
SET (SRC
    ${SRC_DIR}/Repository.cpp
    ${SRC_DIR}/main.cpp
)

# Check to see if we're using the G++ compiler
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v -std=c++1z")
    target_link_libraries(${PROJECT_NAME} -lstdc++fs)
endif()

# Include The Directories
include_directories (${INCLUDE})

# Add Executable
add_executable (${PROJNAME} ${SRC} ${INC})

# Set Up Outputs
set_target_properties (${PROJNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set_target_properties (${PROJNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
set_target_properties (${PROJNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
