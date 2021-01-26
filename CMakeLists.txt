cmake_minimum_required(VERSION 3.1.0)

project(GameSorting VERSION 0.11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

include_directories(include/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 5.15 COMPONENTS Widgets REQUIRED)

# Automatically add into variable the headers and sources files.
file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.cpp)

# Adding the resources file.
set(RESOURCES 
	"gamesorting.qrc")
# This resources files is only add on windows.
# It's use to add the icon of the program to the executable.
if(WIN32)
	set(RESOURCES 
		${RESOURCES}
		"gamesorting.rc")
endif()

if (WIN32)
	set(WIN32_PROGRAM WIN32)
else()
	set(WIN32_PROGRAM "")
endif()

add_executable(GameSorting 
	${WIN32_PROGRAM}
	${SOURCES}
	${HEADERS}
	${RESOURCES})
target_link_libraries(GameSorting Qt5::Widgets)
set_target_properties(GameSorting PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")