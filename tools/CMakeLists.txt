cmake_minimum_required(VERSION 3.3)
project(nuvie-tools)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${nuvie_SOURCE_DIR}/builds/cmake")

find_package(SDL2 REQUIRED)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA_USE_MACOSX -DUNIX -DMACOSX -DHAVE_JOYSTICK_SUPPORT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_MACOSX -DMACOSX -DHAVE_JOYSTICK_SUPPORT")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32 -D_DEBUG -D_CONSOLE -DHAVE_JOYSTICK_SUPPORT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWIN32 -DHAVE_JOYSTICK_SUPPORT")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

SET(SDL_VERSION "SDL2")

set(unpack_lzc_SOURCE_FILES
        tools/unpack_lzc.cpp

        Debug.cpp
        conf/Configuration.cpp
        conf/Configuration.h
        conf/XMLNode.cpp
        conf/XMLNode.h
        conf/XMLTree.cpp
        conf/XMLTree.h
        conf/ConfigNode.h

        files/NuvieIO.cpp
        files/NuvieIO.h
        files/NuvieIOFile.cpp
        files/NuvieIOFile.h
        files/U6Lib_n.cpp
        files/U6Lib_n.h
        files/U6Lzw.cpp
        files/U6Lzw.h

        misc/U6misc.h
        misc/U6misc.cpp
        )

set(unpack_font_SOURCE_FILES
        tools/unpack_font.cpp

        Debug.cpp
        conf/Configuration.cpp
        conf/Configuration.h
        conf/XMLNode.cpp
        conf/XMLNode.h
        conf/XMLTree.cpp
        conf/XMLTree.h
        conf/ConfigNode.h

        files/NuvieIO.cpp
        files/NuvieIO.h
        files/NuvieIOFile.cpp
        files/NuvieIOFile.h
        files/NuvieBmpFile.cpp
        files/NuvieBmpFIle.h
        files/U6Lib_n.cpp
        files/U6Lib_n.h
        files/U6Lzw.cpp
        files/U6Lzw.h

        misc/U6misc.h
        misc/U6misc.cpp
        )

set(pack_font_SOURCE_FILES
        tools/pack_font.cpp

        Debug.cpp
        conf/Configuration.cpp
        conf/Configuration.h
        conf/XMLNode.cpp
        conf/XMLNode.h
        conf/XMLTree.cpp
        conf/XMLTree.h
        conf/ConfigNode.h

        files/NuvieIO.cpp
        files/NuvieIO.h
        files/NuvieIOFile.cpp
        files/NuvieIOFile.h
        files/NuvieBmpFile.cpp
        files/NuvieBmpFIle.h
        files/U6Lib_n.cpp
        files/U6Lib_n.h
        files/U6Lzw.cpp
        files/U6Lzw.h

        misc/U6misc.h
        misc/U6misc.cpp
        )

set(pack_lzc_SOURCE_FILES
        tools/pack_lzc.cpp

        Debug.cpp
        conf/Configuration.cpp
        conf/Configuration.h
        conf/XMLNode.cpp
        conf/XMLNode.h
        conf/XMLTree.cpp
        conf/XMLTree.h
        conf/ConfigNode.h

        files/NuvieIO.cpp
        files/NuvieIO.h
        files/NuvieIOFile.cpp
        files/NuvieIOFile.h
        files/U6Lib_n.cpp
        files/U6Lib_n.h
        files/U6Lzw.cpp
        files/U6Lzw.h

        misc/U6misc.h
        misc/U6misc.cpp
        )

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
include_directories(.)
include_directories(./tools)
include_directories(conf)
include_directories(misc)

add_executable(pack_lzc ${pack_lzc_SOURCE_FILES})
add_executable(unpack_lzc ${unpack_lzc_SOURCE_FILES})
add_executable(unpack_font ${unpack_font_SOURCE_FILES})
add_executable(pack_font ${pack_font_SOURCE_FILES})

TARGET_LINK_LIBRARIES(unpack_font ${SDL2_LIBRARY})
TARGET_LINK_LIBRARIES(pack_font ${SDL2_LIBRARY})

