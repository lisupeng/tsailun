cmake_minimum_required(VERSION 3.1.1)

project(TslServer)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set (QT_INSTALL C:/Qt/Qt5.6.3/5.6.3/msvc2015_64)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (QT_INSTALL /opt/Qt5.6.3/5.6.3/gcc_64)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fpermissive -fvisibility=hidden -s")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set (Qt5Core_DIR ${QT_INSTALL}/lib/cmake/Qt5Core)

find_package(Qt5Core REQUIRED)


add_subdirectory(../CWF/CPPWebFramework CPPWebFramework)
add_subdirectory (./xdiff xdiff)
add_subdirectory (./htmlparser htmlparser)
add_subdirectory (./SMTPEmail SMTPEmail)
include_directories(.
    ../CWF/CPPWebFramework/
    ../htmlparser/src/
    ../SMTPEmail/src/
    ${CMAKE_CURRENT_BINARY_DIR}
)


add_executable(${PROJECT_NAME} main.cpp utils.h utils.cpp pagemgr.h pagemgr.cpp configmgr.h configmgr.cpp pagelistmgr.h 
    pagelistmgr.cpp userdbmgr.h userdbmgr.cpp crypto.h crypto.cpp sessionmgr.h sessionmgr.cpp historymgr.h historymgr.cpp 
    versionrecordmgr.h versionrecordmgr.cpp spacedbmgr.h spacedbmgr.cpp basedbmgr.h basedbmgr.cpp groupdbmgr.h groupdbmgr.cpp 
    auth.h auth.cpp syslog.h syslog.cpp search.h search.cpp stats.h stats.cpp serverapp.h serverapp.cpp demomgr.h demomgr.cpp 
    urlhandlers.h urlhandlers.cpp sysdefs.h opcodehandlers.h opcodehandlers.cpp)

target_link_libraries(${PROJECT_NAME} Qt5::Core  CPPWebFramework xdiff htmlparser SMTPEmail)
