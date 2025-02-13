// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <vtkRectilinearGridFacelistFilter.h>

#include <vtkAccessors.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRectilinearGrid.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVisItUtility.h>

#include <ImproperUseException.h>

using  std::vector;

//------------------------------------------------------------------------------
vtkRectilinearGridFacelistFilter* vtkRectilinearGridFacelistFilter::New()
{
    // First try to create the object from the vtkObjectFactory
    vtkObject* ret = vtkObjectFactory::CreateInstance("vtkRectilinearGridFacelistFilter");
    if(ret)
    {
        return (vtkRectilinearGridFacelistFilter*)ret;
    }
    // If the factory was unable to create the object, then create it here.
    return new vtkRectilinearGridFacelistFilter;
}


vtkRectilinearGridFacelistFilter::vtkRectilinearGridFacelistFilter()
{
    ForceFaceConsolidation = 0;
}


//
// The rectilinear grid must create a set of points and it does so by walking
// through the front face, back face, bottom face, top face, left face, right
// face (in that order without repeating points on the edges).  Indexing those
// points later can be tiresome, so the logic is contained here in the
// "SpecializedIndexer".
//
class SpecializedIndexer
{
  public:
                      SpecializedIndexer(int, int, int);

   inline int         GetFrontFacePoint(int x, int y)
                           { return frontOffset + x*nY+y; };
   inline int         GetBackFacePoint(int x, int y)
                           { return backOffset + x*nY+y; };
   inline int         GetBottomFacePoint(int x, int z)
                           {
                             if (z == 0) return GetFrontFacePoint(x, 0);
                             if (z == nZ-1) return GetBackFacePoint(x, 0);
                             return bottomOffset + x*(nZ-2)+z-1;
                           };
   inline int         GetTopFacePoint(int x, int z)
                           {
                             if (z == 0) return GetFrontFacePoint(x, nY-1);
                             if (z == nZ-1) return GetBackFacePoint(x, nY-1);
                             return topOffset + x*(nZ-2)+z-1;
                           };
   inline int         GetLeftFacePoint(int y, int z)
                           {
                             if (z == 0) return GetFrontFacePoint(0, y);
                             if (z == nZ-1) return GetBackFacePoint(0, y);
                             if (y == 0) return GetBottomFacePoint(0, z);
                             if (y == nY-1) return GetTopFacePoint(0, z);
                             return leftOffset + (y-1)*(nZ-2)+z-1;
                           };
   inline int         GetRightFacePoint(int y, int z)
                           {
                             if (z == 0) return GetFrontFacePoint(nX-1, y);
                             if (z == nZ-1) return GetBackFacePoint(nX-1, y);
                             if (y == 0) return GetBottomFacePoint(nX-1, z);
                             if (y == nY-1) return GetTopFacePoint(nX-1, z);
                             return rightOffset + (y-1)*(nZ-2)+z-1;
                           };
   inline int         GetCellIndex(int x, int y, int z)
                           { return z*cellZBase + y*cellYBase + x; };


  protected:
   int                nX, nY, nZ;
   int                frontOffset;
   int                backOffset;
   int                bottomOffset;
   int                topOffset;
   int                leftOffset;
   int                rightOffset;
   int                cellZBase;
   int                cellYBase;
};


// ****************************************************************************
//  Modifications:
//    Hank Childs, Wed Aug 25 16:28:52 PDT 2004
//    Account for degenerate meshes.
//
// ****************************************************************************

SpecializedIndexer::SpecializedIndexer(int x, int y, int z)
{
    nX = x;
    nY = y;
    nZ = z;

    frontOffset = 0;
    backOffset = nX*nY;
    bottomOffset = 2*nX*nY;
    topOffset = 2*nX*nY + nX*(nZ-2);
    leftOffset = 2*nX*nY + 2*nX*(nZ-2);
    rightOffset = 2*nX*nY + 2*nX*(nZ-2) + (nY-2)*(nZ-2);

    if (nX > 1)
       cellYBase = (nX-1);
    else
       cellYBase = 1;

    if (nY > 1 && nX > 1)
       cellZBase = (nY-1)*(nX-1);
    else if (nX > 1)
       cellZBase = nX-1;
    else if (nY > 1)
       cellZBase = nY-1;
    else
       cellZBase = 1;
}


