project(kodiplatform)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)
include(CheckAtomic.cmake)

set(kodiplatform_NAME kodiplatform)
set(kodiplatform_DESCRIPTION "Kodi add-on platform support library")
set(kodiplatform_VERSION_MAJOR ${APP_VERSION_MAJOR})
set(kodiplatform_VERSION_MINOR ${APP_VERSION_MINOR})
set(kodiplatform_VERSION_PATCH 0)

if(WIN32)
  add_definitions(-D_WIN32)
  set(PLAT_SOURCES src/windows/dlfcn-win32.cpp
                   src/windows/os-threads.cpp
                   src/windows/serialport.cpp)
else()
  set(PLAT_SOURCES src/posix/serialport.cpp)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

set(kodiplatform_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/kodi")
IF(WIN32)
  LIST(APPEND kodiplatform_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/kodi/windows")
ENDIF(WIN32)
set(kodiplatform_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

if(NOT ${CORE_SYSTEM_NAME} STREQUAL "")
  if(${CORE_SYSTEM_NAME} STREQUAL "darwin" OR ${CORE_SYSTEM_NAME} STREQUAL "ios")
    list(APPEND kodiplatform_LIBRARIES "-framework CoreVideo -framework IOKit")
  endif()
endif()

add_library(kodiplatform STATIC ${SOURCES} ${PLAT_SOURCES})
target_link_libraries(kodiplatform ${kodiplatform_LIBRARIES})
set_target_properties(kodiplatform PROPERTIES VERSION ${kodiplatform_VERSION_MAJOR}.${kodiplatform_VERSION_MINOR}.${kodiplatform_VERSION_PATCH}
                                              SOVERSION ${kodiplatform_VERSION_MAJOR}.0)

#
# Install required headers from kodi-platform
configure_file(src/os.h ${CMAKE_BINARY_DIR}/include/kodi/os.h COPYONLY)
IF(WIN32)
  configure_file(src/windows/dlfcn-win32.h ${CMAKE_BINARY_DIR}/include/kodi/windows/dlfcn-win32.h COPYONLY)
  configure_file(src/windows/os-socket.h ${CMAKE_BINARY_DIR}/include/kodi/windows/os-socket.h COPYONLY)
  configure_file(src/windows/os-threads.h ${CMAKE_BINARY_DIR}/include/kodi/windows/os-threads.h COPYONLY)
  configure_file(src/windows/os-types.h ${CMAKE_BINARY_DIR}/include/kodi/windows/os-types.h COPYONLY)
ELSE(WIN32)
  configure_file(src/posix/os-socket.h ${CMAKE_BINARY_DIR}/include/kodi/posix/os-socket.h COPYONLY)
  configure_file(src/posix/os-threads.h ${CMAKE_BINARY_DIR}/include/kodi/posix/os-threads.h COPYONLY)
  configure_file(src/posix/os-types.h ${CMAKE_BINARY_DIR}/include/kodi/posix/os-types.h COPYONLY)
ENDIF(WIN32)
configure_file(src/sockets/serialport.h ${CMAKE_BINARY_DIR}/include/kodi/sockets/serialport.h COPYONLY)
configure_file(src/sockets/socket.h ${CMAKE_BINARY_DIR}/include/kodi/sockets/socket.h COPYONLY)
configure_file(src/sockets/tcp.h ${CMAKE_BINARY_DIR}/include/kodi/sockets/tcp.h COPYONLY)
configure_file(src/threads/atomics.h ${CMAKE_BINARY_DIR}/include/kodi/threads/atomics.h COPYONLY)
configure_file(src/threads/mutex.h ${CMAKE_BINARY_DIR}/include/kodi/threads/mutex.h COPYONLY)
configure_file(src/threads/threads.h ${CMAKE_BINARY_DIR}/include/kodi/threads/threads.h COPYONLY)
configure_file(src/util/atomic.h ${CMAKE_BINARY_DIR}/include/kodi/util/atomic.h COPYONLY)
configure_file(src/util/baudrate.h ${CMAKE_BINARY_DIR}/include/kodi/util/baudrate.h COPYONLY)
configure_file(src/util/buffer.h ${CMAKE_BINARY_DIR}/include/kodi/util/buffer.h COPYONLY)
configure_file(src/util/timeutils.h ${CMAKE_BINARY_DIR}/include/kodi/util/timeutils.h COPYONLY)
configure_file(src/util/util.h ${CMAKE_BINARY_DIR}/include/kodi/util/util.h COPYONLY)

