// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                           avtDatasetExaminer.C                            //
// ************************************************************************* //

#include <avtDatasetExaminer.h>

#include <avtParallel.h>

#include <float.h>
#include <DebugStream.h>
#include <TimingsManager.h>


// ****************************************************************************
//  Method: avtDatasetExaminer::HasData
//
//  Purpose:
//    Examines the data tree for data.
//
//  Returns:
//    Return true if there is a data tree with data.
//
//  Programmer:  Allen Sanderson
//  Creation:    January 24, 2021
//
//  Modifications:
//
// ****************************************************************************
bool
avtDatasetExaminer::HasData(avtDataset_p &ds)
{
    avtDataTree_p dataTree = ds->dataTree;

    if(*dataTree != nullptr)
    {
        return dataTree->GetNumberOfLeaves() > 0;
    }
    else
        return false;
}

// ****************************************************************************
//  Method: avtDatasetExaminer::GetNumberOfZones
//
//  Purpose:
//    Gets the number of zones in the data tree.
//
//  Returns:
//    The number of zones (cells) in the underlying vtk dataset of the
//    avtDataTree.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    January 04, 2001
//
//  Modifications:
//
//    Kathleen Bonnell, Thu Apr  5 13:34:11 PDT 2001
//    Reflect data now stored as avtDataTree instead of array of
//    avtDomainTrees.  Use avtDataTree::Traverse method instead of
//    retrieving vtkDataSets individually.
//
//    Hank Childs, Fri Mar 15 17:18:00 PST 2002
//    Moved from class avtDataset.
//
//    Hank Childs, Sat Nov 21 13:16:09 PST 2009
//    Calculate number of zones with a long long.
//
//    Kathleen Biagas, Thu Sep 11 08:49:01 PDT 2014
//    Add 'originalOnly' arg, to call a method that only counts original
//    zones (utilizes avtOriginalCellNumbers).
//
//    Kathleen Biagas, Wed Nov 18 2020
//    Replace VISIT_LONG_LONG with long long.
//
// ****************************************************************************