// ****************************************************************************
//  Modifications:
//    Kathleen Biagas, Thu Sep 6 11:10:54 MST 2012
//    Templated method to process faces, preserves coordinate data type.
//
// ****************************************************************************

template <class Accessor> inline void
vtkRectilinearGridFacelistFilter_ProcessFaces(int nX, int nY, int nZ,
    vtkPointData *outPointData, vtkPointData *inPointData,
    Accessor x, Accessor y, Accessor z, Accessor p)
{
  int i, j, pointId = 0;

  p.InitTraversal();

  // Front face.
  for (i = 0 ; i < nX ; ++i)
  {
    for (j = 0 ; j < nY ; ++j)
    {
      p.SetComponent(0, x.GetComponent(i));
      p.SetComponent(1, y.GetComponent(j));
      p.SetComponent(2, z.GetComponent(0));
      ++p;
      outPointData->CopyData(inPointData, j*nX + i, pointId++);
    }
  }

  // Back face
  if (nZ > 1)
  {
    for (i = 0 ; i < nX ; ++i)
    {
      for (j = 0 ; j < nY ; ++j)
      {
        p.SetComponent(0, x.GetComponent(i));
        p.SetComponent(1, y.GetComponent(j));
        p.SetComponent(2, z.GetComponent(nZ-1));
        ++p;
        outPointData->CopyData(inPointData, (nZ-1)*nX*nY + j*nX + i,pointId++);
      }
    }
  }

  // Bottom face
  for (i = 0 ; i < nX ; ++i)
  {
    for (j = 1 ; j < nZ-1 ; ++j)
    {
      p.SetComponent(0, x.GetComponent(i));
      p.SetComponent(1, y.GetComponent(0));
      p.SetComponent(2, z.GetComponent(j));
      ++p;
      outPointData->CopyData(inPointData, j*nX*nY + i, pointId++);
    }
  }

  // Top face
  if (nY > 1)
  {
    for (i = 0 ; i < nX ; ++i)
    {
      for (j = 1 ; j < nZ-1 ; ++j)
      {
        p.SetComponent(0, x.GetComponent(i));
        p.SetComponent(1, y.GetComponent(nY-1));
        p.SetComponent(2, z.GetComponent(j));
        ++p;
        outPointData->CopyData(inPointData, j*nX*nY + (nY-1)*nX + i,pointId++);
      }
    }
  }

  // Left face
  for (i = 1 ; i < nY-1 ; ++i)
  {
    for (j = 1 ; j < nZ-1 ; ++j)
    {
      p.SetComponent(0, x.GetComponent(0));
      p.SetComponent(1, y.GetComponent(i));
      p.SetComponent(2, z.GetComponent(j));
      ++p;
      outPointData->CopyData(inPointData, j*nX*nY + i*nX, pointId++);
    }
  }

  // Right face
  if (nX > 1)
  {
    for (i = 1 ; i < nY-1 ; ++i)
    {
      for (j = 1 ; j < nZ-1 ; ++j)
      {
        p.SetComponent(0, x.GetComponent(nX-1));
        p.SetComponent(1, y.GetComponent(i));
        p.SetComponent(2, z.GetComponent(j));
        ++p;
        outPointData->CopyData(inPointData, j*nX*nY + i*nX + nX-1, pointId++);
      }
    }
  }
}


