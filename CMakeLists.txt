cmake_minimum_required(VERSION 3.20)

set(OSGEO4W_ROOT "D:/cpp/OSGeo4W" CACHE PATH "OSGeo4W root directory")
set(LOG4CPP_ROOT "D:/cpp/log4cpp-1.1.4" CACHE PATH "log4cpp root directory")
set(LIBXML2_ROOT "D:/cpp/libxml2" CACHE PATH "libxml2 root directory")

foreach(_required_root IN ITEMS OSGEO4W_ROOT LOG4CPP_ROOT LIBXML2_ROOT)
    if(NOT DEFINED ${_required_root} OR "${${_required_root}}" STREQUAL "")
        message(FATAL_ERROR "${_required_root} not set. Please provide the absolute path.")
    endif()
endforeach()

set(OSGEO4W_QT5_ROOT "${OSGEO4W_ROOT}/apps/Qt5")
set(OSGEO4W_GDAL_ROOT "${OSGEO4W_ROOT}/apps/gdal-dev")
set(OSGEO4W_QGIS_ROOT "${OSGEO4W_ROOT}/apps/qgis-ltr")
set(LOG4CPP_INCLUDE_DIR "${LOG4CPP_ROOT}/include")
set(LOG4CPP_DEBUG_LIB_DIR "${LOG4CPP_ROOT}/debug")
set(LOG4CPP_RELEASE_LIB_DIR "${LOG4CPP_ROOT}/release")
set(LIBXML2_INCLUDE_DIR "${LIBXML2_ROOT}/include")
set(LIBXML2_LIB_DIR "${LIBXML2_ROOT}/lib")

list(PREPEND CMAKE_PREFIX_PATH "${OSGEO4W_QT5_ROOT}")

project(SpatialAnalysisEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Force MSVC to use UTF-8 encoding
if(MSVC)
    add_compile_options("/utf-8")
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network Xml Svg Sql PrintSupport Charts)

add_executable(SpatialAnalysisEngine
    main.cpp
    core/AppController.cpp
    core/AppController.h
    model/DataModel.cpp
    model/DataModel.h
    service/AnalysisModule.cpp
    service/AnalysisModule.h
    service/AnalysisStrategy.h
    service/BasicStatStrategy.cpp
    service/BasicStatStrategy.h
    service/DataModule.cpp
    service/DataModule.h
    service/FreqStatStrategy.cpp
    service/FreqStatStrategy.h
    service/NeighborStatStrategy.cpp
    service/NeighborStatStrategy.h
    ui/MainWindow.cpp
    ui/MainWindow.h
    view/ChartView.cpp
    view/ChartView.h
    controller/VisualizationManager.cpp
    controller/VisualizationManager.h
    
    # LLM subsystem
    service/llm/llminterface.h
    service/llm/mockllm.cpp
    service/llm/mockllm.h
    service/llm/promptmanager.cpp
    service/llm/promptmanager.h
    service/llm/responseparser.cpp
    service/llm/responseparser.h
    service/llm/llmservice.cpp
    service/llm/llmservice.h
    service/llm/deepseekllm.h
    service/llm/deepseekllm.cpp)

target_include_directories(SpatialAnalysisEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/service
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/view
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
    ${OSGEO4W_GDAL_ROOT}/include
    ${OSGEO4W_QGIS_ROOT}/include
    ${LOG4CPP_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

target_link_directories(SpatialAnalysisEngine PRIVATE
    ${OSGEO4W_GDAL_ROOT}/lib
    ${OSGEO4W_QGIS_ROOT}/lib
    ${LIBXML2_LIB_DIR}
    ${OSGEO4W_QT5_ROOT}/lib
    ${OSGEO4W_ROOT}/lib
    $<$<CONFIG:Debug>:${LOG4CPP_DEBUG_LIB_DIR}>
    $<$<CONFIG:Release>:${LOG4CPP_RELEASE_LIB_DIR}>
)

target_link_libraries(SpatialAnalysisEngine PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Xml
    Qt5::Charts
    qgis_core
    qgis_gui
    gdal_i
    log4cpp
    libxml2
    iconv
)

# Set runtime PATH for debugging in Visual Studio
if(MSVC)
    set_property(TARGET SpatialAnalysisEngine PROPERTY VS_DEBUGGER_ENVIRONMENT 
        "PATH=${OSGEO4W_ROOT}/apps/gdal-dev/bin;${OSGEO4W_ROOT}/apps/Qt5/bin;${OSGEO4W_ROOT}/bin;${OSGEO4W_ROOT}/apps/qgis-ltr/bin;%PATH%"
    )
    
    # Copy required DLLs to output directory after build
    add_custom_command(TARGET SpatialAnalysisEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying runtime DLLs..."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OSGEO4W_ROOT}/bin/*.dll"
            "$<TARGET_FILE_DIR:SpatialAnalysisEngine>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OSGEO4W_ROOT}/apps/gdal-dev/bin/*.dll"
            "$<TARGET_FILE_DIR:SpatialAnalysisEngine>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OSGEO4W_ROOT}/apps/Qt5/bin/Qt5*.dll"
            "$<TARGET_FILE_DIR:SpatialAnalysisEngine>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OSGEO4W_ROOT}/apps/qgis-ltr/bin/*.dll"
            "$<TARGET_FILE_DIR:SpatialAnalysisEngine>"
        COMMENT "Copying DLL dependencies to output directory"
    )
endif()