long long
avtDatasetExaminer::GetNumberOfZones(avtDataset_p &ds, bool originalOnly)
{
    avtDataTree_p dataTree = ds->dataTree;

    long long numZones = 0;
    if (*dataTree != nullptr)
    {
        bool dummy;
        if (!originalOnly)
        {
            dataTree->Traverse(CGetNumberOfZones, &numZones, dummy);
        }
        else
        {
            OrigElementCountArgs args;
            dataTree->Traverse(CGetNumberOfOriginalZones, (void *)&args, dummy);
            if (args.elementCount.size() > 0)
                numZones = args.elementCount.size();
            else
                numZones = GetNumberOfZones(ds);
        }
    }
    return numZones;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetVariableList
//
//  Purpose:
//      Gets a list of variables the dataset is defined over.
//
//  Returns:     A list of variables the dataset is defined over.
//
//  Programmer:  Hank Childs
//  Creation:    November 14, 2001
//
//  Modifications:
//
//    Hank Childs, Fri Mar 15 17:18:00 PST 2002
//    Moved from class avtDataset.
//
//    Hank Childs, Wed Jul  7 08:10:46 PDT 2004
//    Get the variable list using meta-data information, rather than by
//    searching the dataset.
//
//    Hank Childs, Thu May 31 22:32:08 PDT 2007
//    Set the variable size.
//
// ****************************************************************************

void
avtDatasetExaminer::GetVariableList(avtDataset_p &ds, VarList &vl)
{
    avtDataAttributes &atts = ds->GetInfo().GetAttributes();
    vl.nvars = atts.GetNumberOfVariables();
    vl.varnames.clear();
    vl.varsizes.clear();
    for (int i = 0 ; i < vl.nvars ; i++)
    {
        vl.varnames.push_back(atts.GetVariableName(i));
        vl.varsizes.push_back(atts.GetVariableDimension(vl.varnames[i].c_str()));
    }
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetSpatialExtents
//
//  Purpose:
//      Gets the spatial extents of the dataset.
//
//  Arguments:
//      se        A place to put the spatial extents
//
//  Returns:      Whether or not the extents were obtained.
//
//  Programmer:   Hank Childs
//  Creation:     January 5, 2001
//
//  Modifications:
//
//    Kathleen Bonnell, Fri Feb  9 17:11:18 PST 2001
//    Added call to GetDomain, as domains are stored as avtDomainTree.
//
//    Hank Childs, Sun Mar 25 15:25:50 PST 2001
//    Added debug statement for case where there are no extents.
//
//    Kathleen Bonnell, Fri Apr 13 16:27:15 PDT 2001
//    Changed to utilize avtDataTree::Traverse method.
//
//    Hank Childs, Tue Jul 17 13:34:28 PDT 2001
//    Return whether or not we got the extents.
//
//    Hank Childs, Fri Sep  7 18:30:33 PDT 2001
//    Changed argument from float to double.  Also made the declaration be
//    a double * instead of a double[6], since some compilers struggle with
//    the (non-)distinction.
//
//    Hank Childs, Fri Mar 15 17:18:00 PST 2002
//    Moved from class avtDataset.
//
//    Jeremy Meredith, Thu Feb 15 13:09:05 EST 2007
//    Also pass along any inherent transform to be applied to rectilinear
//    grids.
//
// ****************************************************************************

bool
avtDatasetExaminer::GetSpatialExtents(avtDataset_p &ds, double *se)
{
    avtDataTree_p dataTree = ds->dataTree;


    bool foundExtents = false;
    for (int i = 0 ; i < 3 ; i++)
    {
        se[2*i + 0] = +DBL_MAX;
        se[2*i + 1] = -DBL_MAX;
    }

    if ( *dataTree != nullptr )
    {
        // See if we're supposed to apply a transform to any rectilinear grids.
        const double *rectXform = nullptr;
        avtDataAttributes &atts = ds->GetInfo().GetAttributes();
        if (atts.GetRectilinearGridHasTransform())
            rectXform = atts.GetRectilinearGridTransform();

        // Create an info structure with the needed variables.
        struct {double *se; const double *xform;} info = { se, rectXform };
        dataTree->Traverse(CGetSpatialExtents, &info, foundExtents);
    }

    if (!foundExtents)
    {
        debug1 << "Unable to determine spatial extents -- dataset needs an "
               << "update" << endl;
    }

    return foundExtents;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetSpatialExtents
//
//  Purpose:
//      Gets the spatial extents of the multiple data sets.
//
//  Arguments:
//      se        A place to put the spatial extents
//
//  Returns:      Whether or not the extents were obtained.
//
//  Programmer:   Hank Childs
//  Creation:     January 9, 2006
//
//  Note: this will not perform correctly on transformed rectilinear grids
//
//  Modifications:
//    Jeremy Meredith, Thu Jan 18 10:56:28 EST 2007
//    The CGetSpatialExtents traversal now also expects a unit cell vector
//    pointer.  We pass in nullptr, which is the best we can do without
//    further information from the avtDataAttributes.
//
// ****************************************************************************

bool
avtDatasetExaminer::GetSpatialExtents(std::vector<avtDataTree_p> &l,double *se)
{

    bool foundExtents = false;
    for (size_t i = 0 ; i < 3 ; i++)
    {
        se[2*i + 0] = +DBL_MAX;
        se[2*i + 1] = -DBL_MAX;
    }

    for (size_t i = 0 ; i < l.size() ; i++)
    {
        // We don't have access to avtDataAttributes here, so assume
        // that there is no rectilinear transform applied
        const double *rectXform = nullptr;

        // Create an info structure with the needed variables.
        struct {double *se; const double *xform;} info = { se, rectXform };
        l[i]->Traverse(CGetSpatialExtents, &info, foundExtents);
    }

    if (!foundExtents)
    {
        debug1 << "Unable to determine spatial extents -- dataset needs an "
               << "update" << endl;
    }

    return foundExtents;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetDataExtents
//
//  Purpose:
//      Gets the data extents of the dataset.
//
//  Arguments:
//      de        A place to put the data extents
//
//  Returns:      Whether or not the extents were obtained.
//
//  Programmer:   Hank Childs
//  Creation:     January 5, 2001
//
//  Modifications:
//
//    Kathleen Bonnell, Fri Feb  9 17:11:18 PST 2001
//    Added call to GetDomain, as domains are stored as avtDataTree.
//
//    Hank Childs, Sun Mar 25 15:25:50 PST 2001
//    Added debug statement for case where there are no extents.
//
//    Kathleen Bonnell, Fri Apr 13 16:27:15 PDT 2001
//    Changed to utilize avtDataTree::Traverse method.
//
//    Hank Childs, Tue Jul 17 13:34:28 PDT 2001
//    Return whether or not we got the extents.
//
//    Hank Childs, Fri Sep  7 18:30:33 PDT 2001
//    Changed argument from float to double.  Also made the declaration be
//    a double * instead of a double[2], since some compilers struggle with
//    the (non-)distinction.  Removed assumptions that we have scalar data.
//
//    Hank Childs, Fri Mar 15 17:18:00 PST 2002
//    Moved from class avtDataset.
//
//    Hank Childs, Tue Feb 24 17:36:32 PST 2004
//    Account for multiple variables.
//
//    Kathleen Bonnell, Thu Mar 11 10:14:20 PST 2004
//    DataExtents now always has only 2 components.
//
//    Hank Childs, Fri Jun  9 13:25:31 PDT 2006
//    Remove unused variable
//
//    Kathleen Biagas, Wed May 28 17:38:40 MST 2014
//    Added connectedNodesOnly.
//
//    Eric Brugger, Thu Jul  6 13:31:48 PDT 2023
//    Added debug output and return false if no values could be found to
//    set the limits.
//
// ****************************************************************************

bool
avtDatasetExaminer::GetDataExtents(avtDataset_p &ds, double *de,
                                   const char *varname,
                                   bool connectedNodesOnly)
{
    bool foundExtents = false;

    de[0] = +DBL_MAX;
    de[1] = -DBL_MAX;

    if (*ds != nullptr && *(ds->GetDataTree()) != nullptr)
    {
        if (varname == nullptr)
            varname = ds->GetInfo().GetAttributes().GetVariableName().c_str();

        avtDataTree_p dataTree = ds->dataTree;

        GetVariableRangeArgs gvra;
        gvra.varname = varname;
        gvra.extents = de;
        gvra.connectedNodesOnly = connectedNodesOnly;

        dataTree->Traverse(CGetDataExtents, (void *) &gvra, foundExtents);

        if (!foundExtents)
        {
            debug1 << "Unable to determine data extents -- dataset needs an "
                   << "update" << endl;
        }
    }
    
    //
    // If the data extents are still their initial values, then there were
    // no elements or the cells were all ghost cells.
    //
    if (de[0] == +DBL_MAX && de[1] == -DBL_MAX)
    {
        debug1 << "Unable to determine data extents -- there was either no data "
               << "or all the data was in ghost zones." << endl;
        foundExtents = false;
    }

    return foundExtents;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::FindMaximum
//
//  Purpose:
//      Determines the minimum value and the point corresponding to that
//      maximum.
//
//  Arguments:
//      ds       A dataset.
//      pt       The point.
//      value    The value of the maximum.
//
//  Programmer:  Hank Childs
//  Creation:    March 15, 2002
//
// ****************************************************************************

void
avtDatasetExaminer::FindMaximum(avtDataset_p &ds, double *pt, double &value)
{
    FindExtremeArgs args;
    bool  success = false;
    args.value = -DBL_MAX;
    if ( *ds->dataTree != nullptr )
    {
        ds->dataTree->Traverse(CFindMaximum, (void *) &args, success);
    }

    if (success)
    {
        value = args.value;
        pt[0] = args.point[0];
        pt[1] = args.point[1];
        pt[2] = args.point[2];
    }
    else
    {
        value = -DBL_MAX;
        pt[0] = 0.;
        pt[1] = 0.;
        pt[2] = 0.;
    }
}


// ****************************************************************************
//  Method: avtDatasetExaminer::FindMinimum
//
//  Purpose:
//      Determines the minimum value and the point corresponding to that
//      minimum.
//
//  Arguments:
//      ds       A dataset.
//      pt       The point.
//      value    The value of the minimum.
//
//  Programmer:  Hank Childs
//  Creation:    March 15, 2002
//
// ****************************************************************************

void
avtDatasetExaminer::FindMinimum(avtDataset_p &ds, double *pt, double &value)
{
    FindExtremeArgs args;
    bool  success = false;
    args.value = DBL_MAX;
    if ( *ds->dataTree != nullptr )
    {
        ds->dataTree->Traverse(CFindMinimum, (void *) &args, success);
    }

    if (success)
    {
        value = args.value;
        pt[0] = args.point[0];
        pt[1] = args.point[1];
        pt[2] = args.point[2];
    }
    else
    {
        value = DBL_MAX;
        pt[0] = 0.;
        pt[1] = 0.;
        pt[2] = 0.;
    }
}


// ****************************************************************************
//  Method: avtDatasetExaminer::FindZone
//
//  Purpose:
//      Determines the location of a zone.
//
//  Arguments:
//      ds       A dataset.
//      dom      A domain number.
//      zone     A zone number.
//      pt       A location to put the point in.
//
//  Returns:     true if it located the zone, false otherwise.
//
//  Programmer:  Hank Childs
//  Creation:    March 15, 2002
//
// ****************************************************************************

bool
avtDatasetExaminer::FindZone(avtDataset_p &ds, int dom, int zone, double *pt)
{
    LocateObjectArgs args;
    bool  success = false;
    args.domain = dom;
    args.index  = zone;
    if ( *ds->dataTree != nullptr )
    {
        ds->dataTree->Traverse(CLocateZone, (void *) &args, success);
    }

    if (success)
    {
        pt[0] = args.point[0];
        pt[1] = args.point[1];
        pt[2] = args.point[2];
    }

    return success;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::FindNode
//
//  Purpose:
//      Determines the location of a node.
//
//  Arguments:
//      ds       A dataset.
//      dom      A domain number.
//      zone     A zone number.
//      pt       A location to put the point in.
//
//  Returns:     true if it located the zone, false otherwise.
//
//  Programmer:  Hank Childs
//  Creation:    March 15, 2002
//
// ****************************************************************************

bool
avtDatasetExaminer::FindNode(avtDataset_p &ds, int dom, int zone, double *pt)
{
    LocateObjectArgs args;
    bool  success = false;
    args.domain = dom;
    args.index  = zone;
    if ( *ds->dataTree != nullptr )
    {
        ds->dataTree->Traverse(CLocateNode, (void *) &args, success);
    }

    if (success)
    {
        pt[0] = args.point[0];
        pt[1] = args.point[1];
        pt[2] = args.point[2];
    }

    return success;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetArray
//
//  Purpose:
//      Locates an array for a domain.
//
//  Arguments:
//      ds       A dataset.
//      varname  The name of a variable.
//      dom      A domain number.
//      cent     The centering for the variable (output variable).
//
//  Returns:     The data array, nullptr if it does not exist.
//
//  Programmer:  Hank Childs
//  Creation:    July 29, 2003
//
// ****************************************************************************

vtkDataArray *
avtDatasetExaminer::GetArray(avtDataset_p &ds, const char *varname, int dom,
                             avtCentering &cent)
{
    GetArrayArgs args;
    bool  success = false;
    args.arr = nullptr;
    args.domain = dom;
    args.varname  = varname;
    args.centering = AVT_UNKNOWN_CENT;
    if ( *ds->dataTree != nullptr )
    {
        ds->dataTree->Traverse(CGetArray, (void *) &args, success);
    }

    if (success)
    {
        cent = args.centering;
    }

    return args.arr;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetVariableCentering
//
//  Purpose:
//      Locates an array for a domain.
//
//  Arguments:
//      ds       A dataset.
//      varname  The name of a variable.
//      dom      A domain number.
//      cent     The centering for the variable (output variable).
//
//  Returns:     The data array, nullptr if it does not exist.
//
//  Programmer:  Hank Childs
//  Creation:    July 29, 2003
//
// ****************************************************************************

avtCentering
avtDatasetExaminer::GetVariableCentering(avtDataset_p &ds, const char *varname)
{
    GetArrayArgs args;
    bool  success = false;
    args.arr = nullptr;
    args.domain = -1;
    args.varname  = varname;
    args.centering = AVT_UNKNOWN_CENT;
    if ( *ds->dataTree != nullptr )
    {
        ds->dataTree->Traverse(CGetVariableCentering, (void *) &args, success);
    }

    return args.centering;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetNumberOfNodes
//
//  Purpose:
//    Gets the number of nodes in the data tree.
//
//  Returns:
//    The number of nodes (vertices) in the underlying vtk dataset of the
//    avtDataTree.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    February 18, 2004
//
//  Modifications:
//
//    Hank Childs, Sat Nov 21 13:16:09 PST 2009
//    Calculate number of nodes with a long long.
//
//    Kathleen Biagas, Thu Sep 11 08:49:01 PDT 2014
//    Add 'originalOnly' arg, to call a method that only counts original
//    nodes (utilizes avtOriginalNodeNumbers).
//
//    Kathleen Biagas, Wed Nov 18 2020
//    Replace VISIT_LONG_LONG with long long.
//
// ****************************************************************************

long long
avtDatasetExaminer::GetNumberOfNodes(avtDataset_p &ds, bool originalOnly)
{
    avtDataTree_p dataTree = ds->dataTree;

    long long numNodes = 0;
    if (*dataTree != nullptr)
    {
        bool dummy;
        if (!originalOnly)
        {
            dataTree->Traverse(CGetNumberOfNodes, &numNodes, dummy);
        }
        else
        {
            OrigElementCountArgs args;
            dataTree->Traverse(CGetNumberOfOriginalNodes, (void *)&args, dummy);
            if (args.elementCount.size() > 0)
                numNodes = args.elementCount.size();
            else
                numNodes = GetNumberOfNodes(ds);
        }
    }
    return numNodes;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetNumberOfZones
//
//  Purpose:
//    Gets the number of zones in the data tree.
//    Counts 'real' and 'ghost' zones separately.
//
//  Returns:
//    The number of zones (cells) in the underlying vtk dataset of the
//    avtDataTree.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    January 04, 2001
//
//  Modifications:
//
//    Hank Childs, Sat Nov 21 13:16:09 PST 2009
//    Calculate number of zones with a long long.
//
//    Kathleen Biagas, Thu Sep 11 08:49:01 PDT 2014
//    Add 'originalOnly' arg, to call a method that only counts original
//    zones (utilizes avtOriginalCellNumbers).
//
//    Kathleen Biagas, Wed Nov 18 2020
//    Replace VISIT_LONG_LONG with long long.
//
// ****************************************************************************

void
avtDatasetExaminer::GetNumberOfZones(avtDataset_p &ds, long long &nReal,
                                     long long &nGhost, bool originalOnly)
{
    avtDataTree_p dataTree = ds->dataTree;

    long long numZones[2] = {0, 0};
    if (*dataTree != nullptr)
    {
        bool dummy;
        if (!originalOnly)
        {
            dataTree->Traverse(CGetNumberOfRealZones, numZones, dummy);
            nReal = numZones[0];
            nGhost = numZones[1];
        }
        else
        {
            OrigElementCountArgs args;
            dataTree->Traverse(CGetNumberOfRealOriginalZones, (void*)&args, dummy);
            if (args.elementCount.size() > 0 || args.ghostElementCount.size() > 0)
            {
                nReal = args.elementCount.size();
                nGhost = args.ghostElementCount.size();
            }
            else
            {
                GetNumberOfZones(ds, nReal, nGhost);
            }
        }
    }
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetNumberOfNodes
//
//  Purpose:
//    Gets the number of nodes in the data tree.
//    Counts 'real' and 'ghost' nodes separately.
//
//  Returns:
//    The number of nodes (points) in the underlying vtk dataset of the
//    avtDataTree.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    July 29, 2008
//
//  Modifications:
//
//    Hank Childs, Sat Nov 21 13:16:09 PST 2009
//    Calculate number of nodes with a long long.
//
//    Kathleen Biagas, Thu Sep 11 08:49:01 PDT 2014
//    Add 'originalOnly' arg, to call a method that only counts original
//    nodes (utilizes avtOriginalNodeNumbers).
//
//    Kathleen Biagas, Wed Nov 18 2020
//    Replace VISIT_LONG_LONG with long long.
//
// ****************************************************************************

void
avtDatasetExaminer::GetNumberOfNodes(avtDataset_p &ds, long long &nReal,
                                     long long &nGhost, bool originalOnly)
{
    avtDataTree_p dataTree = ds->dataTree;

    long long numNodes[2] = {0, 0};
    if (*dataTree != nullptr)
    {
        bool dummy;
        if (!originalOnly)
        {
            dataTree->Traverse(CGetNumberOfRealNodes, numNodes, dummy);
            nReal = numNodes[0];
            nGhost = numNodes[1];
        }
        else
        {
            OrigElementCountArgs args;
            dataTree->Traverse(CGetNumberOfRealOriginalNodes, (void*)&args, dummy);
            if (args.elementCount.size() > 0 || args.ghostElementCount.size() > 0)
            {
                nReal = args.elementCount.size();
                nGhost = args.ghostElementCount.size();
            }
            else
            {
                GetNumberOfNodes(ds, nReal, nGhost);
            }
        }
    }
}


// ****************************************************************************
//  Method: avtDatasetExaminer::CalculateHistogram
//
//  Purpose:
//      Calculates a histogram for a given variable.  This deals with parallel
//      details as well.
//
//  Arguments:
//      var       The variable to calculate the histogram for
//      min       The minimum value for the histogram (values below this are
//                clamped to min).
//      max       The maximum value for the histogram (values above this are
//                clamped to max).
//      numvals   An array to store the number of values.  Note that this array
//                should be sized to the number of bins in the histogram.
//
//  Returns:      true if the histogram was successfully calculated, false
//                otherwise.
//
//  Programmer: Hank Childs
//  Creation:   May 21, 2010
//
//  Modifications:
//    Kathleen Biagas, Wed Nov 18 2020
//    Replace VISIT_LONG_LONG with long long.
//
// ****************************************************************************

bool
avtDatasetExaminer::CalculateHistogram(avtDataset_p &ds,
                                       const std::string &var,
                                       double min, double max,
                                       std::vector<long long> &numvals)
{
    avtDataTree_p dataTree = ds->dataTree;

    bool err = false;
    CalculateHistogramArgs args;
    int t1 = visitTimer->StartTimer();

    if (*dataTree != nullptr)
    {
        args.min      = min;
        args.max      = max;
        args.variable = var;
        args.numVals.resize(numvals.size(), 0);
        dataTree->Traverse(CCalculateHistogram, (void *) &args, err);

        if( !err )
        {
            for(size_t i = 0; i < numvals.size(); ++i)
                numvals[i] = args.numVals[i];
        }
    }

    return err;
}


// ****************************************************************************
//  Method: avtDatasetExaminer::GetToplogicalDim
//
//  Purpose:
//      Calculates topological dim by examing the contents of the data tree.
//
//  Returns:     The overall topological dimension
//               (largest in mixed-topology case).
//
//  Programmer:  Kathleen Biagas
//  Creation:    April 5, 2022
//
//  Modifications:
//
// ****************************************************************************

int
avtDatasetExaminer::GetTopologicalDim(avtDataTree_p &dataTree, const int reportedDim)
{
    // Don't want an empty or nullptr tree contributing
    if (*dataTree == nullptr)
        return -1;

    if (dataTree->IsEmpty())
        return -1;

    int tDim = reportedDim;
    int newDim = tDim;
    bool success = false;
    // Create an info structure with the needed variables.
    struct {const int *reportedDim; int *dim;} info = { &tDim, &newDim };
    dataTree->Traverse(CGetTopologicalDim, &info, success);
    if( success )
    {
        tDim = newDim;
    }
    return tDim;
}

int
avtDatasetExaminer::GetTopologicalDim(avtDataset_p &ds)
{
    avtDataAttributes &atts = ds->GetInfo().GetAttributes();
    int tDim = atts.GetTopologicalDimension();
    avtDataTree_p dataTree = ds->dataTree;

    return avtDatasetExaminer::GetTopologicalDim(dataTree, tDim);
}