// ****************************************************************************
//  Method: vtkRectilinearGridFacelistFilter::RequestData
//
//  Modifications:
//    Hank Childs, Thu Aug 15 21:13:43 PDT 2002
//    Fixed bug where cell data was being copied incorrectly.
//
//    Hank Childs, Wed Oct 15 19:24:56 PDT 2003
//    Added logic for consolidating faces.
//
//    Hank Childs, Sun Nov  9 09:44:04 PST 2003
//    Made logic for consolidating faces when ghost zones are involved a little
//    more sophisticated.  Also re-ordered how some of the quads in the normal
//    execution were stored, to make the consolidation logic easier.  The new
//    ordering is also more consistent.
//
//    Hank Childs, Fri Jan 30 08:31:44 PST 2004
//    Use pointer arithmetic to construct poly data output.
//
//    Hank Childs, Sun Feb  1 22:02:51 PST 2004
//    Do a better job of estimating the number of cells in the 2D case.
//
//    Hank Childs, Wed Aug 25 16:30:30 PDT 2004
//    Do a better job of handling degenerate meshes.
//
//    Hank Childs, Fri Aug 27 15:15:20 PDT 2004
//    Rename ghost data array.
//
//    Brad Whitlock, Fri Oct 1 17:03:53 PST 2004
//    Pass field data through to the output.
//
//    Hank Childs, Wed Nov 10 11:30:03 PST 2004
//    Correct problem where we are over-allocating number of points for 2D case.
//
//    Hank Childs, Sun Mar 13 11:09:16 PST 2005
//    Fix memory leak.
//
//    Hank Childs, Tue Jan 24 09:53:16 PST 2006
//    Add support for ghost nodes.
//
//    Brad Whitlock, Tue Apr 25 15:13:20 PST 2006
//    Pass the field data after the shallow copy or it does not get passed.
//
//    Kathleen Biagas, Thu Sep 6 11:12:43 MST 2012
//    Use new templated method to process faces, to preserve coordinate type.
//
//    Kathleen Biagas, Thu Aug 11, 2022
//    Support VTK9: connectivity and offsets now stored in separate arrays.
//
// ****************************************************************************

