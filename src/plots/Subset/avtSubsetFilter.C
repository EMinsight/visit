// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                        avtSubsetFilter.C                                  // 
// ************************************************************************* // 


#include <avtSubsetFilter.h>

#include <avtDataAttributes.h>
#include <avtOriginatingSource.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <DebugStream.h>
#include <ImproperUseException.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

// ****************************************************************************
//  Method: avtSubsetFilter constructor
//
//  Arguments:
//
//  Programmer: Kathleen Bonnell 
//  Creation:   November 10, 2004 
//
//  Modifications:
//
// ****************************************************************************

avtSubsetFilter::avtSubsetFilter()
{
    keepNodeZone = false; 
}



// ****************************************************************************
//  Method: avtSubsetFilter::SetPlotAtts
//
//  Purpose:    Sets the SubsetAttributes needed for this filter.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   October 13, 2001 
//
// ****************************************************************************

void
avtSubsetFilter::SetPlotAtts(const SubsetAttributes *atts)
{
    plotAtts = *atts;
}


// ****************************************************************************
//  Method: avtSubsetFilter::ExecuteDataTree
//
//  Purpose:    Break up the dataset into a collection of datasets, one
//              per subset.
//
//  Programmer: Eric Brugger
//  Creation:   December 14, 2001 
//
//  Modifications:
//    Jeremy Meredith, Thu Mar 14 18:00:02 PST 2002
//    Added a flag to enable internal surfaces.
//
//    Jeremy Meredith, Fri Mar 15 12:57:58 PST 2002
//    Added support for propogating ghost zones when splitting the data set.
//
//    Kathleen Bonnell, Thu May 30 09:44:32 PDT 2002      
//    Added debug lines if invalid label format encountered.  It indicates
//    something went wrong somewhere in the pipeline. 
//
//    Kathleen Bonnell, Wed Jun  5 16:09:07 PDT 2002  
//    Moved bad-label test to proper place so it won't prevent simple domain
//    subsets.
//
//    Kathleen Bonnell, Wed Sep  4 16:47:38 PDT 2002 
//    Add test for empty input.
//    
//    Hank Childs, Thu Sep 26 08:37:07 PDT 2002
//    Fixed memory leak.
//
//    Kathleen Bonnell, Fri Dec  6 12:02:04 PST 2002 
//    Copy all cell data, not just ghost levels.  Pass point data.
//
//    Hank Childs, Sat Jun 21 10:48:19 PDT 2003
//    Fix a problem with accessing cells from poly data.  This fix also made
//    the accessing more efficient.
//
//    Kathleen Bonnell, Mon Sep  8 13:43:30 PDT 2003 
//    Add test for No cells for early termination. 
//    
//    Jeremy Meredith, Mon Feb 23 17:35:12 EST 2009
//    If the label isn't a proper subset label, don't error out with
//    a null return value.  Instead, assume we're doing something like
//    a subset plot of a Mesh variable, fall through to the single
//    chunk case, and if it's a NULL label, use the label from the plot
//    attributes.  This fixes subset plots of materials.
//
//    Mark C. Miller, Thu Dec 18 13:21:10 PST 2014
//    Incorporate changes from Jeremy to support subset plots of enum scalar
//    variables for both cell centered and node-centered cases. Thanks Jeremy!
// ****************************************************************************

