# ---------------------------------------------------------------------------
# Author:      Jon Gough Copyright:   2020 License:     GPLv3+
# ---------------------------------------------------------------------------

# This file contains changes needed during the make package process depending on the type of package being created

if(CPACK_GENERATOR MATCHES "DEB")
    set(CPACK_PACKAGE_FILE_NAME "aisradar_pi-1.2.6.0-ubuntu-20.04")
    if(CPACK_DEBIAN_PACKAGE_ARCHITECTURE MATCHES "x86_64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
    endif()
else()
    set(CPACK_PACKAGE_FILE_NAME "aisradar_pi-1.2.6.0-ubuntu-x86_64-20.04")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE x86_64)
endif()
