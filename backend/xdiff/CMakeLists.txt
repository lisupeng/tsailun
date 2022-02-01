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

set(LIBXDIFF_SRCS
src/xalloc.c
src/xdiff.h
src/xdiffi.c
src/xdiffi.h
src/xdiff_interface.c
src/xdiff_interface.h
src/xemit.c
src/xemit.h
src/xinclude.h
src/xmacros.h
src/xmissing.c
src/xmissing.h
src/xpatchi.c
src/xprepare.c
src/xprepare.h
src/xtestutils.c
src/xtestutils.h
src/xtypes.h
src/xutils.c
src/xutils.h
    )

add_library(xdiff SHARED ${LIBXDIFF_SRCS} )

target_link_libraries(
    xdiff
    Qt5::Core
)
INSTALL(TARGETS xdiff LIBRARY DESTINATION lib COMPONENT xdiff)
add_definitions(-DLIBXDIFF_LIBRARY)
