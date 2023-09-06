# *****************************************************************************
# *
# *  PROJECT:        SampConsoleLauncher
# *  LICENSE:        See LICENSE in the top level directory
# *  FILE:           CMakeLists.txt
# *  DESCRIPTION:    CMakeLists
# *  COPYRIGHT:      (c) 2023 RINWARES <rinwares.com>
# *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
# *
# *****************************************************************************

cmake_minimum_required(VERSION 3.8)

project("samp_launcher" C)

set(${PROJECT_NAME}_SOURCES
	"src/main.c"
	"src/main.h"
	"src/utils.c"
	"src/utils.h"
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

install(TARGETS ${PROJECT_NAME} DESTINATION .)
