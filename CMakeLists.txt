cmake_minimum_required(VERSION 3.16)
project(Optix VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic compilation tools for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Configure output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find Packages
# 1. Qt 6 (Widgets is required for GUI)
find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

# 2. OpenCV (core, imgproc, imgcodecs, videoio, objdetect, face)
if(NOT OpenCV_DIR)
    if(EXISTS "C:/Users/ACER/Downloads/OpenCV_MingW-main/OpenCV_MingW-main/OpenCV4.11_mingw13.1.0")
        set(OpenCV_DIR "C:/Users/ACER/Downloads/OpenCV_MingW-main/OpenCV_MingW-main/OpenCV4.11_mingw13.1.0" CACHE PATH "Path to OpenCV build directory")
    elseif(EXISTS "C:/Users/ACER/Downloads/OpenCV_MingW-main/OpenCV4.11_mingw13.1.0")
        set(OpenCV_DIR "C:/Users/ACER/Downloads/OpenCV_MingW-main/OpenCV4.11_mingw13.1.0" CACHE PATH "Path to OpenCV build directory")
    elseif(EXISTS "C:/Users/ACER/Downloads/OpenCV4.11_mingw13.1.0")
        set(OpenCV_DIR "C:/Users/ACER/Downloads/OpenCV4.11_mingw13.1.0" CACHE PATH "Path to OpenCV build directory")
    else()
        set(OpenCV_DIR "C:/opencv-4.5.0-mingw" CACHE PATH "Path to OpenCV build directory")
    endif()
endif()
find_package(OpenCV COMPONENTS core imgproc imgcodecs videoio objdetect face)

# 3. SQLite3
find_package(SQLite3)
if(NOT SQLite3_FOUND AND EXISTS "C:/vcpkg/installed/x64-mingw-dynamic/include/sqlite3.h")
    set(SQLite3_INCLUDE_DIRS "C:/vcpkg/installed/x64-mingw-dynamic/include" CACHE PATH "SQLite3 include dir")
    set(SQLite3_LIBRARIES "C:/vcpkg/installed/x64-mingw-dynamic/lib/libsqlite3.dll.a" CACHE FILEPATH "SQLite3 library file")
    set(SQLite3_FOUND TRUE)
endif()

if(OpenCV_FOUND AND SQLite3_FOUND)
    message(STATUS "Optix: Real hardware camera & OpenCV backend enabled.")
else()
    message(WARNING "Optix: OpenCV or SQLite3 not found. Building with MOCK BACKENDS for UI preview.")
    add_compile_definitions(OPTIX_MOCK_BACKEND)
endif()

# Define compile options
if(MSVC)
    add_compile_options(/W4 /WX- /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Print status of dependencies for debugging compilation environment
message(STATUS "Optix: Configuring Build System...")
message(STATUS "Optix: Qt6 version: ${Qt6_VERSION}")
message(STATUS "Optix: OpenCV version: ${OpenCV_VERSION}")
message(STATUS "Optix: OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "Optix: SQLite3 include: ${SQLite3_INCLUDE_DIRS}")
message(STATUS "Optix: SQLite3 library: ${SQLite3_LIBRARIES}")

# Include source subdirectory
add_subdirectory(src)
