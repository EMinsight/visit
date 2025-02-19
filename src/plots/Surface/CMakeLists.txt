# DO NOT EDIT THIS FILE! THIS FILE IS AUTOMATICALLY GENERATED BY xml2cmake
PROJECT(Surface_plot)

ADD_PLOT_CODE_GEN_TARGETS(Surface)

SET(COMMON_SOURCES
SurfacePluginInfo.C
SurfaceCommonPluginInfo.C
SurfaceAttributes.C
)

SET(LIBI_SOURCES
SurfacePluginInfo.C
)

SET(LIBG_SOURCES
SurfaceGUIPluginInfo.C
QvisSurfacePlotWindow.C
${COMMON_SOURCES}
)

SET(LIBV_SOURCES
SurfaceViewerEnginePluginInfo.C
SurfaceViewerPluginInfo.C
avtSurfacePlot.C
avtSurfaceMapper.C
${COMMON_SOURCES}
)

SET(LIBE_SOURCES
SurfaceViewerEnginePluginInfo.C
SurfaceEnginePluginInfo.C
avtSurfacePlot.C
avtSurfaceMapper.C
${COMMON_SOURCES}
)


INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${VISIT_PLOT_INCLUDES}
)

LINK_DIRECTORIES(${VISIT_LIBRARY_DIR} )

ADD_LIBRARY(ISurfacePlot ${LIBI_SOURCES})
TARGET_LINK_LIBRARIES(ISurfacePlot visitcommon)
SET(INSTALLTARGETS ISurfacePlot)

IF(NOT VISIT_SERVER_COMPONENTS_ONLY AND NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)
    ADD_LIBRARY(GSurfacePlot ${LIBG_SOURCES})
    set_target_properties(GSurfacePlot PROPERTIES AUTOMOC ON)
    TARGET_LINK_LIBRARIES(GSurfacePlot visitcommon gui )

    ADD_LIBRARY(VSurfacePlot ${LIBV_SOURCES})
    ADD_TARGET_DEFINITIONS(VSurfacePlot VIEWER)
    TARGET_LINK_LIBRARIES(VSurfacePlot visitcommon viewer )

    SET(INSTALLTARGETS ${INSTALLTARGETS} GSurfacePlot VSurfacePlot)

    IF(VISIT_PYTHON_SCRIPTING)
        SET(LIBS_SOURCES
            SurfaceScriptingPluginInfo.C
            PySurfaceAttributes.C
            ${COMMON_SOURCES}
        )
        ADD_LIBRARY(SSurfacePlot ${LIBS_SOURCES})
        IF(WIN32)
            # This prevents python from #defining snprintf as _snprintf
            SET_TARGET_PROPERTIES(SSurfacePlot PROPERTIES COMPILE_DEFINITIONS HAVE_SNPRINTF)
        ENDIF()
        TARGET_LINK_LIBRARIES(SSurfacePlot visitcommon visitpy ${PYTHON_LIBRARY})
        SET(INSTALLTARGETS ${INSTALLTARGETS} SSurfacePlot)
    ENDIF(VISIT_PYTHON_SCRIPTING)

    IF(VISIT_JAVA)
        FILE(COPY SurfaceAttributes.java DESTINATION ${JavaClient_BINARY_DIR}/src/plots)
        ADD_CUSTOM_TARGET(JavaSurface ALL ${Java_JAVAC_EXECUTABLE} ${VISIT_Java_FLAGS} -d ${JavaClient_BINARY_DIR} -classpath ${JavaClient_BINARY_DIR} -sourcepath ${JavaClient_BINARY_DIR} SurfaceAttributes.java
            DEPENDS JavaClient
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    ENDIF(VISIT_JAVA)
ENDIF(NOT VISIT_SERVER_COMPONENTS_ONLY AND NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)

ADD_LIBRARY(ESurfacePlot_ser ${LIBE_SOURCES})
TARGET_LINK_LIBRARIES(ESurfacePlot_ser visitcommon avtpipeline_ser avtplotter_ser )
ADD_TARGET_DEFINITIONS(ESurfacePlot_ser ENGINE)
SET(INSTALLTARGETS ${INSTALLTARGETS} ESurfacePlot_ser)

IF(VISIT_PARALLEL)
    ADD_PARALLEL_LIBRARY(ESurfacePlot_par ${LIBE_SOURCES})
    TARGET_LINK_LIBRARIES(ESurfacePlot_par visitcommon avtpipeline_par avtplotter_par )
    ADD_TARGET_DEFINITIONS(ESurfacePlot_par ENGINE)
    SET(INSTALLTARGETS ${INSTALLTARGETS} ESurfacePlot_par)
ENDIF(VISIT_PARALLEL)

VISIT_INSTALL_PLOT_PLUGINS(${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_OUTPUT_DIR(plots ${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_FOLDER(plots Surface ${INSTALLTARGETS})

