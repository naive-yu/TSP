cmake_minimum_required(VERSION 3.16)

project(TSP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(CMAKE_AUTOUIC_SEARCH_PATHS
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/ui
)
message(STATUS "AUTOUIC will search: ${CMAKE_AUTOUIC_SEARCH_PATHS}")

set(TS_FILES ${CMAKE_SOURCE_DIR}/ts/TSP_zh_CN.ts)

set(PROJECT_SOURCES
  src/main.cpp
  src/include/TSPWindow.h
  src/TSPWindow.cpp
  src/ui/TSPWindow.ui
  src/ui/Dialog.ui
  config/tsp_config.ini
  ${TS_FILES}
  src/include/Config.h
  src/Config.cpp
  src/include/Algorithm.h
  src/Algorithm.cpp
  src/Ant_colony.cpp
  src/include/Ant_colony.h
  src/include/resource.h
  src/include/Genetic.h
  src/Genetic.cpp
  src/include/Dialog.h
  src/Dialog.cpp
  src/include/Particle.h
  src/Particle.cpp
  src/include/Helper.h
  src/Helper.cpp
)

set(TSP_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
# NOTE: prefer target_include_directories for modern CMake. The target is defined later,
# so we'll add the include dir to the target after target creation.

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(TSP
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
  # Define target properties for Android with Qt 6 as:
  #    set_property(TARGET TSP APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
  # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

  qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
  if(ANDROID)
    add_library(TSP SHARED
      ${PROJECT_SOURCES}
    )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(TSP
      ${PROJECT_SOURCES}
    )
  endif()

  qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(TSP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Add project include directory to the target (modern CMake)
target_include_directories(TSP PRIVATE ${TSP_SRC_INCLUDE_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TSP)
endif()
set_target_properties(TSP PROPERTIES
  ${BUNDLE_ID_OPTION}
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TSP
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(TSP)
endif()