avtDataTree_p
avtSubsetFilter::ExecuteDataTree(avtDataRepresentation *in_dr)
{
    //
    // Get the VTK data set, the domain number, and the label.
    //
    vtkDataSet *in_ds = in_dr->GetDataVTK();
    int domain = in_dr->GetDomain();
    std::string label = in_dr->GetLabel();

    if (in_ds == NULL || in_ds->GetNumberOfPoints() == 0 ||
        in_ds->GetNumberOfCells() == 0)
    {
        return NULL;
    }

    vector<string> labels;
    int            nDataSets = 0;
    vtkDataSet   **out_ds = NULL;
    bool nodal = false;

    vtkDataArray *subsetArray = in_ds->GetCellData()->GetArray("avtSubsets");
    if (!subsetArray)
        subsetArray = in_ds->GetCellData()->GetScalars();
    if (!subsetArray)
    {
        subsetArray = in_ds->GetPointData()->GetScalars();
        if (subsetArray)
            nodal = true;
    }

    bool splitMats = plotAtts.GetDrawInternal();
    if (subsetArray &&
        !splitMats &&
        label.find(";") != string::npos)
    {
        //
        // Break up the dataset into a collection of datasets, one
        // per subset.
        //
        if (in_ds->GetDataObjectType() != VTK_POLY_DATA)
        {
            EXCEPTION0(ImproperUseException);
        }

        vtkPolyData *in_pd = (vtkPolyData *)in_ds;

        vtkCellData *in_CD = in_ds->GetCellData();

        //
        // Determine the total number of subsets, the number that
        // were selected, and the labels for the subsets.
        //
        char *cLabelStorage = new char[label.length()+1];
        strcpy(cLabelStorage, label.c_str());
        char *cLabel = cLabelStorage;

        int nSelectedSubsets;
        sscanf(cLabel, "%d", &nSelectedSubsets);
        cLabel = strchr(cLabel, ';') + 1;

        int i, *selectedSubsets = new int[nSelectedSubsets];
        char **selectedSubsetNames = new char*[nSelectedSubsets];
        for (i = 0; i < nSelectedSubsets; i++)
        {
            sscanf(cLabel, "%d", &selectedSubsets[i]);
            cLabel = strchr(cLabel, ';') + 1;
            selectedSubsetNames[i] = cLabel;
            cLabel = strchr(cLabel, ';');
            cLabel[0] = '\0';
            cLabel = cLabel + 1;
        }

        int maxSubset = selectedSubsets[0];
        for (i = 1; i < nSelectedSubsets; i++)
        {
            maxSubset = selectedSubsets[i] > maxSubset ?
                        selectedSubsets[i] : maxSubset;
        }

        //
        // Count the number of cells of each subset.
        //
        int *subsetCounts = new int[maxSubset+1];
        for (int s = 0; s < maxSubset + 1; s++)
        {
            subsetCounts[s] = 0;
        }
        int nArrayValues = subsetArray->GetNumberOfTuples();
        for (i = 0; i < nArrayValues; i++)
        {
            subsetCounts[int(subsetArray->GetTuple1(i))]++;
        }

        //
        // Create a dataset for each subset.
        //
        out_ds = new vtkDataSet *[nSelectedSubsets];

        //
        // The following call is a workaround for a VTK bug.  It turns
        // out that when GetCellType if called for the first time for a
        // PolyData it calls its BuildCells method which causes the iterator
        // used by InitTraversal and GetNextCell to be put at the end of
        // the list.
        //
        in_pd->GetCellType(0);
        
        int ntotalcells = in_pd->GetNumberOfCells();
        for (i = 0; i < nSelectedSubsets; i++)
        {
            int s = selectedSubsets[i];

            if (subsetCounts[s] > 0)
            {
                // Create a new polydata
                vtkPolyData *out_pd = vtkPolyData::New();

                // Copy the points and point data.
                out_pd->SetPoints(in_pd->GetPoints());
                out_pd->GetPointData()->PassData(in_pd->GetPointData());

                // Prepare cell data for copy. 
                vtkCellData *out_CD = out_pd->GetCellData();
                out_CD->CopyAllocate(in_CD, subsetCounts[s]);

                // insert the cells
                out_pd->Allocate(subsetCounts[s]);

                vtkIdType npts;
                const vtkIdType *pts;
                int numNewCells = 0;
                for (int j = 0; j < ntotalcells; j++)
                {
                    if (nodal)
                    {
                        in_pd->GetCellPoints(j, npts, pts);
                        bool include = true;
                        for (int k=0; k<npts; ++k)
                        {
                            if (subsetArray->GetTuple1(pts[k]) != s)
                            {
                                include = false;
                                break;
                            }
                        }
                        if (include)
                        {
                            out_pd->InsertNextCell(in_pd->GetCellType(j),
                                                   npts, pts);
                            out_CD->CopyData(in_CD, j, numNewCells++);
                        }
                    }
                    else
                    {
                        if (subsetArray->GetTuple1(j) == s)
                        {
                            in_pd->GetCellPoints(j, npts, pts);
                            out_pd->InsertNextCell(in_pd->GetCellType(j),
                                                   npts, pts);
                            out_CD->CopyData(in_CD, j, numNewCells++); 
                        }
                    }
                }
                out_CD->RemoveArray("avtSubsets");
                labels.push_back(selectedSubsetNames[i]);
                out_ds[nDataSets] = out_pd;
                nDataSets++;
            }
        }

        delete [] subsetCounts;
        delete [] selectedSubsetNames;
        delete [] selectedSubsets;
        delete [] cLabelStorage;
    }
    else
    {
        //
        // The dataset represents a single subset, so just turn it into
        // a data tree.
        //

        if (plotAtts.GetSubsetType() == SubsetAttributes::Mesh &&
            plotAtts.GetSubsetNames().size() > 0)
        {
            // A Subset plot of a mesh doesn't give us a good label.
            // It's like a material plot, but we use the label from
            // the plot attributes instead.  Handle that case here.
            labels.push_back(plotAtts.GetSubsetNames()[0]);
        }
        else
        {
            if (label=="")
                labels.push_back("Whole");
            else
                labels.push_back(label);
        }

        out_ds = new vtkDataSet *[1];
        out_ds[0] = in_ds;
        out_ds[0]->Register(NULL);  // This makes it symmetric with the 'if'
                                    // case so we can delete it blindly later.

        nDataSets = 1;
    }

    if (nDataSets == 0)
    {
        delete [] out_ds;

        return NULL;
    }

    avtDataTree_p outDT = new avtDataTree(nDataSets, out_ds, domain, labels);

    for (int i = 0 ; i < nDataSets ; i++)
    {
        if (out_ds[i] != NULL)
        {
            out_ds[i]->Delete();
        }
    }
    delete [] out_ds;

    return outDT;
}


