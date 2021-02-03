# MIT License
# 
# This file is part of the GameSorting
# 
# Copyright © 2021 Erwan Saclier de la Bâtie
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

if (WIN32)
	if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
		add_custom_target(Qt5Binaries 
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Cored.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Guid.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Widgetsd.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Svgd.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/platforms/qwindowsd.dll" "${CMAKE_BINARY_DIR}/bin/plugins/platforms/qwindowsd.dll"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/styles/qwindowsvistastyled.dll" "${CMAKE_BINARY_DIR}/bin/plugins/styles/qwindowsvistastyled.dll"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/imageformats/qicod.dll" "${CMAKE_BINARY_DIR}/bin/plugins/imageformats/qicod.dll"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/imageformats/qsvgd.dll" "${CMAKE_BINARY_DIR}/bin/plugins/imageformats/qsvgd.dll"
			)
		add_dependencies(GameSorting Qt5Binaries)
	else()
		add_custom_target(Qt5Binaries 
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Core.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Gui.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Widgets.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../bin/Qt5Svg.dll" "${CMAKE_BINARY_DIR}/bin"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/platforms/qwindows.dll" "${CMAKE_BINARY_DIR}/bin/plugins/platforms/qwindows.dll"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/styles/qwindowsvistastyle.dll" "${CMAKE_BINARY_DIR}/bin/plugins/styles/qwindowsvistastyle.dll"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/imageformats/qico.dll" "${CMAKE_BINARY_DIR}/bin/plugins/imageformats/qico.dll"
			COMMAND ${CMAKE_COMMAND} -E copy "${Qt5_DIR}/../../../plugins/imageformats/qsvg.dll" "${CMAKE_BINARY_DIR}/bin/plugins/imageformats/qsvg.dll"
			)
		add_dependencies(GameSorting Qt5Binaries)
	endif()
endif()