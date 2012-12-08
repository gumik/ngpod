cmake_minimum_required (VERSION 2.6)

macro(use_library name prefix)
	pkg_check_modules (${prefix} REQUIRED ${name})
	include_directories (${${prefix}_INCLUDE_DIRS})
	link_directories (${${prefix}_LIBRARY_DIRS})
	add_definitions (${${prefix}_CFLAGS_OTHER})
	list (APPEND LIBRARIES_LIST ${${prefix}_LIBRARIES})
endmacro(use_library)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")

project (ngpod)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

find_package (PkgConfig REQUIRED)

use_library (gtkmm-3.0 GTKMM)
use_library (libsoup-2.4 LIBSOUP)
use_library (mateconf-2.0 MATECONF)
use_library (libxfconf-0 LIBXFCONF)
use_library (libxml-2.0 LIBXML)
find_package(Boost COMPONENTS date_time filesystem system REQUIRED)

add_custom_command (OUTPUT resources.c
                    COMMAND glib-compile-resources --generate-source resources.xml
                    DEPENDS resources.xml)

aux_source_directory (${PROJECT_SOURCE_DIR} SOURCES)
add_executable(ngpod ${SOURCES} resources.c)

target_link_libraries (ngpod ${LIBRARIES_LIST} ${Boost_LIBRARIES})