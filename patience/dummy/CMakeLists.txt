cmake_minimum_required(VERSION 3.21)

# get directory for file
function(get_relative_directory FILE_PATH RELATIVE_DIR)
	if (IS_ABSOLUTE "${FILE_PATH}")
		file(RELATIVE_PATH REL_SOURCE_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${FILE_PATH}")
	else()
		set(REL_SOURCE_FILE_PATH "${FILE_PATH}")
	endif()

	get_filename_component(PATH_FOR_FILE "${REL_SOURCE_FILE_PATH}" DIRECTORY)
	string(REPLACE "/" "\\" REL_DIR "${PATH_FOR_FILE}")
	set("${RELATIVE_DIR}" "${REL_DIR}" PARENT_SCOPE)
endfunction()

# filters for source files
function(assign_source_group)
	foreach(f ${ARGN})
		get_relative_directory("${f}" dir)
		source_group("${dir}" FILES "${f}")
	endforeach()
endfunction()

# export msbuild path
function(export_msbuild_path COMPILER_PATH OUTFILE)
	get_filename_component(DIRNAME ${COMPILER_PATH} NAME)
	set(PREV_DIRNAME ${DIRNAME})
	while(NOT ${PREV_DIRNAME} MATCHES "VC")
		set(PREV_DIRNAME ${DIRNAME})
		get_filename_component(COMPILER_PATH ${COMPILER_PATH} DIRECTORY)
		get_filename_component(DIRNAME ${COMPILER_PATH} NAME)
	endwhile()

	file(WRITE ${OUTFILE} "${COMPILER_PATH}/MSBuild/Current/Bin/amd64/MSBuild.exe")
endfunction()

# declare the project
project(PatienceDummy)

# export msbuild executable path so we can compile from the command line
export_msbuild_path(${CMAKE_CXX_COMPILER} ${CMAKE_BINARY_DIR}/msbuild)

# set the output directory
set(TARGET_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR})
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${CONFIG} CONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${TARGET_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${TARGET_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${TARGET_DIR})
endforeach()

# project properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION "1")
set(BUILD_SHARED_LIBS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# include modules
add_subdirectory(brokenConfig)
add_subdirectory(failingTest)
add_subdirectory(longTest)
add_subdirectory(outputFiles)
add_subdirectory(simpleArgs)
