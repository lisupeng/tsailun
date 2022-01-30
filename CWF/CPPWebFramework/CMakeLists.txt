cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set (QT_INSTALL C:/Qt/Qt5.6.3/5.6.3/msvc2015_64)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (QT_INSTALL /opt/Qt5.6.3/5.6.3/gcc_64)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fvisibility=hidden -s")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
#set (QT_INSTALL C:/Qt/Qt5.6.3/5.6.3/msvc2015_64)
# below for Linux
#set (QT_INSTALL /opt/Qt5.6.3/5.6.3/gcc_64)

set (Qt5_DIR ${QT_INSTALL}/lib/cmake/Qt5)

set (Qt5Core_DIR ${QT_INSTALL}/lib/cmake/Qt5Core)
set (Qt5Network_DIR ${QT_INSTALL}/lib/cmake/Qt5Network)
set (Qt5Sql_DIR ${QT_INSTALL}/lib/cmake/Qt5Sql)
set (Qt5Xml_DIR ${QT_INSTALL}/lib/cmake/Qt5Xml)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Xml REQUIRED)
set( CMAKE_AUTOMOC ON )

include_directories("cwf/")

set(CPP_WebFrameWork_SRCS
    cwf/configuration.cpp
    cwf/cppwebapplication.cpp
    cwf/cppwebserver.cpp
    cwf/cstlcompiler.cpp
    cwf/cstlcompilerattributes.cpp
    cwf/cstlcompilerfor.cpp
    cwf/cstlcompilerif.cpp
    cwf/cstlcompilerimport.cpp
    cwf/filemanager.cpp
    cwf/filterchain.cpp
    cwf/filter.cpp
    cwf/httpparser.cpp
    cwf/httpreadrequest.cpp
    cwf/metaclassparser.cpp
    cwf/properties.cpp
    cwf/qlistobject.cpp
    cwf/requestdispatcher.cpp
    cwf/urlencoder.cpp
    cwf/sqlquery.cpp
    cwf/response.cpp
    cwf/request.cpp
    cwf/controller.cpp
    cwf/session.cpp
    cwf/cppwebcontroller.cpp
    cwf/sslloader.cpp
    cwf/model.cpp
    cwf/modelbasicoperation.cpp
    cwf/sqlquerymanager.cpp
    cwf/cstlcompilerobject.h
    cwf/variant.h

    )

add_library(CPPWebFramework SHARED ${CPP_WebFrameWork_SRCS} )

target_link_libraries(
    CPPWebFramework
    Qt5::Core
    Qt5::Network
    Qt5::Xml
    Qt5::Sql

)
INSTALL(TARGETS CPPWebFramework LIBRARY DESTINATION lib COMPONENT CPPWebFramework)
add_definitions(-DCPPWEBFRAMEWORK_LIBRARY)
