cmake_minimum_required(VERSION 3.1)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set (QT_INSTALL C:/Qt/Qt5.6.3/5.6.3/msvc2015_64)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (QT_INSTALL /opt/Qt5.6.3/5.6.3/gcc_64)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fvisibility=hidden -s")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

set (Qt5_DIR ${QT_INSTALL}/lib/cmake/Qt5)
set (Qt5Core_DIR ${QT_INSTALL}/lib/cmake/Qt5Core)

set (CMAKE_CXX_STANDARD 11)

find_package(Qt5Core REQUIRED)
set( CMAKE_AUTOMOC ON )

include_directories("src/")
include_directories("visualc/include/")

set(LIBHTMLPARSER_SRCS
src/attribute.h
src/attribute.c
src/char_ref.h
src/char_ref.c
src/error.h
src/error.c
src/gumbo.h
src/insertion_mode.h
src/parser.h
src/parser.c
src/string_buffer.h
src/string_buffer.c
src/string_piece.h
src/string_piece.c
visualc/include/strings.h
src/tag.c
src/token_type.h
src/tokenizer.h
src/tokenizer.c
src/utf8.h
src/utf8.c
src/util.h
src/util.c
src/vector.h
src/vector.c
src/htmlparser_interface.h
src/htmlparser_interface.cpp
    )

add_library(htmlparser SHARED ${LIBHTMLPARSER_SRCS} )

target_link_libraries(
    htmlparser
    Qt5::Core
)
INSTALL(TARGETS htmlparser LIBRARY DESTINATION lib COMPONENT htmlparser)
add_definitions(-DLIBHTMLPARSER)
