# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


MESSAGE(STATUS "This is XUtil PROJECT BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is XUtil CMAKE BINARY dir " ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "This is XUtil PROJECT SOURCE dir " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is XUtil CMAKE SOURCE dir " ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "This is XUtil current BINARY dir " ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "This is XUtil current SOURCE dir " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "This is XUtil current CMAKE SOURCE dir " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "This is XUtil current CMakeList.txt dir " ${CMAKE_CURRENT_LIST_DIR})
MESSAGE(STATUS "This is XUtil current CMakeList.txt line " ${CMAKE_CURRENT_LIST_LINE})

MESSAGE(STATUS "This is XUtil library output " ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

ADD_DEFINITIONS(-std=c++17)

if(BOOST_ROOT)
set(Boost_INCLUDE_DIRS ${BOOST_ROOT}/bcpout)
else()
set(Boost_INCLUDE_DIRS ../../boost_1_66_0/bcpout)
endif()
include_directories(${Boost_INCLUDE_DIRS} ../../local ../include)

#添加库文件搜索路径
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/bin)

#aux_source_directory(. libSRC_LIST)
set(libSRC_LIST ${libSRC_LIST} 
XApi.cpp
)

#设置输出位置
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_library( # Sets the name of the library.
        XUtil

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${libSRC_LIST}
        )
TARGET_LINK_LIBRARIES(XUtil boost)
