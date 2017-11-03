cmake_minimum_required(VERSION 3.3)
project(nuvie-tools)

SET(SDL_VERSION "SDL2")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DLUA_USE_MACOSX -DUNIX -DMACOSX -DHAVE_JOYSTICK_SUPPORT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_MACOSX -DMACOSX -DHAVE_JOYSTICK_SUPPORT")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DLUA_USE_LINUX -DUNIX -DHAVE_JOYSTICK_SUPPORT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX -DHAVE_JOYSTICK_SUPPORT")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(CMAKE_VERBOSE_MAKEFILE ON)
INCLUDE(FindPkgConfig)

IF (${SDL_VERSION} MATCHES "SDL2")
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
    SET(SDL_LIBS "${SDL2_LIBRARIES}")
ELSE ()
    PKG_SEARCH_MODULE(SDL REQUIRED sdl)
    INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})
    SET(SDL_LIBS "${SDL_LIBRARIES}")
ENDIF ()

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

include_directories(.)
include_directories(./tools)
include_directories(conf)
include_directories(misc)

add_executable(pack_lzc ${pack_lzc_SOURCE_FILES})
add_executable(unpack_lzc ${unpack_lzc_SOURCE_FILES})
add_executable(unpack_font ${unpack_font_SOURCE_FILES})
add_executable(pack_font ${pack_font_SOURCE_FILES})

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    TARGET_LINK_LIBRARIES(unpack_font ${SDL_LIBS} -Wl,-framework,Cocoa)
    TARGET_LINK_LIBRARIES(pack_font ${SDL_LIBS} -Wl,-framework,Cocoa)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    TARGET_LINK_LIBRARIES(unpack_font dl ${SDL_LIBS})
    TARGET_LINK_LIBRARIES(pack_font dl ${SDL_LIBS})
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