int
vtkRectilinearGridFacelistFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  //
  // Initialize some frequently used values.
  //
  vtkRectilinearGrid *input = vtkRectilinearGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int   i, j;

  //
  // Set up some objects that we will be using throughout the process.
  //
  vtkPolyData        *outPD        = vtkPolyData::New();
  vtkCellData        *inCellData   = input->GetCellData();
  vtkPointData       *inPointData  = input->GetPointData();
  vtkCellData        *outCellData  = outPD->GetCellData();
  vtkPointData       *outPointData = outPD->GetPointData();

  //
  // If there are no ghost zones and we want to consolidate faces, this is a
  // very easy problem.  Just call the routine that will do this simply.
  //
  if (ForceFaceConsolidation)
  {
     if (inCellData->GetArray("avtGhostZones") == NULL &&
         inPointData->GetArray("avtGhostNodes") == NULL)
     {
        ConsolidateFacesWithoutGhostZones(input, output);
        outPD->Delete();
        return 1;
     }
  }

  //
  // If we are doing face compaction, we will come in after the fact and
  // compact some of the faces.  These variables are used for telling the
  // face compaction routine about how the faces are laid out.
  //
  vector<int> faceStart;
  vector<int> rowSize;
  vector<int> columnSize;

  //
  // Get the information about X, Y, and Z from the rectilinear grid.
  //
  vtkDataArray *xc = input->GetXCoordinates();
  int nX = xc->GetNumberOfTuples();
  int tX = xc->GetDataType();
  vtkDataArray *yc = input->GetYCoordinates();
  int nY = yc->GetNumberOfTuples();
  int tY = yc->GetDataType();
  vtkDataArray *zc = input->GetZCoordinates();
  int nZ = zc->GetNumberOfTuples();
  int tZ = zc->GetDataType();

  bool same = (tX == tY && tY == tZ);
  int type = (same ? tX : (tX == VTK_DOUBLE ? tX : (tY == VTK_DOUBLE ? tY :
             (tZ == VTK_DOUBLE ? tZ : VTK_FLOAT))));

  //
  // Now create the points.  Do this so that the total number of points is
  // minimal -- this requires sharing points along edges and corners and leads
  // to a nasty indexing scheme.  Also be wary of 2D issues.
  //
  vtkPoints *pts = vtkPoints::New(type);
  int npts = 0;
  if (nX <= 1)
  {
    npts = nY*nZ;
  }
  else if (nY <= 1)
  {
    npts = nX*nZ;
  }
  else if (nZ <= 1)
  {
    npts = nX*nY;
  }
  else
  {
    npts = 2*nX*nY + 2*(nZ-2)*nX + 2*(nZ-2)*(nY-2);
  }
  pts->SetNumberOfPoints(npts);

  //
  // We will be copying the point data as we go so we need to set this up.
  //
  outPointData->CopyAllocate(input->GetPointData());

  if (same && type == VTK_FLOAT)
  {
      vtkRectilinearGridFacelistFilter_ProcessFaces(nX, nY, nZ,
          outPointData, inPointData,
          vtkDirectAccessor<float>(xc),
          vtkDirectAccessor<float>(yc),
          vtkDirectAccessor<float>(zc),
          vtkDirectAccessor<float>(pts->GetData()));
  }
  else  if (same && type == VTK_DOUBLE)
  {
      vtkRectilinearGridFacelistFilter_ProcessFaces(nX, nY, nZ,
          outPointData, inPointData,
          vtkDirectAccessor<double>(xc),
          vtkDirectAccessor<double>(yc),
          vtkDirectAccessor<double>(zc),
          vtkDirectAccessor<double>(pts->GetData()));
  }
  else
  {
      vtkRectilinearGridFacelistFilter_ProcessFaces(nX, nY, nZ,
          outPointData, inPointData,
          vtkGeneralAccessor(xc),
          vtkGeneralAccessor(yc),
          vtkGeneralAccessor(zc),
          vtkGeneralAccessor(pts->GetData()));
  }

  outPD->SetPoints(pts);
  pts->Delete();

  //
  // Our indexing of points can be nasty, so use the indexer set up
  // specifically for this class and scheme.
  //
  SpecializedIndexer indexer(nX, nY, nZ);

  //
  // Have the cell data allocate memory.
  //
  int   numOutCells = 0;
  if (nX > 1)
     numOutCells += 2*(nY-1)*(nZ-1);
  else
     numOutCells += (nY-1)*(nZ-1);
  if (nY > 1)
     numOutCells += 2*(nX-1)*(nZ-1);
  else
     numOutCells += (nX-1)*(nZ-1);
  if (nZ > 1)
     numOutCells += 2*(nX-1)*(nY-1);
  else
     numOutCells += (nX-1)*(nY-1);
  outCellData->CopyAllocate(inCellData);

  vtkCellArray *polys = vtkCellArray::New();

  vtkNew<vtkIdTypeArray> offsets;
  offsets->SetNumberOfValues(numOutCells+1);
  vtkIdType *ol = offsets->GetPointer(0);
  // Set first offset entry
  *ol++ = 0;
  // subsequent offsets will be incremented by number of points in current cell
  // Create a holder for the incrementation
  vtkIdType currentOffset = 0;

  vtkNew<vtkIdTypeArray> connectivity;
  connectivity->SetNumberOfValues(numOutCells*4);
  vtkIdType *cl = connectivity->GetPointer(0);

  //
  // Left face
  //
  int cellId = 0;
  faceStart.push_back(cellId);
  columnSize.push_back(nZ-1);
  rowSize.push_back(nY-1);
  for (j = 0 ; j < nZ-1 ; ++j)
  {
    for (i = 0 ; i < nY-1 ; ++i)
    {
      currentOffset += 4;
      *ol++ = currentOffset;
      *cl++ = indexer.GetLeftFacePoint(i, j);
      *cl++ = indexer.GetLeftFacePoint(i, j+1);
      *cl++ = indexer.GetLeftFacePoint(i+1, j+1);
      *cl++ = indexer.GetLeftFacePoint(i+1, j);
      int cId = indexer.GetCellIndex(0, i, j);
      outCellData->CopyData(inCellData, cId, cellId);
      ++cellId;
    }
  }

  //
  // Right face
  //
  if (nX > 1)
    {
    faceStart.push_back(cellId);
    rowSize.push_back(nZ-1);
    columnSize.push_back(nY-1);
    for (i = 0 ; i < nY-1 ; ++i)
    {
      for (j = 0 ; j < nZ-1 ; ++j)
      {
        currentOffset += 4;
        *ol++ = currentOffset;
        *cl++ = indexer.GetRightFacePoint(i, j);
        *cl++ = indexer.GetRightFacePoint(i+1, j);
        *cl++ = indexer.GetRightFacePoint(i+1, j+1);
        *cl++ = indexer.GetRightFacePoint(i, j+1);
        int cId = indexer.GetCellIndex(nX-2, i, j);
        outCellData->CopyData(inCellData, cId, cellId);
        ++cellId;
      }
    }
  }

  //
  // Bottom face
  //
  faceStart.push_back(cellId);
  rowSize.push_back(nZ-1);
  columnSize.push_back(nX-1);
  for (i = 0 ; i < nX-1 ; ++i)
  {
    for (j = 0 ; j < nZ-1 ; ++j)
    {
      currentOffset += 4;
      *ol++ = currentOffset;
      *cl++ = indexer.GetBottomFacePoint(i, j);
      *cl++ = indexer.GetBottomFacePoint(i+1, j);
      *cl++ = indexer.GetBottomFacePoint(i+1, j+1);
      *cl++ = indexer.GetBottomFacePoint(i, j+1);
      int cId = indexer.GetCellIndex(i, 0, j);
      outCellData->CopyData(inCellData, cId, cellId);
      ++cellId;
    }
  }

  //
  // Top face
  //
  if (nY > 1)
  {
    faceStart.push_back(cellId);
    rowSize.push_back(nX-1);
    columnSize.push_back(nZ-1);
    for (j = 0 ; j < nZ-1 ; ++j)
    {
      for (i = 0 ; i < nX-1 ; ++i)
      {
        currentOffset += 4;
        *ol++ = currentOffset;
        *cl++ = indexer.GetTopFacePoint(i, j);
        *cl++ = indexer.GetTopFacePoint(i, j+1);
        *cl++ = indexer.GetTopFacePoint(i+1, j+1);
        *cl++ = indexer.GetTopFacePoint(i+1, j);
        int cId = indexer.GetCellIndex(i, nY-2, j);
        outCellData->CopyData(inCellData, cId, cellId);
        ++cellId;
      }
    }
  }

  //
  // Back face
  //
  faceStart.push_back(cellId);
  rowSize.push_back(nX-1);
  columnSize.push_back(nY-1);
  for (j = 0 ; j < nY-1 ; ++j)
  {
    for (i = 0 ; i < nX-1 ; ++i)
    {
      currentOffset += 4;
      *ol++ = currentOffset;
      *cl++ = indexer.GetFrontFacePoint(i, j);
      *cl++ = indexer.GetFrontFacePoint(i, j+1);
      *cl++ = indexer.GetFrontFacePoint(i+1, j+1);
      *cl++ = indexer.GetFrontFacePoint(i+1, j);
      int cId = indexer.GetCellIndex(i, j, 0);
      outCellData->CopyData(inCellData, cId, cellId);
      ++cellId;
    }
  }

  //
  // Front face
  //
  if (nZ > 1)
  {
    faceStart.push_back(cellId);
    rowSize.push_back(nY-1);
    columnSize.push_back(nX-1);
    for (i = 0 ; i < nX-1 ; ++i)
    {
      for (j = 0 ; j < nY-1 ; ++j)
      {
        currentOffset += 4;
        *ol++ = currentOffset;
        *cl++ = indexer.GetBackFacePoint(i, j);
        *cl++ = indexer.GetBackFacePoint(i+1, j);
        *cl++ = indexer.GetBackFacePoint(i+1, j+1);
        *cl++ = indexer.GetBackFacePoint(i, j+1);
        int cId = indexer.GetCellIndex(i, j, nZ-2);
        outCellData->CopyData(inCellData, cId, cellId);
        ++cellId;
      }
    }
  }
  polys->SetData(offsets, connectivity);
  outCellData->Squeeze();
  outPD->SetPolys(polys);
  polys->Delete();

  if (ForceFaceConsolidation)
  {
     //
     // We only get to this spot if we have ghost zones -- which makes
     // consolidating faces a harder problem.  Use a sub-routine to do that.
     //
     vtkPolyData *new_outPD = ConsolidateFacesWithGhostZones(outPD, offsets,
         connectivity, faceStart, rowSize, columnSize);
     outPD->Delete();
     outPD = new_outPD;
  }

  output->ShallowCopy(outPD);
  output->GetFieldData()->ShallowCopy(input->GetFieldData());

  outPD->Delete();

  return 1;
}


