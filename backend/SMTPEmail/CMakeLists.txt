cmake_minimum_required(VERSION 3.1)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set (QT_INSTALL C:/Qt/Qt5.6.3/5.6.3/msvc2015_64)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (QT_INSTALL /opt/Qt5.6.3/5.6.3/gcc_64)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fvisibility=hidden -s")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

set (Qt5_DIR ${QT_INSTALL}/lib/cmake/Qt5)
set (Qt5Core_DIR ${QT_INSTALL}/lib/cmake/Qt5Core)
set (Qt5Network_DIR ${QT_INSTALL}/lib/cmake/Qt5Network)

set (CMAKE_CXX_STANDARD 11)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
set( CMAKE_AUTOMOC ON )

include_directories("src/")

set(SMTPEmail_SRCS
    src/emailaddress.cpp
    src/mimeattachment.cpp
    src/mimefile.cpp
    src/mimehtml.cpp
    src/mimeinlinefile.cpp
    src/mimemessage.cpp
    src/mimepart.cpp
    src/mimetext.cpp
    src/smtpclient.cpp
    src/quotedprintable.cpp
    src/mimemultipart.cpp
    src/mimecontentformatter.cpp
    src/emailaddress.h
    src/mimeattachment.h
    src/mimefile.h
    src/mimehtml.h
    src/mimeinlinefile.h
    src/mimemessage.h
    src/mimepart.h
    src/mimetext.h
    src/smtpclient.h
    src/SmtpMime
    src/quotedprintable.h
    src/mimemultipart.h
    src/mimecontentformatter.h
    src/smtpexports.h
    )

add_library(SMTPEmail SHARED ${SMTPEmail_SRCS} )

target_link_libraries(
    SMTPEmail
    Qt5::Core
    Qt5::Network
)
INSTALL(TARGETS SMTPEmail LIBRARY DESTINATION lib COMPONENT SMTPEmail)
add_definitions(-DSMTP_BUILD)
