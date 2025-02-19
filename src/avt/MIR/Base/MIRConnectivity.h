// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#ifndef MIR_CONNECTIVITY
#define MIR_CONNECTIVITY

#include <vtkType.h>

class vtkDataSet;

// ****************************************************************************
//  Class:  MIRConnectivity
//
//  Purpose:
//    Holds connectivity for material interface reconstruction.
//
//  Programmer:  Hank Childs
//  Creation:    October 7, 2002
//
//  Modifications:
//    Jeremy Meredith, Mon Sep 15 09:49:10 PDT 2003
//    Put into its own file.
//
//    Kathleen Biagas, Thu Aug 11 2022
//    For VTK9: add offsets, to keep in line with storage in vtkCellArray class:
//    separate connectivity and offsets arrays.  Remove cellIndex.
//
// ****************************************************************************
struct MIRConnectivity
{
    vtkIdType *connectivity;
    vtkIdType  ncells;
    int *celltype;
    vtkIdType *offsets;
    MIRConnectivity();
    ~MIRConnectivity();
    void SetUpConnectivity(vtkDataSet *);
};

#endif
