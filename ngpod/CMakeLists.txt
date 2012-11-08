cmake_minimum_required (VERSION 2.6)

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

pkg_check_modules (GTK2 REQUIRED gtkmm-3.0)
include_directories (${GTK2_INCLUDE_DIRS})
link_directories (${GTK2_LIBRARY_DIRS})
add_definitions (${GTK2_CFLAGS_OTHER})

pkg_check_modules (LIBSOUP REQUIRED libsoup-2.4)
include_directories (${LIBSOUP_INCLUDE_DIRS})
link_directories (${LIBSOUP_LIBRARY_DIRS})
add_definitions (${LIBSOUP_CFLAGS_OTHER})

pkg_check_modules (MATECONF2 REQUIRED mateconf-2.0)
include_directories (${MATECONF2_INCLUDE_DIRS})
link_directories (${MATECONF2_LIBRARY_DIRS})
add_definitions (${MATECONF2_CFLAGS_OTHER})

pkg_check_modules (LIBXML2 REQUIRED libxml-2.0)
include_directories (${LIBXML2_INCLUDE_DIRS})
link_directories (${LIBXML2_LIBRARY_DIRS})
add_definitions (${LIBXML2_CFLAGS_OTHER})

add_custom_command (OUTPUT resources.c
                    COMMAND glib-compile-resources --generate-source resources.xml
                    DEPENDS resources.xml)

aux_source_directory (${PROJECT_SOURCE_DIR} SOURCES)
add_executable(ngpod ${SOURCES} resources.c)

target_link_libraries (ngpod ${GTK2_LIBRARIES} ${LIBSOUP_LIBRARIES} ${MATECONF2_LIBRARIES} ${LIBXML2_LIBRARIES})
