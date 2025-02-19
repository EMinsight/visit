# Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
# Project developers.  See the top-level LICENSE file for dates and other
# details.  No copyright assignment is required to contribute to VisIt.

#****************************************************************************
# Modifications:
#   Kathleen Bonnell, Wed Jan 05 10:25:13 PST 2010
#   Add FOLDER property.
#
#   Kathleen Biagas, Thu Jun  7 14:45:49 PDT 2018
#   Remove use of VTK_DEFINTIONS in compiler. It initializes some vtk
#   modules, which is now being done in avt/Plotter/vtk/InitVTKRendering.C.
#
#   Kathleen Biagas, Tue Sep 5, 2023 
#   Remove untested, unused EGL logic.
#
#****************************************************************************

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${VISIT_SOURCE_DIR}/common/utility
${VTK_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${LIBRARY_OUTPUT_DIRECTORY} )

ADD_EXECUTABLE(text2polys text2polys.C arial_font.C)
ADD_EXECUTABLE(time_annotation time_annotation.C arial_font.C)

# Specify the libraries to link against
TARGET_LINK_LIBRARIES(text2polys VTK::IOLegacy)

TARGET_LINK_LIBRARIES(time_annotation
        VTK::RenderingOpenGL2 VTK::RenderingFreeType VTK::InteractionStyle
        VTK::vtksys VTK::IOImage)

if(HAVE_OSMESA)
    target_link_libraries(time_annotation ${OSMESA_LIBRARIES})
endif()

VISIT_INSTALL_TARGETS(text2polys time_annotation)
VISIT_FX_ADD_FOLDER(annotations text2polys time_annotation)
