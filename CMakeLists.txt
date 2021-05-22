##---------------------------------------------------------------------------
## Author:    Johan van der Sman      hannes@andcrew.nl           
## License:   GNUGLv3
## License:   wxWidgets License
## ---------------------------------------------------------------------------
## Frontend Author: Jon Gough     Frontend2 Version: 1.0.165
## ---------------------------------------------------------------------------
## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##
## ----- Modify section below to include all the details for your plugin ----- ##
##

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.1.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)

# define plugin name, owner and versions
set(VERBOSE_NAME "AISradar")
set(COMMON_NAME "AIS Radar view")
set(TITLE_NAME "AISradar")
set(PACKAGE_CONTACT "Johan Sman")
set(PACKAGE "aisradar")
set(SHORT_DESCRIPTION "AIS Radar view Plugin")
set(LONG_DESCRIPTION "Radar PlugIn for OpenCPN Shows AIS targets in a radar style view.")
set(VERSION_MAJOR "1")
set(VERSION_MINOR "2")
set(VERSION_PATCH "6")
set(VERSION_TWEAK "6")
set(VERSION_DATE "22-05-2021")  #dd-mm-yyyy
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "  * Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows setup of a local webserver with git for testing purposes.
# The default is shown below, but it is done in cmake/pluginconfigure.cmake now.
# This default is needed to build locally and to build flatpak.
#set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith repository suffixes
set(PROD "prod") # Standard Repos
set(BETA "beta") # Standard Repos
set(ALPHA "alpha") # Standard Repos

# set this if your cloudsmith repository name does not match your git repository name set(CLOUDSMITH_BASE_REPOSITORY "testplugin_pi")
set (CLOUDSMITH_BASE_REPOSITORY "aisradar") 

set(CLOUDSMITH_USER "opencpn")

set(XML_INFO_URL "https://opencpn.org/OpenCPN/plugins/aisradarview.html")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

## ----- Modify section above to include all the details for your plugin ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##

option(PLUGIN_USE_SVG "Use SVG graphics" OFF)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MACOSX_RPATH "ON")
# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
set(USE_LOCAL_GLU FALSE)

option(USE_GL "Enable OpenGL support" OFF)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##

project(${PACKAGE})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")

#===================================
include("PluginSetup")
#===================================
## ----- The above is used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##


set(CMAKE_VERBOSE_MAKEFILE ON)

option(Plugin_CXX11 "Use c++11" OFF)

## ----- Modify section above if there are special requirements for the plugin ----- ##
## ----- Do not change next section - needed to configure build process ----- ##

#===================================
include("PluginConfigure")
#===================================
## ----- Do not change section above - needed to configure build process ----- ##
## ----- Change below to match project requirements for source, headers, etc. ----- ##


add_definitions(-DUSE_S57)

SET(SRC_AISRADAR
      src/aisradar_pi.h
      src/aisradar_pi.cpp
      src/aisview.h
      src/aisview.cpp
      src/icons.h
      src/icons.cpp
      src/Canvas.h
      src/Canvas.cpp
      src/Target.h
      src/Target.cpp
#	  api-16/ocpn_plugin.h
    )

# add_definitions(-DPLUGIN_USE_SVG)

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/ocpn-api)

#===================================
target_sources(${PACKAGE_NAME} PUBLIC ${SRC_AISRADAR})
#===================================
# add_library(${PACKAGE_NAME} SHARED ${SRC_AISRADAR})


#if(NOT OCPN_FLATPAK_CONFIG)
    # Build environment not available when flatpak is being configured so following statements will not work
    message(STATUS "${CMLOC}Adding target link libraries to ${PACKAGE_NAME}")

#    add_subdirectory(libs/tinyxml)

#    target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)
#endif(NOT OCPN_FLATPAK_CONFIG)


add_definitions(-DTIXML_USE_STL)

## ----- Change above to match project requirements ----- ##
## ----- do not change next section - needed to configure build process ----- ##

# Needed for android builds
if(QT_ANDROID)
    include_directories(BEFORE ${qt_android_include})
endif(QT_ANDROID)

# Needed for all builds
#===================================
include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")
#===================================
## ----- do not change section above - needed to configure build process ----- ##

message(STATUS "${CMLOC}include directories: ")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "${CMLOC}dir='${dir}'")
endforeach()
