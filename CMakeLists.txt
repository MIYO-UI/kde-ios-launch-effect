cmake_minimum_required(VERSION 3.16)

project(kwin-ioslauncheffect)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find includes in the build directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find KDE packages
find_package(ECM 5.82.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMInstallIcons)

# Find the required libraries
find_package(KF5 REQUIRED COMPONENTS 
    Config
    ConfigWidgets
    GlobalAccel
    I18n
    XmlGui
)

# Find KWin
find_package(KWin REQUIRED)

# Source files for the effect
set(ioslauncheffect_SOURCES
    ioslauncheffect.cpp
    main.cpp
)

# Source files for the configuration module
set(ioslauncheffect_config_SOURCES
    ioslauncheffect_config.cpp
)
ki18n_wrap_ui(ioslauncheffect_config_SOURCES ioslauncheffect_config.ui)

# Build the KWin effect
kwin_add_effect(ioslauncheffect ${ioslauncheffect_SOURCES})
target_link_libraries(ioslauncheffect
    PUBLIC
        KF5::ConfigCore
        KF5::ConfigGui
        KF5::I18n
)

# Build the configuration module
add_library(kwin_ioslauncheffect_config MODULE ${ioslauncheffect_config_SOURCES})
target_link_libraries(kwin_ioslauncheffect_config
    PUBLIC
        KF5::ConfigWidgets
        KF5::I18n
        KWinEffects
)

# Install the config file
kconfig_add_kcfg_files(ioslauncheffect ioslauncheffect.kcfgc)

# Install everything
install(TARGETS ioslauncheffect DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins/)
install(TARGETS kwin_ioslauncheffect_config DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/configs/)
install(FILES ioslauncheffect.desktop DESTINATION ${KDE_INSTALL_DATADIR}/kwin/effects/)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)