// ****************************************************************************
//  Method: vtkRectilinearGridFacelistFilter::FillInputPortInformation
//
// ****************************************************************************

int
vtkRectilinearGridFacelistFilter::FillInputPortInformation(int,
  vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
  return 1;
}


// ****************************************************************************
//  Method: vtkRectilinearGridFacelistFilter::PrintSelf
//
// ****************************************************************************

void
vtkRectilinearGridFacelistFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


// ****************************************************************************
//  Method: vtkRectilinearGridFacelistFilter::ConsolidateFacesWithGhostZones
//
//  Modifications:
//    Hank Childs, Fri Aug 27 15:15:20 PDT 2004
//    Rename ghost data array.
//
//    Hank Childs, Tue Jan 24 09:53:16 PST 2006
//    Add support for ghost nodes.
//
//    Kathleen Biagas, Thu Aug 11, 2022
//    Support VTK9: new signature as connectivity and offsets now stored in
//    separate arrays.
//
// ****************************************************************************

vtkPolyData *
vtkRectilinearGridFacelistFilter::ConsolidateFacesWithGhostZones(
  vtkPolyData *pd, vtkIdTypeArray *offsets, vtkIdTypeArray *connectivity, vector<int> &sideStart,
  vector<int> &rowSize, vector<int> &columnSize)
{
  //
  // The output will have identical point information to our input.  So copy
  // that over now.
  //
  vtkPolyData *cpd = vtkPolyData::New();
  cpd->SetPoints(pd->GetPoints());
  cpd->GetPointData()->ShallowCopy(pd->GetPointData());

  //
  // Set up some useful vars for later.
  //
  vtkPointData *inPointData = pd->GetPointData();
  vtkCellData *inCellData   = pd->GetCellData();
  vtkCellData *outCellData  = cpd->GetCellData();
  vtkUnsignedCharArray *gzv = (vtkUnsignedCharArray *)
                                 inCellData->GetArray("avtGhostZones");
  unsigned char *gza        = NULL;
  bool constructGZA = false;
  vector<unsigned char> ghost_zones;
  vtkUnsignedCharArray *gnv = (vtkUnsignedCharArray *)
                                 inPointData->GetArray("avtGhostNodes");
  if (gzv == NULL && gnv == NULL)
  {
      EXCEPTION0(ImproperUseException);
  }

  if (gnv != NULL)
  {
      unsigned char *gna = gnv->GetPointer(0);
      vtkIdType nCells = pd->GetNumberOfCells();
      gza = new unsigned char[nCells];
      constructGZA = true;

      unsigned char *gz_val = NULL;
      if (gzv != NULL)
          gz_val = gzv->GetPointer(0);
      vtkIdType *ol = offsets->GetPointer(0);
      vtkIdType *cl = connectivity->GetPointer(0);
      for (vtkIdType i = 0 ; i < nCells ; ++i)
      {
          // the difference between this offset and the next yields npts
          // using i+1 is safe for ol because offsets size is nCells+1
          vtkIdType npts = ol[i+1]-ol[i];
          bool oneOkay = false;
          for (vtkIdType j = 0 ; j < npts ; ++j)
          {
              oneOkay = oneOkay || (gna[*cl] == 0);
              ++cl;
          }
          gza[i] = (oneOkay ? 0 : 1);
          if (gz_val != NULL && gz_val[i] > 0)
              gza[i] = gz_val[i];
      }
  }
  else
      gza = gzv->GetPointer(0);

  //
  // We will be modifying the cells.  So set up some of the data structures.
  //
  outCellData->CopyAllocate(inCellData);
  int cellGuess = pd->GetNumberOfCells();
  vtkCellArray *polys = vtkCellArray::New();
  polys->Allocate(cellGuess*(4+1));

  //
  // These for loops will walk through the cells and try to identify
  // neighboring cells that can be compacted.  Although the algorithm appears
  // to be quite slow (there are 5 nested for loops), it should actually run
  // in about O(nfaces) time.
  //
  int nOutputCells = 0;
  int nSides = (int)sideStart.size();
  for (int i = 0 ; i < nSides ; ++i)
  {
    int nEntries = rowSize[i]*columnSize[i];
    int startFace = sideStart[i];
    vector<bool> faceUsed(nEntries, false);
    for (int k = 0 ; k < columnSize[i] ; ++k)
    {
      for (int j = 0 ; j < rowSize[i] ; ++j)
      {
        int face = k*rowSize[i] + j;
        if (faceUsed[face])
           continue;
        unsigned char gz_standard = gza[startFace+face];

        //
        // Find out how far we can go along the row with the same ghost
        // zone value.
        //
        int lastRowMatch = j;
        int l, m;
        for (l = j+1 ; l < rowSize[i] ; ++l)
        {
           int face = k*rowSize[i] + l;
           if (faceUsed[face])
             break;
           unsigned char gz_current = gza[startFace+face];
           if (gz_current != gz_standard)
             break;
           lastRowMatch = l;
        }

        //
        // Now we know we can go from k - lastRowMatch with the same ghost zone
        // value.  Now see how far we can down in columns.
        //
        int lastColumnMatch = k;
        for (m = k+1 ; m < columnSize[i] ; ++m)
        {
          bool all_matches = true;
          for (l = j ; l <= lastRowMatch ; ++l)
          {
            int face = m*rowSize[i] + l;
            if (faceUsed[face])
            {
              all_matches = false;
              break;
            }
            unsigned char gz_current = gza[startFace+face];
            if (gz_current != gz_standard)
            {
              all_matches = false;
              break;
            }
          }

          if (all_matches)
            lastColumnMatch = m;
          else
            break;
        }

        for (l = j ; l <= lastRowMatch ; ++l)
          for (m = k ; m <= lastColumnMatch ; ++m)
          {
            int face = m*rowSize[i] + l;
            faceUsed[face] = true;
          }

        //
        // We know now that we have a face that hasn't been used yet --
        // face (j, k).  In addition, we know that we can form a bigger quad
        // with face (j, k) as one corner and
        // face (lastRowMatch, lastColumnMatch) as the opposite corner.
        //
        int quad_index[4];
        quad_index[0] = startFace + k*rowSize[i]+j;  // bottom-left
        quad_index[1] = startFace + lastColumnMatch*rowSize[i]+j; // top-left
        quad_index[2] = startFace
                       + lastColumnMatch*rowSize[i]+lastRowMatch; // top-right
        quad_index[3] = startFace + k*rowSize[i]+lastRowMatch; // bottom-right

        //
        // We've constructed quad_index so that we want the l'th point from
        // quad 'l' to make our new, bigger quad.  Note that this heavily
        // depends on the quads in "Execute" being constructed in a consistent
        // way.
        //
        vtkIdType   quad[4];
        for (l = 0 ; l < 4 ; ++l)
        {
          quad[l] = pd->GetCell(quad_index[l])->GetPointId(l);
        }
        polys->InsertNextCell(4, quad);
        ghost_zones.push_back(gz_standard);

        //
        // Copy over the new cell data, too.  Just copy the cell data from
        // the original face -> (j, k).
        //
        outCellData->CopyData(inCellData, quad_index[0], nOutputCells++);
      }
    }
  }

  cpd->SetPolys(polys);
  polys->Squeeze();
  cpd->GetCellData()->Squeeze();
  polys->Delete();
  if (constructGZA)
  {
      cpd->GetPointData()->RemoveArray("avtGhostNodes");
      vtkUnsignedCharArray *new_gz = vtkUnsignedCharArray::New();
      new_gz->SetName("avtGhostZones");
      new_gz->SetNumberOfTuples(ghost_zones.size());
      for (size_t i = 0 ; i < ghost_zones.size() ; ++i)
      {
          new_gz->SetValue(i, ghost_zones[i]);
      }
      cpd->GetCellData()->AddArray(new_gz);
      new_gz->Delete();
      delete [] gza;
  }

  return cpd;
}


