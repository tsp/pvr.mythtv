project(xbmcplatform)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)
include(CheckAtomic.cmake)

set(xbmcplatform_NAME xbmcplatform)
set(xbmcplatform_DESCRIPTION "Kodi add-on platform support library")
set(xbmcplatform_VERSION_MAJOR ${APP_VERSION_MAJOR})
set(xbmcplatform_VERSION_MINOR ${APP_VERSION_MINOR})
set(xbmcplatform_VERSION_PATCH 0)

if(WIN32)
  add_definitions(-D_WIN32)
  set(PLAT_SOURCES src/windows/dlfcn-win32.cpp
                   src/windows/os-threads.cpp
                   src/windows/serialport.cpp)
else()
  set(PLAT_SOURCES src/posix/serialport.cpp)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

set(xbmcplatform_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/xbmc")
IF(WIN32)
  LIST(APPEND xbmcplatform_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/xbmc/windows")
ENDIF(WIN32)
set(xbmcplatform_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

if(NOT ${CORE_SYSTEM_NAME} STREQUAL "")
  if(${CORE_SYSTEM_NAME} STREQUAL "darwin" OR ${CORE_SYSTEM_NAME} STREQUAL "ios")
    list(APPEND xbmcplatform_LIBRARIES "-framework CoreVideo -framework IOKit")
  endif()
endif()

add_library(xbmcplatform STATIC ${SOURCES} ${PLAT_SOURCES})
target_link_libraries(xbmcplatform ${xbmcplatform_LIBRARIES})
set_target_properties(xbmcplatform PROPERTIES VERSION ${xbmcplatform_VERSION_MAJOR}.${xbmcplatform_VERSION_MINOR}.${xbmcplatform_VERSION_PATCH}
                                              SOVERSION ${xbmcplatform_VERSION_MAJOR}.0)

#
# Install required headers from xbmc-platform
configure_file(src/os.h ${CMAKE_BINARY_DIR}/include/xbmc/os.h COPYONLY)
IF(WIN32)
  configure_file(src/windows/dlfcn-win32.h ${CMAKE_BINARY_DIR}/include/xbmc/windows/dlfcn-win32.h COPYONLY)
  configure_file(src/windows/os-socket.h ${CMAKE_BINARY_DIR}/include/xbmc/windows/os-socket.h COPYONLY)
  configure_file(src/windows/os-threads.h ${CMAKE_BINARY_DIR}/include/xbmc/windows/os-threads.h COPYONLY)
  configure_file(src/windows/os-types.h ${CMAKE_BINARY_DIR}/include/xbmc/windows/os-types.h COPYONLY)
ELSE(WIN32)
  configure_file(src/posix/os-socket.h ${CMAKE_BINARY_DIR}/include/xbmc/posix/os-socket.h COPYONLY)
  configure_file(src/posix/os-threads.h ${CMAKE_BINARY_DIR}/include/xbmc/posix/os-threads.h COPYONLY)
  configure_file(src/posix/os-types.h ${CMAKE_BINARY_DIR}/include/xbmc/posix/os-types.h COPYONLY)
ENDIF(WIN32)
configure_file(src/sockets/serialport.h ${CMAKE_BINARY_DIR}/include/xbmc/sockets/serialport.h COPYONLY)
configure_file(src/sockets/socket.h ${CMAKE_BINARY_DIR}/include/xbmc/sockets/socket.h COPYONLY)
configure_file(src/sockets/tcp.h ${CMAKE_BINARY_DIR}/include/xbmc/sockets/tcp.h COPYONLY)
configure_file(src/threads/atomics.h ${CMAKE_BINARY_DIR}/include/xbmc/threads/atomics.h COPYONLY)
configure_file(src/threads/mutex.h ${CMAKE_BINARY_DIR}/include/xbmc/threads/mutex.h COPYONLY)
configure_file(src/threads/threads.h ${CMAKE_BINARY_DIR}/include/xbmc/threads/threads.h COPYONLY)
configure_file(src/util/atomic.h ${CMAKE_BINARY_DIR}/include/xbmc/util/atomic.h COPYONLY)
configure_file(src/util/baudrate.h ${CMAKE_BINARY_DIR}/include/xbmc/util/baudrate.h COPYONLY)
configure_file(src/util/buffer.h ${CMAKE_BINARY_DIR}/include/xbmc/util/buffer.h COPYONLY)
configure_file(src/util/timeutils.h ${CMAKE_BINARY_DIR}/include/xbmc/util/timeutils.h COPYONLY)
configure_file(src/util/util.h ${CMAKE_BINARY_DIR}/include/xbmc/util/util.h COPYONLY)

