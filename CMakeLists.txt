##---------------------------------------------------------------------------
## Author:      nohal aka. Pavel Kalian
## Copyright:
## License:     wxWidgets License
##---------------------------------------------------------------------------

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(aisradar_pi)

SET(CMAKE_MACOSX_RPATH "ON")

SET(PACKAGE_NAME aisradar_pi)
SET(VERBOSE_NAME AISRadar)
SET(TITLE_NAME AISRadar)
SET(CPACK_PACKAGE_CONTACT "Johan Sman")
SET(VERSION_MINOR "2")
SET(VERSION_PATCH "1")
SET(VERSION_DATE "2020-07-17")
SET(OCPN_VERSION "ov50")

#SET(CMAKE_BUILD_TYPE Debug)

INCLUDE("cmake/PluginConfigure.cmake")

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
    )
ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_AISRADAR})

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
