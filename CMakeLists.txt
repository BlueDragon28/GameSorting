# MIT Licence
# 
# This file is part of the GameSorting
# 
# Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.16.0)

project(gamesorting VERSION 1.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

include_directories(include/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.0 COMPONENTS Widgets Sql REQUIRED)

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

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	if (WIN32)
		set(WIN32_PROGRAM WIN32)
	else()
		set(WIN32_PROGRAM "")
	endif()
else()
	set(WIN32_PROGRAM "")
endif()

add_executable(gamesorting 
	${WIN32_PROGRAM}
	${SOURCES}
	${HEADERS}
	${RESOURCES})
target_link_libraries(gamesorting PRIVATE Qt6::Widgets Qt6::Sql)
set_target_properties(gamesorting PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
install(TARGETS gamesorting RUNTIME DESTINATION bin)