// ****************************************************************************
//  Method: vtkRectilinearGridFacelistFilter::ConsolidateFacesWithoutGhostZones
//
//  Modifications:
//    Hank Childs, Sun Nov  9 12:37:15 PST 2003
//    Modified this routine to not handle ghost zones at all (since it wasn't
//    doing a very good job in the first place).  Also renamed the routine
//    to make it clear what its purpose was.
//
//    Jeremy Meredith, Tue Oct 14 15:14:39 EDT 2008
//    Handle cases where the grid is 2D but aligned with the X or Y axis,
//    not just the Z axis.
//
//    David Camp, Thu Jul 17 12:49:06 PDT 2014
//    Removed the static variables quads2 and quads3. Made them static class
//    members.
//
// ****************************************************************************

const vtkIdType vtkRectilinearGridFacelistFilter::quads2[1][4] = { { 0, 1, 2, 3 } };
const vtkIdType vtkRectilinearGridFacelistFilter::quads3[6][4] = { { 0, 1, 2, 3 }, { 0, 4, 5, 1 },
                                                                   { 1, 5, 6, 2 }, { 2, 6, 7, 3 },
                                                                   { 3, 7, 4, 0 }, { 4, 7, 6, 5 } };

void
vtkRectilinearGridFacelistFilter::ConsolidateFacesWithoutGhostZones(
  vtkRectilinearGrid *input, vtkPolyData *output)
{
  vtkCellData        *inCellData   = input->GetCellData();
  vtkPointData       *inPointData  = input->GetPointData();
  vtkCellData        *outCellData  = output->GetCellData();
  vtkPointData       *outPointData = output->GetPointData();

  int numOutCells;
  int numOutPoints;
  const vtkIdType (*quads)[4];
  int ptIds[8];

  int nX = input->GetXCoordinates()->GetNumberOfTuples();
  int nY = input->GetYCoordinates()->GetNumberOfTuples();
  int nZ = input->GetZCoordinates()->GetNumberOfTuples();
  if (nX == 1)
  {
      ptIds[0] = 0;
      ptIds[1] = nY-1;
      ptIds[2] = nY*nZ-1;
      ptIds[3] = (nZ-1)*nY;

      numOutCells  = 1;
      numOutPoints = 4;
      quads = quads2;
  }
  else if (nY == 1)
  {
      ptIds[0] = 0;
      ptIds[1] = nX-1;
      ptIds[2] = nX*nZ-1;
      ptIds[3] = (nZ-1)*nX;

      numOutCells  = 1;
      numOutPoints = 4;
      quads = quads2;
  }
  else if (nZ == 1)
  {
      ptIds[0] = 0;
      ptIds[1] = nX-1;
      ptIds[2] = nX*nY-1;
      ptIds[3] = (nY-1)*nX;

      numOutCells  = 1;
      numOutPoints = 4;
      quads = quads2;
  }
  else
  {
      ptIds[0] = 0;
      ptIds[1] = nX-1;
      ptIds[2] = nX*nY-1;
      ptIds[3] = (nY-1)*nX;
      ptIds[4] = (nX*nY)*(nZ-1);
      ptIds[5] = nX-1 + (nX*nY)*(nZ-1);
      ptIds[6] = nX*nY-1 + (nX*nY)*(nZ-1);
      ptIds[7] = (nY-1)*nX + (nX*nY)*(nZ-1);

      numOutCells = 6;
      numOutPoints = 8;
      quads = quads3;
  }

  vtkCellArray *polys = vtkCellArray::New();
  polys->Allocate(numOutCells*(4+1));

  for (int i = 0 ; i < numOutCells ; ++i)
      polys->InsertNextCell(4, quads[i]);

  outCellData->CopyAllocate(inCellData, numOutCells);
  for (vtkIdType i = 0 ; i < numOutCells ; ++i)
      outCellData->CopyData(inCellData, vtkIdType(0), i);

  outPointData->CopyAllocate(inPointData, numOutPoints);
  vtkPoints *pts = vtkVisItUtility::NewPoints(input);
  pts->SetNumberOfPoints(numOutPoints);
  double pt[3];
  for (int i = 0 ; i < numOutPoints ; ++i)
  {
      outPointData->CopyData(inPointData, ptIds[i], i);
      input->GetPoint(ptIds[i], pt);
      pts->SetPoint(i, pt);
  }

  output->SetPolys(polys);
  polys->Delete();
  output->SetPoints(pts);
  pts->Delete();
}

