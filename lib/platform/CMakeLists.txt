project(platform)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)
enable_language(C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)
include(CheckAtomic.cmake)

set(platform_NAME platform)
set(platform_DESCRIPTION "Platform support library")
set(platform_VERSION_MAJOR 1)
set(platform_VERSION_MINOR 0)
set(platform_VERSION_PATCH 9)

if(WIN32)
  add_definitions(-D_WIN32)
  set(PLAT_SOURCES src/windows/dlfcn-win32.cpp
                   src/windows/os-threads.cpp)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

set(platform_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/platform")
IF(WIN32)
  LIST(APPEND platform_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/platform/windows")
ENDIF(WIN32)
set(platform_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

if(NOT ${CORE_SYSTEM_NAME} STREQUAL "")
  if(${CORE_SYSTEM_NAME} STREQUAL "darwin" OR ${CORE_SYSTEM_NAME} STREQUAL "ios")
    list(APPEND platform_LIBRARIES "-framework CoreVideo -framework IOKit")
  endif()
endif()

set(SOURCES src/util/fstrcmp.c
            src/util/StringUtils.cpp)

add_library(platform STATIC ${SOURCES} ${PLAT_SOURCES})
target_link_libraries(platform ${platform_LIBRARIES})
set_target_properties(platform PROPERTIES VERSION ${platform_VERSION_MAJOR}.${platform_VERSION_MINOR}.${platform_VERSION_PATCH}
                                              SOVERSION ${platform_VERSION_MAJOR}.0)

#
# Install required headers from platform
configure_file(src/os.h ${CMAKE_BINARY_DIR}/include/platform/os.h COPYONLY)
IF(WIN32)
  configure_file(src/windows/dlfcn-win32.h ${CMAKE_BINARY_DIR}/include/platform/windows/dlfcn-win32.h COPYONLY)
  configure_file(src/windows/os-socket.h ${CMAKE_BINARY_DIR}/include/platform/windows/os-socket.h COPYONLY)
  configure_file(src/windows/os-threads.h ${CMAKE_BINARY_DIR}/include/platform/windows/os-threads.h COPYONLY)
  configure_file(src/windows/os-types.h ${CMAKE_BINARY_DIR}/include/platform/windows/os-types.h COPYONLY)
ELSE(WIN32)
  configure_file(src/posix/os-socket.h ${CMAKE_BINARY_DIR}/include/platform/posix/os-socket.h COPYONLY)
  configure_file(src/posix/os-threads.h ${CMAKE_BINARY_DIR}/include/platform/posix/os-threads.h COPYONLY)
  configure_file(src/posix/os-types.h ${CMAKE_BINARY_DIR}/include/platform/posix/os-types.h COPYONLY)
ENDIF(WIN32)
configure_file(src/sockets/cdevsocket.h ${CMAKE_BINARY_DIR}/include/platform/sockets/cdevsocket.h COPYONLY)
configure_file(src/sockets/socket.h ${CMAKE_BINARY_DIR}/include/platform/sockets/socket.h COPYONLY)
configure_file(src/sockets/tcp.h ${CMAKE_BINARY_DIR}/include/platform/sockets/tcp.h COPYONLY)
configure_file(src/threads/atomics.h ${CMAKE_BINARY_DIR}/include/platform/threads/atomics.h COPYONLY)
configure_file(src/threads/mutex.h ${CMAKE_BINARY_DIR}/include/platform/threads/mutex.h COPYONLY)
configure_file(src/threads/threads.h ${CMAKE_BINARY_DIR}/include/platform/threads/threads.h COPYONLY)
configure_file(src/util/atomic.h ${CMAKE_BINARY_DIR}/include/platform/util/atomic.h COPYONLY)
configure_file(src/util/buffer.h ${CMAKE_BINARY_DIR}/include/platform/util/buffer.h COPYONLY)
configure_file(src/util/StringUtils.h ${CMAKE_BINARY_DIR}/include/platform/util/StringUtils.h COPYONLY)
configure_file(src/util/StdString.h ${CMAKE_BINARY_DIR}/include/platform/util/StdString.h COPYONLY)
configure_file(src/util/timeutils.h ${CMAKE_BINARY_DIR}/include/platform/util/timeutils.h COPYONLY)
configure_file(src/util/util.h ${CMAKE_BINARY_DIR}/include/platform/util/util.h COPYONLY)

