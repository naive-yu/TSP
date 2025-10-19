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
  src/include/AlgoExecuter.h
  src/AlgoExecuter.cpp
  src/include/AlgorithmWorker.h
  src/AlgorithmWorker.cpp
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

# ==================== 打包相关配置开始 ====================

# 查找 windeployqt 工具 (Windows)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt 
    HINTS ${QT_DIR}/bin ${CMAKE_PREFIX_PATH}/bin
    DOC "Qt Windows deployment tool"
)

# 查找 linuxdeployqt 工具 (Linux)
find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt
    DOC "Linux deployment tool for Qt applications"
)

# 查找 macdeployqt 工具 (macOS)
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
    HINTS ${QT_DIR}/bin ${CMAKE_PREFIX_PATH}/bin
    DOC "Qt macOS deployment tool"
)

# 创建部署目录
set(DEPLOY_DIR ${CMAKE_BINARY_DIR}/deploy)
set(INSTALLER_DIR ${CMAKE_BINARY_DIR}/installer)

# 添加部署目标
if(WINDEPLOYQT_EXECUTABLE AND WIN32)
    add_custom_target(deploy
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}/config"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TSP> "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/config/tsp_config.ini" "${DEPLOY_DIR}/config"
        COMMAND ${WINDEPLOYQT_EXECUTABLE} 
                --compiler-runtime 
                --no-translations 
                --no-system-d3d-compiler 
                --no-opengl-sw 
                "${DEPLOY_DIR}/$<TARGET_FILE_NAME:TSP>"
        COMMENT "Deploying TSP application with Qt dependencies..."
        DEPENDS TSP
    )
elseif(MACDEPLOYQT_EXECUTABLE AND APPLE)
    add_custom_target(deploy
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}/config"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_BUNDLE_DIR:TSP>" "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/config/tsp_config.ini" "${DEPLOY_DIR}/config"
        COMMAND ${MACDEPLOYQT_EXECUTABLE} "${DEPLOY_DIR}/$<TARGET_BUNDLE_CONTENT_DIR:TSP>/.." 
                -always-overwrite
        COMMENT "Deploying TSP application for macOS..."
        DEPENDS TSP
    )
elseif(LINUXDEPLOYQT_EXECUTABLE AND UNIX AND NOT APPLE)
    add_custom_target(deploy
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}/config"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TSP> "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/config/tsp_config.ini" "${DEPLOY_DIR}/config"
        COMMAND ${LINUXDEPLOYQT_EXECUTABLE} 
                "${DEPLOY_DIR}/$<TARGET_FILE_NAME:TSP>" 
                -appimage
                -always-overwrite
        COMMENT "Deploying TSP application for Linux..."
        DEPENDS TSP
    )
endif()

# 创建 ZIP 打包目标
add_custom_target(package
    COMMAND ${CMAKE_COMMAND} -E tar cf "${CMAKE_BINARY_DIR}/TSP-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}.zip" 
            --format=zip 
            "${DEPLOY_DIR}"
    DEPENDS deploy
    COMMENT "Creating distribution package..."
)

# 添加资源文件安装
install(FILES 
    ${CMAKE_SOURCE_DIR}/config/tsp_config.ini
    DESTINATION config
)

# 添加翻译文件安装
install(FILES 
    ${QM_FILES}
    DESTINATION translations
)

# 创建便携式版本目标
add_custom_target(portable
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/config/tsp_config.ini" 
            "${CMAKE_BINARY_DIR}/$<TARGET_FILE_NAME:TSP>.ini"
    COMMENT "Creating portable version configuration..."
    DEPENDS TSP
)

# 添加清理部署文件的目标
add_custom_target(clean_deploy
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${DEPLOY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALLER_DIR}"
    COMMENT "Cleaning deployment files..."
)

# ==================== 打包相关配置结束 ====================

include(GNUInstallDirs)
install(TARGETS TSP
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(TSP)
endif()

# 显示打包相关信息
message(STATUS "Packaging targets available:")
message(STATUS "  make deploy      - Deploy application with Qt dependencies")
message(STATUS "  make package     - Create distribution package")
if(ISCC_EXECUTABLE)
    message(STATUS "  make create_installer - Create Windows installer")
endif()
message(STATUS "  make portable    - Create portable version")
message(STATUS "  make clean_deploy - Clean deployment files")
