project(xbmcdepends)

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
cmake_policy (VERSION 2.6)

set(APP_VERSION_MAJOR "13" CACHE string "Application version major")
set(APP_VERSION_MINOR "2" CACHE string "Application version minor")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platform)

#
# Install required headers from xbmc depends
IF(WIN32)
  configure_file(pvr/dlfcn-win32.h ${CMAKE_BINARY_DIR}/include/xbmc/dlfcn-win32.h COPYONLY)
ENDIF(WIN32)
configure_file(pvr/libXBMC_addon.h ${CMAKE_BINARY_DIR}/include/xbmc/libXBMC_addon.h COPYONLY)
configure_file(pvr/libXBMC_codec.h ${CMAKE_BINARY_DIR}/include/xbmc/libXBMC_codec.h COPYONLY)
configure_file(pvr/libXBMC_gui.h ${CMAKE_BINARY_DIR}/include/xbmc/libXBMC_gui.h COPYONLY)
configure_file(pvr/libXBMC_pvr.h ${CMAKE_BINARY_DIR}/include/xbmc/libXBMC_pvr.h COPYONLY)
configure_file(pvr/DVDDemuxPacket.h ${CMAKE_BINARY_DIR}/include/xbmc/DVDDemuxPacket.h COPYONLY)
configure_file(pvr/xbmc_addon_cpp_dll.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_addon_cpp_dll.h COPYONLY)
configure_file(pvr/xbmc_addon_dll.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_addon_dll.h COPYONLY)
configure_file(pvr/xbmc_addon_types.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_addon_types.h COPYONLY)
configure_file(pvr/xbmc_codec_types.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_codec_types.h COPYONLY)
configure_file(pvr/xbmc_epg_types.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_epg_types.h COPYONLY)
configure_file(pvr/xbmc_pvr_dll.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_pvr_dll.h COPYONLY)
configure_file(pvr/xbmc_pvr_types.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_pvr_types.h COPYONLY)
configure_file(pvr/xbmc_scr_dll.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_scr_dll.h COPYONLY)
configure_file(pvr/xbmc_scr_types.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_scr_types.h COPYONLY)
configure_file(pvr/xbmc_stream_utils.hpp ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_stream_utils.hpp COPYONLY)
configure_file(pvr/xbmc_codec_descriptor.hpp ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_codec_descriptor.hpp COPYONLY)
configure_file(pvr/xbmc_vis_dll.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_vis_dll.h COPYONLY)
configure_file(pvr/xbmc_vis_types.h ${CMAKE_BINARY_DIR}/include/xbmc/xbmc_vis_types.h COPYONLY)