// ****************************************************************************
//  Method: avtSubsetFilter::UpdateDataObjectInfo
//
//  Purpose:  Retrieves the subset names from the plot attributes and
//            sets them as labels in the data attributes. 
//
//  Programmer: Kathleen Bonnell 
//  Creation:   October 12, 2001 
//
//  Modifications:
//    Kathleen Bonnell, Fri Nov 12 11:50:33 PST 2004
//    Added call to SetKeepNodeZoneArrays. 
//
// ****************************************************************************

void
avtSubsetFilter::UpdateDataObjectInfo(void)
{
    avtDataAttributes &outAtts = GetOutput()->GetInfo().GetAttributes();
    outAtts.SetLabels(plotAtts.GetSubsetNames());
    outAtts.SetKeepNodeZoneArrays(keepNodeZone);
}

 
// ****************************************************************************
//  Method: avtSubsetFilter::ModifyContract
//
//  Purpose:  Turn on domain labels in the data spec if needed.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   Oct 18, 2001
//
//  Modifications:
//    Jeremy Meredith, Thu Mar 14 18:00:02 PST 2002
//    Added a flag to enable internal surfaces.
//
//    Kathleen Bonnell, Wed Sep  4 16:22:57 PDT 2002 
//    Removed NeedDomainLabels. 
//    
//    Hank Childs, Wed Aug 13 07:55:35 PDT 2003
//    Explicitly tell the data spec when we want material interface
//    reconstruction.
//
//    Kathleen Bonnell, Fri Nov 12 10:23:09 PST 2004
//    If working with a point mesh (topodim == 0), then determine if a point
//    size var secondary variable needs to be added to the pipeline, and
//    whether or not we need to keep Node and Zone numbers around. 
//
//    Hank Childs, Tue Jul 31 08:28:18 PDT 2007
//    Tell the contract that we want simplified nesting representations
//    when possible.
//
//    Kathleen Bonnell, Tue Jul 14 13:42:37 PDT 2009
//    Added test for MayRequireNodes for turning Node numbers on.
//
//    Hank Childs, Thu Aug 26 22:23:26 PDT 2010
//    Calculate the extents of the scaling variable.
//
//    Kathleen Biagas, Thu Dec 15 16:16:03 PST 2016
//    Remove test for Material subset as this plot doesn't accept materials.
//
// ****************************************************************************

avtContract_p
avtSubsetFilter::ModifyContract(avtContract_p spec)
{
    if (plotAtts.GetDrawInternal())
    {
        spec->GetDataRequest()->TurnInternalSurfacesOn();
    }

    if (GetInput()->GetInfo().GetAttributes().GetTopologicalDimension() == 0)
    {
        string pointVar = plotAtts.GetPointSizeVar();
        avtDataRequest_p dataRequest = spec->GetDataRequest();

        //
        // Find out if we REALLY need to add the secondary variable.
        //
        if (plotAtts.GetPointSizeVarEnabled() && 
            pointVar != "default" &&
            pointVar != "\0" &&
            pointVar != dataRequest->GetVariable() &&
            !dataRequest->HasSecondaryVariable(pointVar.c_str()))
        {
            spec->GetDataRequest()->AddSecondaryVariable(pointVar.c_str());
            spec->SetCalculateVariableExtents(pointVar, true);
        }

        if (spec->GetDataRequest()->MayRequireZones() ||
            spec->GetDataRequest()->MayRequireNodes())
        {
            keepNodeZone = true;
            spec->GetDataRequest()->TurnNodeNumbersOn();
        }
        else
        {
            keepNodeZone = false;
        }
    }

    spec->GetDataRequest()->TurnSimplifiedNestingRepresentationOn();

    return spec;
}


// ****************************************************************************
//  Method: avtSubsetFilter::PostExcecute
//
//  Purpose:  
//    Sets the output's label attributes to reflect what is currently
//    present in the tree.  
//
//  Programmer: Kathleen Bonnell 
//  Creation:   April 29, 2002 
//
//  Modifications:
//    Kathleen Bonnell, Mon Jul  8 15:22:10 PDT 2002
//    Ensure that subsetNames passed as labels to DataAttributes are in
//    same order as listed in plot atts. 
//
//    Kathleen Bonnell, Mon Nov 25 17:43:32 PST 2002 
//    Re-implemented to work correctly in parallel. 
//    
//    Kathleen Bonnell, Thu Dec 19 10:58:28 PST 2002  
//    Moved label-sorting to the viewer portion of the plot, to prevent
//    communication bottleneck in parallel.  Still send only unique labels
//    to DataAttributes.
//    
// ****************************************************************************

void
avtSubsetFilter::PostExecute(void)
{
    vector <string> treeLabels;
    GetDataTree()->GetAllUniqueLabels(treeLabels);
    GetOutput()->GetInfo().GetAttributes().SetLabels(treeLabels);
}
