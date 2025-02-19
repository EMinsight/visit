// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                                avtExprNode.C                              //
// ************************************************************************* //

#include <ExpressionException.h>
#include <ExprToken.h>
#include <ExprPipelineState.h>
#include <avtExprNode.h>
#include <DebugStream.h>

#include <avtApplyDataBinningExpression.h>
#include <avtApplyEnumerationExpression.h>
#include <avtApplyMapExpression.h>
#include <avtArrayComposeExpression.h>
#include <avtArrayComposeWithBinsExpression.h>
#include <avtArrayDecomposeExpression.h>
#include <avtArrayDecompose2DExpression.h>
#include <avtArrayComponentwiseDivisionExpression.h>
#include <avtArrayComponentwiseProductExpression.h>
#include <avtArraySumExpression.h>
#include <avtBinExpression.h>
#include <avtBinaryAddExpression.h>
#include <avtBinaryAndExpression.h>
#include <avtBinaryDivideExpression.h>
#include <avtBinaryModuloExpression.h>
#include <avtBinaryMultiplyExpression.h>
#include <avtBinaryPowerExpression.h>
#include <avtBinarySubtractExpression.h>
#include <avtColorComposeExpression.h>
#include <avtConnComponentsExpression.h>
#include <avtConstantCreatorExpression.h>
#include <avtConstantFunctionExpression.h>
#include <avtCrackWidthExpression.h>
#include <avtCurvatureExpression.h>
#include <avtCurveDomainExpression.h>
#include <avtCurveExpression.h>
#include <avtCurveIntegrateExpression.h>
#include <avtCurveSwapXYExpression.h>
#include <avtCylindricalCoordinatesExpression.h>
#include <avtDegreeExpression.h>
#include <avtDisplacementExpression.h>
#include <avtDistanceToBestFitLineExpression.h>
#include <avtGeodesicVectorQuantizeExpression.h>
#include <avtGradientExpression.h>
#include <avtHSVColorComposeExpression.h>
#include <avtIsNaNExpression.h>
#include <avtKeyAggregatorExpression.h>
#include <avtLambda2Expression.h>
#include <avtLaplacianExpression.h>
#include <avtLocalizedCompactnessExpression.h>
#include <avtMinMaxExpression.h>
#include <avtPerformColorTableLookupExpression.h>
#include <avtProcessorIdExpression.h>
#include <avtThreadIdExpression.h>
#include <avtQCriterionExpression.h>
#include <avtRecenterExpression.h>
#include <avtRectilinearLaplacianExpression.h>
#include <avtRelativeDifferenceExpression.h>
#include <avtResampleExpression.h>
#include <avtResradExpression.h>
#include <avtTimeExpression.h>
#include <avtUnaryMinusExpression.h>
#include <avtVariableSkewExpression.h>
#include <avtVectorComposeExpression.h>
#include <avtVectorDecomposeExpression.h>
#include <avtMergeTreeExpression.h>
#include <avtLocalThresholdExpression.h>

#include <visit-python-config.h>
#ifdef VISIT_PYTHON_FILTERS
#include <avtPythonExpression.h>
#endif

#include <stdio.h>

#include <string>
#include <vector>

using std::string;

