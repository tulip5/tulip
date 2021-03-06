ADD_CORE_FILES(".")

INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${CMAKE_CURRENT_BINARY_DIR})

##----------------------------------------------------------------------------------------------------------------------------
SET(Planarity_SRCS
    Planarity.cpp
)

ADD_LIBRARY(Planarity-${TulipVersion} SHARED ${Planarity_SRCS})
TARGET_LINK_LIBRARIES(Planarity-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Planarity-${TulipVersion} ${TulipPluginsInstallDir})


##----------------------------------------------------------------------------------------------------------------------------
SET(Simple_SRCS
Simple.cpp
)

ADD_LIBRARY(Simple-${TulipVersion} SHARED ${Simple_SRCS})
TARGET_LINK_LIBRARIES(Simple-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Simple-${TulipVersion} ${TulipPluginsInstallDir})

##----------------------------------------------------------------------------------------------------------------------------
SET(Tree_SRCS
Tree.cpp
)

ADD_LIBRARY(Tree-${TulipVersion} SHARED ${Tree_SRCS})
TARGET_LINK_LIBRARIES(Tree-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Tree-${TulipVersion} ${TulipPluginsInstallDir})

##----------------------------------------------------------------------------------------------------------------------------
SET(Connected_SRCS
Connected.cpp
)

ADD_LIBRARY(Connected-${TulipVersion} SHARED ${Connected_SRCS})
TARGET_LINK_LIBRARIES(Connected-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Connected-${TulipVersion} ${TulipPluginsInstallDir})

##----------------------------------------------------------------------------------------------------------------------------
SET(Biconnected_SRCS
Biconnected.cpp
)

ADD_LIBRARY(Biconnected-${TulipVersion} SHARED ${Biconnected_SRCS})
TARGET_LINK_LIBRARIES(Biconnected-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Biconnected-${TulipVersion} ${TulipPluginsInstallDir})

##----------------------------------------------------------------------------------------------------------------------------
SET(Triconnected_SRCS
Triconnected.cpp
)

ADD_LIBRARY(Triconnected-${TulipVersion} SHARED ${Triconnected_SRCS})
TARGET_LINK_LIBRARIES(Triconnected-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Triconnected-${TulipVersion} ${TulipPluginsInstallDir})

##----------------------------------------------------------------------------------------------------------------------------
SET(Outerplanar_SRCS
Outerplanar.cpp
)

ADD_LIBRARY(Outerplanar-${TulipVersion} SHARED ${Outerplanar_SRCS})
TARGET_LINK_LIBRARIES(Outerplanar-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Outerplanar-${TulipVersion} ${TulipPluginsInstallDir})

##----------------------------------------------------------------------------------------------------------------------------
SET(Acyclic_SRCS
Acyclic.cpp
)

ADD_LIBRARY(Acyclic-${TulipVersion} SHARED ${Acyclic_SRCS})
TARGET_LINK_LIBRARIES(Acyclic-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
TULIP_INSTALL_PLUGIN(Acyclic-${TulipVersion} ${TulipPluginsInstallDir})
