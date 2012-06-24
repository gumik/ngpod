cmake_minimum_required (VERSION 2.6)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")

project (ngpod)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/ngpod-config.h.in"
  "${PROJECT_BINARY_DIR}/ngpod-config.h"
  )

 include_directories("${PROJECT_BINARY_DIR}")

find_package (PkgConfig REQUIRED)

pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
include_directories (${GTK2_INCLUDE_DIRS})
link_directories (${GTK2_LIBRARY_DIRS})
add_definitions (${GTK2_CFLAGS_OTHER})

pkg_check_modules (LIBSOUP REQUIRED libsoup-2)
include_directories (${LIBSOUP_INCLUDE_DIRS})
link_directories (${LIBSOUP_LIBRARY_DIRS})
add_definitions (${LIBSOUP_CFLAGS_OTHER})

aux_source_directory (${PROJECT_SOURCE_DIR} SOURCES)
add_executable(ngpod ${SOURCES})

target_link_libraries (ngpod ${GTK2_LIBRARIES} ${LIBSOUP_LIBRARIES})