// ****************************************************************************
// Method: avtIntegerConstExpr::CreateFilters
//
// Purpose:
//     Creates the avt filters that are necessary to complete the given
//     constant
//
// Programmer: Jeremy Meredith
// Creation:   June 13, 2005
//
// Note:  taken roughly from the old ConstExpr::CreateFilters
//
// Modifications:
//
// ****************************************************************************
void
avtIntegerConstExpr::CreateFilters(ExprPipelineState *state)
{
    avtConstantCreatorExpression *f = new avtConstantCreatorExpression();
    f->SetValue(value);
    char strrep[30];
    snprintf(strrep, 30, "'%d'", value);
    state->PushName(string(strrep));
    f->SetOutputVariableName(strrep);

    // Keep track of the current dataObject.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

// ****************************************************************************
// Method: avtFloatConstExpr::CreateFilters
//
// Purpose:
//     Creates the avt filters that are necessary to complete the given
//     constant
//
// Programmer: Jeremy Meredith
// Creation:   June 13, 2005
//
// Note:  taken roughly from the old ConstExpr::CreateFilters
//
// Modifications:
//
// ****************************************************************************
void
avtFloatConstExpr::CreateFilters(ExprPipelineState *state)
{
    avtConstantCreatorExpression *f = new avtConstantCreatorExpression();
    f->SetValue(value);
    char strrep[30];
    snprintf(strrep, 30, "'%e'", value);
    state->PushName(string(strrep));
    f->SetOutputVariableName(strrep);

    // Keep track of the current dataObject.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

// ****************************************************************************
// Method: avtStringConstExpr::CreateFilters
//
// Purpose:
//     Issue an error -- we don't support string constants like this.
//
// Programmer: Jeremy Meredith
// Creation:   June 13, 2005
//
// Modifications:
//
// ****************************************************************************
void
avtStringConstExpr::CreateFilters(ExprPipelineState *state)
{
    EXCEPTION1(ExpressionParseException,
               "avtStringConstExpr::CreateFilters: "
               "Unsupported constant type: String");
}

// ****************************************************************************
// Method: avtBooleanConstExpr::CreateFilters
//
// Purpose:
//     Issue an error -- we don't support boolean constants like this.
//
// Programmer: Jeremy Meredith
// Creation:   June 13, 2005
//
// Modifications:
//
// ****************************************************************************
void
avtBooleanConstExpr::CreateFilters(ExprPipelineState *state)
{
    EXCEPTION1(ExpressionParseException,
               "avtStringConstExpr::CreateFilters: "
               "Unsupported constant type: Bool");
}


void
avtUnaryExpr::CreateFilters(ExprPipelineState *state)
{
    dynamic_cast<avtExprNode*>(expr)->CreateFilters(state);

    avtSingleInputExpressionFilter *f = NULL;
    if (op == '-')
        f = new avtUnaryMinusExpression();
    else
    {
        string error =
            string("avtUnaryExpr::CreateFilters: "
                   "Unknown unary operator:\"") + op + string("\".");
        EXCEPTION1(ExpressionParseException, error);
    }

    // Set the variable the function should process.
    string inputName = state->PopName();
    f->AddInputVariableName(inputName.c_str());

    // Set the variable the function should output.
    string outputName = string() + op + "(" + inputName + ")";
    state->PushName(outputName);
    f->SetOutputVariableName(outputName.c_str());

    // Keep track of the current dataObject.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

void
avtBinaryExpr::CreateFilters(ExprPipelineState *state)
{
    dynamic_cast<avtExprNode*>(left)->CreateFilters(state);
    dynamic_cast<avtExprNode*>(right)->CreateFilters(state);

    avtMultipleInputExpressionFilter *f = NULL;
    if (op == '+')
        f = new avtBinaryAddExpression();
    else if (op == '-')
        f = new avtBinarySubtractExpression();
    else if (op == '*')
        f = new avtBinaryMultiplyExpression();
    else if (op == '/')
        f = new avtBinaryDivideExpression();
    else if (op == '^')
        f = new avtBinaryPowerExpression();
    else if (op == '&')
        f = new avtBinaryAndExpression();
    else if (op == '%')
        f = new avtBinaryModuloExpression();
    else
    {
        string error =
            string("avtBinaryExpr::CreateFilters: "
                   "Unknown binary operator:\"") + op + string("\".");
        EXCEPTION1(ExpressionParseException, error);
    }

    // Set the variable the function should process.
    string inputName2 = state->PopName();
    string inputName1 = state->PopName();
    f->AddInputVariableName(inputName1.c_str());
    f->AddInputVariableName(inputName2.c_str());

    // Set the variable the function should output>
    string outputName = inputName1 + op + inputName2;
    state->PushName(outputName);
    f->SetOutputVariableName(outputName.c_str());

    // Keep track of the current input.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

void
avtIndexExpr::CreateFilters(ExprPipelineState *state)
{
    dynamic_cast<avtExprNode*>(expr)->CreateFilters(state);

    avtVectorDecomposeExpression *f = new avtVectorDecomposeExpression(ind);

    // Set the variable the function should process.
    string inputName = state->PopName();
    f->AddInputVariableName(inputName.c_str());

    // Set the variable the function should output.
    char value_name[200];
    snprintf(value_name, 200, "%d", ind);
    string outputName = inputName + "[" + value_name + "]";
    state->PushName(outputName);
    f->SetOutputVariableName(outputName.c_str());

    // Keep track of the current dataObject.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

// ****************************************************************************
//  Method:  avtVectorExpr::CreateFilters
//
//  Purpose:
//    Creates the avt filters necessary to evaluate a vector.
//
//  Arguments:
//    state      The pipeline state
//
//  Programmer:  Sean Ahern
//
//  Modifications:
//    Jeremy Meredith, Thu Aug 14 09:52:12 PDT 2003
//    Allow 2D vectors.
//
// ****************************************************************************
void
avtVectorExpr::CreateFilters(ExprPipelineState *state)
{
    dynamic_cast<avtExprNode*>(x)->CreateFilters(state);
    dynamic_cast<avtExprNode*>(y)->CreateFilters(state);
    if (z)
        dynamic_cast<avtExprNode*>(z)->CreateFilters(state);

    avtVectorComposeExpression *f = new avtVectorComposeExpression();

    // Set the variable the function should process.
    string inputName3 = z ? state->PopName() : "";
    string inputName2 = state->PopName();
    string inputName1 = state->PopName();
    f->AddInputVariableName(inputName1.c_str());
    f->AddInputVariableName(inputName2.c_str());
    if (z)
        f->AddInputVariableName(inputName3.c_str());

    // Set the variable the function should output>
    string outputName;
    if (z)
        outputName = string("{") + inputName1 + "," + inputName2 + "," +
                                                             inputName3 + "}";
    else
        outputName = string("{") + inputName1 + "," + inputName2 + "}";

    state->PushName(outputName);
    f->SetOutputVariableName(outputName.c_str());

    // Keep track of the current input.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

// ****************************************************************************
// Method: avtFunctionExpr::CreateFilters
//
// Purpose:
//     Creates the avt filters that are necessary to complete the given
//     function.
//
// Notes:  
//     Moved from public CreateFilters(ExprPipelineState) method, so as to 
//     remove nested-if blocks.  Too many nested blocks cause compile failure 
//     on Win32.
//
// Programmer: Kathleen Bonnell 
// Creation:   November 17, 2006
//
// Modifications:
//
//    Thomas R. Treadway, Tue Dec  5 15:09:08 PST 2006
//    added avtStrainAlmansiExpression, avtStrainGreenLagrangeExpression,
//    avtStrainInfinitesimalExpression, avtStrainRateExpression, and
//    avtDisplacementExpression
//
//    Hank Childs, Fri Dec 22 10:03:40 PST 2006
//    Added eval_point and symm_point.
//
//    Hank Childs, Fri Jan 12 13:45:04 PST 2007
//    Added array_compose_with_bins.
// 
//    Cyrus Harrison, Wed Feb 21 09:37:38 PST 2007
//    Added conn_components
//
//    Brad Whitlock, Mon Apr 23 17:32:14 PST 2007
//    Added color.
//
//    Sean Ahern, Tue May  8 13:11:47 EDT 2007
//    Added atan2.
//
//    Cyrus Harrison, Fri Jun  1 14:43:59 PDT 2007
//    Added contraction and viscous_stress
//
//    Cyrus Harrison, Wed Aug  8 14:15:06 PDT 2007
//    Modified to support new unified gradient expression
//
//    Jeremy Meredith, Thu Aug 30 16:02:01 EDT 2007
//    Added hsvcolor.
//
//    Gunther H. Weber, Wed Jan  9 10:22:55 PST 2008
//    Added colorlookup.
//
//    Cyrus Harrison, Tue Jan 29 08:51:41 PST 2008
//    Added value_for_material (& val4mat alias)
//
//    Hank Childs, Wed Feb 13 11:21:40 PST 2008
//    Make module an alias for mod.
//
//    Jeremy Meredith, Tue Feb 19 14:19:18 EST 2008
//    Added "constant".
//
//    Jeremy Meredith, Wed Feb 20 10:01:27 EST 2008
//    Split "constant" into point_constant and cell_constant.
//
//    Hank Childs, Thu Feb 21 15:48:42 PST 2008
//    Added transpose expression.
//
//    Cyrus Harrison, Wed Apr  2 15:34:20 PDT 2008
//    Added new cylindrical_radius implementation.
//
//    Hank Childs, Thu May  8 09:29:08 PDT 2008
//    Add rectilinear_laplacian.
//
//    Hank Childs, Mon May 19 10:57:10 PDT 2008
//    Added [min|max]_corner_angle.
//
//    Sean Ahern, Tue May 27 15:22:47 EDT 2008
//    Added "exp" function.
//
//    Eric Brugger, Wed Aug  6 17:21:53 PDT 2008
//    Renamed smallest_angle to minimum_angle and largest_angle to
//    maximum_angle.
//
//    Kathleen Bonnell, Tue Nov 18 08:10:04 PST 2008
//    Added curve_cmfe.
//
//    Hank Childs, Mon Dec 29 13:57:53 PST 2008
//    Added dominant_mat.
//
//    Hank Childs, Mon Feb 16 15:00:30 PST 2009
//    Added time iteration expressions.
//
//    Jeremy Meredith, Wed Mar 11 12:14:38 EDT 2009
//    Added "cycle" and "timestep" expressions.
//
//    Cyrus Harrison, Thu Mar 26 17:08:32 PDT 2009
//    Added "key_aggregate" expression.
//
//    Kathleen Bonnell, Mon Apr 27 15:47:49 PDT 2009
//    Added sinh, cosh, tanh.
//
//    Brad Whitlock, Thu May 21 09:20:24 PDT 2009
//    I separated out lots of related functions into methods in another file
//    to make this file easier to compile.
//
//    Cyrus Harrison, Tue Aug 11 10:34:08 PDT 2009
//    Added the "map" expression.
//
//    Cyrus Harrison, Tue Feb  2 14:54:25 PST 2010
//    Added the "python" expression.
//
//    Dave Pugmire, Fri Jul  2 14:22:34 EDT 2010
//    Added the "resample" expression.
//
//    Cyrus Harrison, Wed Jul  7 09:13:09 PDT 2010
//    Added 'zonal_constant' & 'nodal_constant' as aliases for the
//    existing 'cell_constant' & 'point_constant' expressions.
//
//    Hank Childs, Sat Aug 21 13:59:11 PDT 2010
//    Rename apply_ddf to apply_data_binning.
//
//    Cyrus Harrison, Wed Aug 25 16:45:25 PDT 2010
//    Preserve 'apply_ddf' as alias for 'apply_data_binning'.
//
//    Eric Brugger, Mon Aug 20 10:14:23 PDT 2012
//    Added curve_integrate.
//
//    Eric Brugger, Mon Aug 27 11:34:02 PDT 2012
//    Added curve_swapxy.
//
//    Brad Whitlock, Wed Sep 12 17:15:18 PDT 2012
//    Added bin expression.
//
//    Gunther H. Weber, Tue May 13 10:42:05 PDT 2014
//    Added array_sum expression.
//
//    Kevin Griffin, Mon Jul 28 17:25:56 PDT 2014
//    Added q_criterion expression.
//
//    Kevin Griffin, Tue Aug 5 15:01:27 PDT 2014
//    Added lambda2 expression.
//
//    Timo Bremer, Fri Oct 28 09:09:27 PDT 2016
//    Added merge_tree, split_tree and local_threshold.
//
//    Eddie Rusu, Mon Sep 30 14:49:38 PDT 2019
//    Changed MinMax expression creation so that they construct with doMin
//    as a construction parameter.
//
//    Kathleen Biagas, Wed June 15, 2022
//    Added crack_width.
//
// ****************************************************************************

avtExpressionFilter *
avtFunctionExpr::CreateFilters(string functionName)
{
    avtExpressionFilter *f = 0;
    if((f = CreateMathFilters(functionName)) != 0)
        return f;
    if((f = CreateVectorMatrixFilters(functionName)) != 0)
        return f;
    if((f = CreateMeshQualityFilters(functionName)) != 0)
        return f;
    if((f = CreateMeshFilters(functionName)) != 0)
        return f;
    if((f = CreateMaterialFilters(functionName)) != 0)
        return f;
    if((f = CreateConditionalFilters(functionName)) != 0)
        return f;
    if((f = CreateCMFEFilters(functionName)) != 0)
        return f;
    if((f = CreateImageProcessingFilters(functionName)) != 0)
        return f;
    if((f = CreateTimeAndValueFilters(functionName)) != 0)
        return f;

    if (functionName == "enumerate")
        return new avtApplyEnumerationExpression();
    if (functionName == "map")
        return new avtApplyMapExpression();
    if (functionName == "array_componentwise_division")
        return new avtArrayComponentwiseDivisionExpression();
    if (functionName == "array_componentwise_product")
        return new avtArrayComponentwiseProductExpression();
    if (functionName == "array_compose")
        return new avtArrayComposeExpression();
    if (functionName == "array_compose_with_bins")
        return new avtArrayComposeWithBinsExpression();
    if (functionName == "array_decompose")
        return new avtArrayDecomposeExpression();
    if (functionName == "array_decompose2d")
        return new avtArrayDecompose2DExpression();
    if (functionName == "array_sum")
        return new avtArraySumExpression();
    if (functionName == "localized_compactness")
        return new avtLocalizedCompactnessExpression();
    if (functionName == "recenter")
        return new avtRecenterExpression();
    if (functionName == "resample")
        return new avtResampleExpression();
    if (functionName == "displacement")
        return new avtDisplacementExpression();
    if (functionName == "degree")
        return new avtDegreeExpression();
    if (functionName == "cylindrical")
        return new avtCylindricalCoordinatesExpression();
    if (functionName == "procid")
        return new avtProcessorIdExpression();
    if (functionName == "threadid")
        return new avtThreadIdExpression();
    if (functionName == "merge_tree")
        return new avtMergeTreeExpression(true);
    if (functionName == "split_tree")
        return new avtMergeTreeExpression(false);
    if (functionName == "local_threshold")
        return new avtLocalThresholdExpression();
    if (functionName == "crack_width")
        return new avtCrackWidthExpression();
    if (functionName == "python" || functionName == "py")
#ifdef VISIT_PYTHON_FILTERS
        return new avtPythonExpression();
#else
        EXCEPTION1(VisItException,
                   "Cannot execute Python Filter Expression because "
                   "VisIt was build without Python Filter support.");
#endif
    if (functionName == "mean_curvature")
    {
        avtCurvatureExpression *c = new avtCurvatureExpression;
        c->DoGaussCurvature(false);
        return c;
    }
    if (functionName == "gauss_curvature")
    {
        avtCurvatureExpression *c = new avtCurvatureExpression;
        c->DoGaussCurvature(true);
        return c;
    }
    if (functionName == "ijk_gradient" || functionName == "ij_gradient")
    {
        avtGradientExpression *g = new avtGradientExpression();
        g->SetAlgorithm(LOGICAL);
        return g;
    }
    if (functionName == "agrad")
    {
        avtGradientExpression *g = new avtGradientExpression();
        g->SetAlgorithm(NODAL_TO_ZONAL_QUAD_HEX);
        return g;
    }

    if (functionName == "key_aggregate" || functionName == "key_agg")
        return new avtKeyAggregatorExpression;
    if (functionName == "laplacian" || functionName == "Laplacian")
        return new avtLaplacianExpression();
    if (functionName == "rectilinear_laplacian")
        return new avtRectilinearLaplacianExpression();
    if (functionName == "conn_components")
        return new avtConnComponentsExpression();
    if (functionName == "resrad")
        return new avtResradExpression();
    if (functionName == "relative_difference")
        return new avtRelativeDifferenceExpression();
    if (functionName == "var_skew")
        return new avtVariableSkewExpression();
    if (functionName == "apply_data_binning" || functionName == "apply_ddf")
        return new avtApplyDataBinningExpression();

    if (functionName == "distance_to_best_fit_line")
        return new avtDistanceToBestFitLineExpression(true);
    if (functionName == "distance_to_best_fit_line2")
        return new avtDistanceToBestFitLineExpression(false);
    if (functionName == "min" || functionName == "minimum")
        return new avtMinMaxExpression(true);
    if (functionName == "max" || functionName == "maximum")
        return new avtMinMaxExpression(false);
    if (functionName == "geodesic_vector_quantize")
        return new avtGeodesicVectorQuantizeExpression();
    if (functionName == "color")
        return new avtColorComposeExpression(3);
    if (functionName == "color4")
        return new avtColorComposeExpression(4);
    if (functionName == "hsvcolor")
        return new avtHSVColorComposeExpression;
    if (functionName == "colorlookup")
        return new avtPerformColorTableLookupExpression;
    if (functionName == "cell_constant"  ||
        functionName == "zonal_constant" ||
        functionName == "zone_constant")
        return new avtConstantFunctionExpression(false);
    if (functionName == "point_constant" ||
        functionName == "nodal_constant" ||
        functionName == "node_constant" )
        return new avtConstantFunctionExpression(true);
    if (functionName == "curve_domain")
        return new avtCurveDomainExpression();
    if (functionName == "curve_integrate")
        return new avtCurveIntegrateExpression();
    if (functionName == "curve_swapxy")
        return new avtCurveSwapXYExpression();
    if (functionName == "curve")
        return new avtCurveExpression();
    if (functionName == "bin")
        return new avtBinExpression();
    if (functionName =="isnan")
        return new avtIsNaNExpression();
    if (functionName == "q_criterion" || functionName == "q_crit")
        return new avtQCriterionExpression();
    if (functionName == "lambda2")
       return new avtLambda2Expression();

    return NULL;
}

// ****************************************************************************
// Method: avtFunctionExpr::CreateFilters
//
// Purpose:
//     Creates the avt filters that are necessary to complete the given
//     function.
//
// Programmer: Sean Ahern
//
// Modifications:
//      Hank Childs, Tue Mar 18 21:36:26 PST 2003
//      Added revolved surface area.
//
//      Sean Ahern, Wed Jun 11 13:44:52 PDT 2003
//      Added vector cross product.
//
//      Hank Childs, Thu Aug 21 09:54:51 PDT 2003
//      Added conditionals/comparisons.
//
//      Hank Childs, Fri Sep 19 16:18:37 PDT 2003
//      Added matrix operations.
//
//      Hank Childs, Wed Dec 10 11:01:07 PST 2003
//      Added recenter.
//
//      Jeremy Meredith, Wed Jun  9 09:16:25 PDT 2004
//      Added specmf.
//
//      Hank Childs, Sat Sep 18 08:54:51 PDT 2004
//      Added neighbor evaluation expressions.
//
//      Hank Childs, Thu Sep 23 09:17:26 PDT 2004
//      Added zone id, node id, and global variants.
//
//      Hank Childs, Sun Jan  2 15:27:00 PST 2005
//      Added macro expressions -- curl, divergence, laplacian, and materror.
//      Also added support functions for materror -- MIRvf and relative diff.
//
//      Hank Childs, Thu Jan 20 15:51:16 PST 2005
//      Added side volume, resrad.
//
//      Kathleen Bonnell, Thu Mar  3 11:13:22 PST 2005 
//      Added var_skew. 
//
//      Jeremy Meredith, Mon Jun 13 17:20:44 PDT 2005
//      This class now holds its name directly.  Use that instead.
//
//      Hank Childs, Thu Jun 30 15:55:06 PDT 2005
//      Added cylindrical, components of cylindrical and polar, mod, floor,
//      round, and ceil.
//
//      Hank Childs, Thu Jul 21 12:34:02 PDT 2005
//      Added array_compose, array_decompose.
//
//      Hank Childs, Tue Aug 16 09:05:03 PDT 2005
//      Added mean_filter, median_filter.
//
//      Hank Childs, Fri Aug 26 13:51:39 PDT 2005
//      Added conn_cmfe.
//
//      Hank Childs, Wed Sep 21 17:30:53 PDT 2005
//      Added external_node, surface normals, min_side_volume, max_side_volume,
//      min_edge_length, and max_edge_length.
//
//      Hank Childs, Mon Oct 10 17:08:05 PDT 2005
//      Added pos_cmfe.
//
//      Brad Whitlock, Fri Nov 18 15:59:27 PST 2005
//      Added distance_to_best_fit_line.
//
//      Hank Childs, Thu Jan  5 08:44:12 PST 2006
//      Remove redundant polar_phi entry that was added with cut-n-paste.
//
//      Hank Childs, Sat Jan 21 14:43:45 PST 2006
//      Added symm_eval_transform.
//
//      Hank Childs, Tue Feb 14 14:03:47 PST 2006
//      Added logical gradient.
//
//      Hank Childs, Sat Feb 18 10:24:30 PST 2006
//      Added apply_ddf.
//
//      Hank Childs, Sun Mar  5 16:01:34 PST 2006
//      Added time.
//
//      Hank Childs, Mon Mar 13 16:43:55 PST 2006
//      Added min and max expressions.
//
//      Hank Childs, Sat Apr 29 14:40:47 PDT 2006
//      Added localized compactness expression.
//
//      Hank Childs, Thu May 11 12:14:51 PDT 2006
//      Added curvature.
//
//      Hank Childs, Fri Aug 25 17:23:34 PDT 2006
//      Make the parsing more robust when the wrong number of arguments are
//      specified.
//
//      Hank Childs, Fri Oct  6 15:45:26 PDT 2006
//      Add inverse of Abel transform.
//
//      Kathleen Bonnell, Fri Sep 15 09:55:55 PDT 2006 
//      Added volume2.  (does a different hex-volume calculation).
//
//      Kathleen Bonnell, Fri Nov 17 08:32:54 PST 2006
//      Moved actual creation of filters to private CreateFilters method,
//      which does not have nested-if blocks, too many of which cause
//      a compile failure on Win32.
//
//      Sean Ahern, Tue Dec 18 16:17:58 EST 2007
//      Set a temporary output variable name for those cases where parsing
//      throws an exception.
//
//      Jeremy Meredith, Tue Feb 19 16:19:24 EST 2008
//      Fixed the ordering of naming for internal variable names.
//
//      Kevin Griffin, Wed May 24 18:02:20 PDT 2017
//      Added all arguments to the filter output name to ensure that it
//      is unique. Only using variable names can result in a name collision
//      which happens with the matvf function when the material names are
//      the same. Fixes Bug #2825.
//
// ****************************************************************************
void
avtFunctionExpr::CreateFilters(ExprPipelineState *state)
{
    // Figure out which filter to add and add it.
    string functionName = name;
    avtExpressionFilter *f = CreateFilters(functionName);

    if (f == NULL)
    {
        string error =
            string("avtFunctionExpr::CreateFilters: "
                   "Unknown function:\"") + functionName +
                   string("\".");
        EXCEPTION1(ExpressionParseException, error);
    }

    // Set a temporary variable name for good error messages.
    f->SetOutputVariableName(functionName.c_str());

    // Have the filter process arguments as necessary.  It's important
    // that this happen before the next bit of code so that the pipeline
    // state is correct.
    f->ProcessArguments(args, state);

    // Ask the filter how many variable arguments it has.  Pop that number
    // of arguments off the stack and store them on a stack.
    int nvars = f->NumVariableArguments();
    string argsText;
    std::vector<string> inputStack;
    int i;
    for (i = 0; i < nvars; i++)
    {
        string inputName;
        if (state->GetNumNames() > 0)
            inputName = state->PopName();
        else
        {
            EXCEPTION2(ExpressionException, functionName,
                       "Parsing of your expression has "
                       "failed.  Failures of the type VisIt's parser has "
                       "encountered are often caused when an expression is "
                       "given less arguments than that expression expects.");
        }

        inputStack.push_back(inputName);
        if (i == 0)
            argsText = inputName;
        else
            argsText = inputName + "," + argsText;
    }

    std::vector<ArgExpr*> *arguments = args->GetArgs();
    if(arguments->size() > nvars)
    {
        for(i=nvars; i<arguments->size(); i++)
        {
            ArgExpr *otherarg = (*arguments)[i];
            argsText = argsText + "," + otherarg->GetText();
        }
    }

    string outputName = functionName + "(" + argsText + ")";

    // Take the stack of variable names and feed them to the function in
    // reverse order.
    int size = static_cast<int>(inputStack.size());
    for (i = 0; i < size; i++)
    {
        string inputName = inputStack.back();
        inputStack.pop_back();
        f->AddInputVariableName(inputName.c_str());
    }

    // Set the variable the function should output.  Reset the output variable
    // name to something more precise than what we set above.
    state->PushName(outputName);
    f->SetOutputVariableName(outputName.c_str());
    
    // Keep track of the current input.
    f->SetInput(state->GetDataObject());
    state->SetDataObject(f->GetOutput());
    state->AddFilter(f);
}

void
avtVarExpr::CreateFilters(ExprPipelineState *state)
{
    state->PushName(var->GetFullpath());
}
