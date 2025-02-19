// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//  File: avtCreateBondsFilter.C
// ************************************************************************* //

#include <avtCreateBondsFilter.h>

#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkCellArrayIterator.h>
#include <vtkGeometryFilter.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <DebugStream.h>
#include <TimingsManager.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>

#include <float.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

using std::map;
using std::pair;
using std::string;
using std::vector;

// ****************************************************************************
//  Method: avtCreateBondsFilter constructor
//
//  Programmer: Jeremy Meredith
//  Creation:   August 29, 2006
//
// ****************************************************************************

avtCreateBondsFilter::avtCreateBondsFilter()
{
}


// ****************************************************************************
//  Method: avtCreateBondsFilter destructor
//
//  Programmer: Jeremy Meredith
//  Creation:   August 29, 2006
//
//  Modifications:
//
// ****************************************************************************

avtCreateBondsFilter::~avtCreateBondsFilter()
{
}


// ****************************************************************************
//  Method:  avtCreateBondsFilter::Create
//
//  Programmer: Jeremy Meredith
//  Creation:   August 29, 2006
//
// ****************************************************************************

avtFilter *
avtCreateBondsFilter::Create()
{
    return new avtCreateBondsFilter();
}


// ****************************************************************************
//  Method:      avtCreateBondsFilter::SetAtts
//
//  Purpose:
//      Sets the state of the filter based on the attribute object.
//
//  Arguments:
//      a        The attributes to use.
//
//  Programmer: Jeremy Meredith
//  Creation:   August 29, 2006
//
// ****************************************************************************

void
avtCreateBondsFilter::SetAtts(const AttributeGroup *a)
{
    atts = *(const CreateBondsAttributes*)a;
}


// ****************************************************************************
//  Method: avtCreateBondsFilter::Equivalent
//
//  Purpose:
//      Returns true if creating a new avtCreateBondsFilter with the given
//      parameters would result in an equivalent avtCreateBondsFilter.
//
//  Programmer: Jeremy Meredith
//  Creation:   August 29, 2006
//
// ****************************************************************************

bool
avtCreateBondsFilter::Equivalent(const AttributeGroup *a)
{
    return (atts == *(CreateBondsAttributes*)a);
}


// ****************************************************************************
//  Method:  avtCreateBondsFilter::AtomsShouldBeBondedManual
//
//  Purpose:
//    Finds min/max bond lengths for a pair of species.
//
//  Arguments:
//    elementA/B        the atomic numbers of the atom pair
//    dmin,dmax         the min/max allowed bonding distance (output)
//
//  Programmer:  Jeremy Meredith
//  Creation:    August 29, 2006
//
//  Modifications:
//    Jeremy Meredith, Mon Feb 11 16:38:48 EST 2008
//    Support wildcards in matches, and bail out if we matched the
//    atom pattern but the distances were wrong.
//
//    Jeremy Meredith, Wed May 20 11:49:18 EDT 2009
//    MAX_ELEMENT_NUMBER now means the actual max element number, not
//    total number of known elements in visit.  Added a fake "0" element
//    which means "unknown", and hydrogen now starts at 1.  This
//    also means we don't have to correct for 1-origin atomic numbers.
//
//    Jeremy Meredith, Tue Jan 26 16:35:41 EST 2010
//    Split into two functions to allow better optimizations.
//
// ****************************************************************************
bool
avtCreateBondsFilter::AtomBondDistances(int elementA, int elementB,
                                        double &dmin, double &dmax)
{
    dmin = 0.;
    dmax = 0.;

    vector<double> &minDist = atts.GetMinDist();
    vector<double> &maxDist = atts.GetMaxDist();
    vector<int>    &element1 = atts.GetAtomicNumber1();
    vector<int>    &element2 = atts.GetAtomicNumber2();

    size_t n1 = atts.GetAtomicNumber1().size();
    size_t n2 = atts.GetAtomicNumber2().size();
    size_t n3 = atts.GetMinDist().size();
    size_t n4 = atts.GetMaxDist().size();
    if (n1 != n2 || n1 != n3 || n1 != n4)
    {
        EXCEPTION1(ImproperUseException,
                   "Bond list data arrays were not all the same length.");
    }
    size_t n = n1;

    for (size_t i=0; i<n; i++)
    {
        // a -1 in the element list means "any"
        int e1 = element1[i];
        int e2 = element2[i];
        bool match11 = (e1 < 0) || (elementA == e1);
        bool match12 = (e1 < 0) || (elementB == e1);
        bool match21 = (e2 < 0) || (elementA == e2);
        bool match22 = (e2 < 0) || (elementB == e2);
        if ((match11 && match22) || (match12 && match21))
        {
            dmin = minDist[i];
            dmax = maxDist[i];
            return true;
        }
    }

    return false;
}

// ****************************************************************************
// Method:  ShouldAtomsBeBonded
//
// Purpose:
//   Check if two atoms are within the allowable distances.
//
// Arguments:
//   dmin,dmax     the min/max bonding distances
//   pA,pB         the atom locations
//
// Programmer:  Jeremy Meredith
// Creation:    January 26, 2010
//
// ****************************************************************************
inline bool
ShouldAtomsBeBonded(double dmin, double dmax,
                    double *pA, double *pB)
{
    // Check for errors, of in case of no match for this pair
    if (dmax <= 0 || dmax < dmin)
        return false;

    // Okay, now see if the atom pair matches the right distance
    float dx = pA[0] - pB[0];
    float dy = pA[1] - pB[1];
    float dz = pA[2] - pB[2];
    float dist2 = dx*dx + dy*dy + dz*dz;

    if (dist2 > dmin*dmin && dist2 < dmax*dmax)
        return true;
    else
        return false;
}


// ****************************************************************************
//  Method:  avtCreateBondsFilter::ExecuteData
//
//  Purpose:
//      Sends the specified input and output through the CreateBonds filter.
//
//  Arguments:
//      in_dr      The input data representation.
//
//  Returns:       The output data representation.
//
//  Programmer:  Jeremy Meredith
//  Creation:    January 27, 2010
//
//  Modifications:
//    Jeremy Meredith, Tue Feb 22 21:36:07 EST 2011
//    Added support for non-polydata mesh types through the geometry filter.
//    Sure, it may be a little blunt, but it doesn't hurt anything and allows
//    e.g. Silo point meshes (which are unstructured, not poly data) to work.
//
//    Eric Brugger, Wed Jul 23 11:25:16 PDT 2014
//    Modified the class to work with avtDataRepresentation.
//
// ****************************************************************************
avtDataRepresentation *
avtCreateBondsFilter::ExecuteData(avtDataRepresentation *in_dr)
{
    //
    // Get the VTK data set.
    //
    vtkDataSet *in_ds = in_dr->GetDataVTK();

    vtkGeometryFilter *geom = NULL;
    if (in_ds->GetDataObjectType() != VTK_POLY_DATA)
    {
        geom = vtkGeometryFilter::New();
        geom->SetInputData(in_ds);
        geom->Update();
        in_ds = geom->GetOutput();
    }

    //
    // Find the maximum bond distance as specified in the atts
    // since this determines the box size.
    //
    float maxBondDist = 0.;
    vector<double> &maxDist = atts.GetMaxDist();
    for (size_t i=0; i<maxDist.size(); i++)
    {
        if (maxDist[i] > maxBondDist)
            maxBondDist = maxDist[i];
    }
    if (maxBondDist <= 0.)
        return in_dr;

    //
    // Find the actual extents
    //
    float minx =  FLT_MAX, maxx = -FLT_MAX;
    float miny =  FLT_MAX, maxy = -FLT_MAX;
    float minz =  FLT_MAX, maxz = -FLT_MAX;
    int nPts = in_ds->GetNumberOfPoints();
    for (int p=0; p<nPts; p++)
    {
        double pt[4] = {0,0,0,1};
        in_ds->GetPoint(p, pt);
        if (pt[0] < minx) minx = pt[0];
        if (pt[0] > maxx) maxx = pt[0];
        if (pt[1] < miny) miny = pt[1];
        if (pt[1] > maxy) maxy = pt[1];
        if (pt[2] < minz) minz = pt[2];
        if (pt[2] > maxz) maxz = pt[2];
    }

    // If the max bond distance isn't some smallish percentage of the
    // extents, then no point doing the more complex spatial
    // subdivision scheme.  And if it's too small, the overhead of
    // making such a big vector grid for the box storage could be bad.
    int approxNBoxes = (int)((maxx - minx)*(maxy - miny)*(maxz - minz) /
                                        (maxBondDist*maxBondDist*maxBondDist));
    double minAllowable = 8;
    double maxAllowable = 10000000;
    vtkDataSet *out_ds = NULL;
    if (approxNBoxes < minAllowable || approxNBoxes > maxAllowable)
    {
        debug4 << "avtCreateBondsFilter: reverting to slow method\n";
        out_ds = ExecuteData_Slow((vtkPolyData*)in_ds);
    }
    else
    {
        debug4 << "avtCreateBondsFilter: using fast method, "
               << "approximately "<<approxNBoxes<<" boxes\n";
        out_ds = ExecuteData_Fast((vtkPolyData*)in_ds, maxBondDist,
                                  minx,maxx, miny,maxy, minz,maxz);
    }
    if (geom)
        geom->Delete();

    avtDataRepresentation *out_dr = new avtDataRepresentation(out_ds,
        in_dr->GetDomain(), in_dr->GetLabel());

    out_ds->Delete();

    return out_dr;
}


// ****************************************************************************
// Method:  avtCreateBondsFilter::ExecuteData_Fast
//
// Purpose:
//   Smarter space-subdivision search for bonds.
//
// Arguments:
//   in           the input data set
//   maxBondDist  the maximum distance of any bonding criteria
//   min*,max*    the extents of the actual data set points
//
// Returns:       The output dataset.
//
// Programmer:  Jeremy Meredith
// Creation:    January 27, 2010
//
// Modifications:
//   Jeremy Meredith, Wed Aug 11 10:21:59 EDT 2010
//   Use the element variable specified in the attributes.
//   Support any type of input nodal array.
//
//   Kathleen Biagas, Tue Aug 21 16:08:27 MST 2012
//   Preserve coordinate type.
//
//   Eric Brugger, Wed Jul 23 11:25:16 PDT 2014
//   Modified the class to work with avtDataRepresentation.
//
//   Kathleen Biagas, Thu Aug 11, 2022
//   Support VTK9: use vtkCellArrayIterator.
//
// ****************************************************************************

vtkDataSet *
avtCreateBondsFilter::ExecuteData_Fast(vtkPolyData *in, float maxBondDist,
                                       float minx, float maxx,
                                       float miny, float maxy,
                                       float minz, float maxz)
{
    //
    // Extract some input data
    //
    vtkPoints    *inPts = in->GetPoints();
    vtkPointData *inPD  = in->GetPointData();
    vtkCellData  *inCD  = in->GetCellData();
    int nPts   = in->GetNumberOfPoints();
    int nVerts = in->GetNumberOfVerts();

    vtkDataArray *element;
    if (atts.GetElementVariable() == "default")
        element = inPD->GetScalars();
    else
        element = inPD->GetArray(atts.GetElementVariable().c_str());
    if (!element)
    {
        debug1 << "avtCreateBondsFilter: did not find appropriate point array\n";
        debug1 << "   -- returning original data set with no bonds changed.\n";
        in->Register(NULL);
        return in;
    }

    vector<float> elementnos;
    elementnos.resize(nPts);
    for (int i=0; i<nPts; i++)
        elementnos[i] = element->GetTuple1(i);

    //
    // Set up the output stuff
    //
    vtkPolyData  *out      = in->NewInstance();
    vtkPointData *outPD    = out->GetPointData();
    vtkCellData  *outCD    = out->GetCellData();
    vtkPoints    *outPts   = vtkPoints::New(inPts->GetDataType());
    vtkCellArray *outVerts = vtkCellArray::New();
    vtkCellArray *outLines = vtkCellArray::New();
    out->GetFieldData()->ShallowCopy(in->GetFieldData());
    out->SetPoints(outPts);
    out->SetLines(outLines);
    out->SetVerts(outVerts);
    outPts->Delete();
    outLines->Delete();
    outVerts->Delete();

    //outPts->SetNumberOfPoints(nPts * 1.1);
    outPD->CopyAllocate(inPD,nPts);
    outCD->CopyAllocate(inCD,(int)(nVerts*1.2));


    //
    // Copy the input points and verts over.
    //
    for (int p=0; p<nPts; p++)
    {
        double pt[4] = {0,0,0,1};
        in->GetPoint(p, pt);
        int outpt = outPts->InsertNextPoint(pt);
        outPD->CopyData(inPD, p, outpt);
    }

    if (nVerts > 0)
    {
        auto inVerts = vtk::TakeSmartPointer(in->GetVerts()->NewIterator());
        for (inVerts->GoToFirstCell(); !inVerts->IsDoneWithTraversal(); inVerts->GoToNextCell())
        {
            vtkIdList *vertPtr = inVerts->GetCurrentCell();
            if (vertPtr->GetNumberOfIds() == 1)
            {
                vtkIdType outcell = outVerts->InsertNextCell(1);
                outVerts->InsertCellPoint(vertPtr->GetId(0));
                outCD->CopyData(inCD, inVerts->GetCurrentCellId(), outcell);
            }
        }
    }

    int natoms = in->GetNumberOfPoints();

    //
    // Extract unit cell vectors in case we want to add bonds
    // for periodic atom images
    //
    bool addPeriodicBonds = atts.GetAddPeriodicBonds();
    double xv[3], yv[3], zv[3];
    for (int j=0; j<3; j++)
    {
        if (atts.GetUseUnitCellVectors())
        {
            const avtDataAttributes &datts = GetInput()->GetInfo().GetAttributes();
            const float *unitCell = datts.GetUnitCellVectors();
            xv[j] = unitCell[3*0+j];
            yv[j] = unitCell[3*1+j];
            zv[j] = unitCell[3*2+j];
        }
        else
        {
            xv[j] = atts.GetXVector()[j];
            yv[j] = atts.GetYVector()[j];
            zv[j] = atts.GetZVector()[j];
        }
    }

    //
    // ni,nj,nk are the number of bins in the i,j,k directions
    //
    // Why the "3 + ..." here?
    // (a) if we weren't doing periodic atom checks, we could
    //     make do with 1+ -- that "1" we always need because there
    //     will be one atom at maxx,maxy,maxz which will go in
    //     the final cube.  easier than clamping or fuzz factors.
    // (b) for periodic checks, we know they must be within
    //     maxBondDist of the existing cube.
    int ni = 3 + int((maxx - minx) / maxBondDist);
    int nj = 3 + int((maxy - miny) / maxBondDist);
    int nk = 3 + int((maxz - minz) / maxBondDist);

    int max_per_atom = atts.GetMaxBondsClamp();

    //
    // Support for per-axis periodicity here
    //
    int perXmin = 0, perXmax = 0;
    int perYmin = 0, perYmax = 0;
    int perZmin = 0, perZmax = 0;
    if (addPeriodicBonds && atts.GetPeriodicInX())
    {
        perXmin = -1;
        perXmax = +1;
    }
    if (addPeriodicBonds && atts.GetPeriodicInY())
    {
        perYmin = -1;
        perYmax = +1;
    }
    if (addPeriodicBonds && atts.GetPeriodicInZ())
    {
        perZmin = -1;
        perZmax = +1;
    }

    //
    // I know -- I'm using a grid of STL vectors, and this could
    // potentially be inefficient.  However, even with STL vectors the
    // algorithm is quite complex, and the benefit of the algorithm so
    // large, that there is little need to do better yet.  I'll
    // probably wait until I see evidence of problems before changing.
    //
    typedef pair<int,int> atomimage;
    typedef vector<atomimage> atomvec;
    atomvec *atomgrid = new atomvec[ni*nj*nk];

    map<atomimage, int> imageMap;

    int nativeImage = -1;
    for (int a=0; a<natoms; a++)
    {
        double pt_orig[4] = {0,0,0,1};
        in->GetPoint(a, pt_orig);
        int image = 0;
        for (int px=perXmin; px<=perXmax; px++)
        {
            for (int py=perYmin; py<=perYmax; py++)
            {
                for (int pz=perZmin; pz<=perZmax; pz++, image++)
                {
                    if (nativeImage < 0 && px==0 && py==0 && pz==0)
                        nativeImage = image;
                    double xoff = (double(px)*xv[0]+
                                   double(py)*yv[0]+
                                   double(pz)*zv[0]);
                    double yoff = (double(px)*xv[1]+
                                   double(py)*yv[1]+
                                   double(pz)*zv[1]);
                    double zoff = (double(px)*xv[2]+
                                   double(py)*yv[2]+
                                   double(pz)*zv[2]);
                    double pt[3] = {pt_orig[0]+xoff,
                                    pt_orig[1]+yoff,
                                    pt_orig[2]+zoff};
                    // Add 1+ here because the outermost boxes are
                    // for holding periodic atoms (though because
                    // the unit cell is not axis-aligned, periodic
                    // atoms coule potentially go anywhere).
                    int ix = 1+int((pt[0] - minx) / maxBondDist);
                    int jx = 1+int((pt[1] - miny) / maxBondDist);
                    int kx = 1+int((pt[2] - minz) / maxBondDist);
                    if (ix<0 || ix>=ni || jx<0 || jx>=nj || kx<0 || kx>=nk)
                        continue;

                    atomgrid[ix + ni*(jx + nj*(kx))].push_back(atomimage(image,a));

                    // We already added the points and vertices for the
                    // original atoms.  Don't add them twice, and don't
                    // add vertex cells for the phoney atoms.
                    if (image != nativeImage)
                    {
                        // Yes, it is memory-wasteful to add all atoms
                        // from all 26 non-native images.  However, we
                        // don't store any which lie any significant
                        // distance outside the original bounding box,
                        // so the max cost should be one extra bin's
                        // worth of atoms.
                        int outpt = outPts->InsertNextPoint(pt);
                        outPD->CopyData(inPD, a, outpt);
                        imageMap[atomimage(image,a)] = outpt;
                    }
                }
            }
        }
    }


    //
    // Loop over all boxes once.  For each atom in the box, loop over
    // all 27 boxes surrounding the current one, since atom matches
    // could be in any of those boxes (by design, this check spans at
    // least maxBondDist every direction from any atom in the box).
    //
    for (int i=1; i<ni-1; i++)
    {
        for (int j=1; j<nj-1; j++)
        {
            for (int k=1; k<nk-1; k++)
            {
                int index1 = i + ni*(j + nj*(k));
                int nla1 = (int)atomgrid[index1].size();
                // for each atom in each box
                for (int la1=0; la1<nla1; la1++)
                {
                    int ctr = 0;
                    atomimage atom1 = atomgrid[index1][la1];
                    int image1 = atom1.first;
                    int a1 = atom1.second;

                    // we need at least one of the atoms to be
                    // original, i.e. in the native image.
                    // choose it to be atom1.
                    if (image1 != nativeImage)
                        continue;

                    // loop over all 27 surrounding boxes to find
                    // atoms which can bond with the current one.
                    for (int p=-1; p<=+1 && ctr<max_per_atom; p++)
                    {
                        int ii = i+p;
                        for (int q=-1; q<=+1 && ctr<max_per_atom; q++)
                        {
                            int jj = j+q;
                            for (int r=-1; r<=+1 && ctr<max_per_atom; r++)
                            {
                                int kk = k+r;
                                // we don't need to see if ii,jj,kk is
                                // within a valid box here, because we
                                // added a layer of extra boxes around
                                // the real ones in case we want to do
                                // periodic checks.  Note that we always
                                // add this extra layer of boxes, though.

                                int index2 = ii + ni*(jj + nj*(kk));
                                int nla2 = (int)atomgrid[index2].size();
                                for (int la2=0; la2<nla2 && ctr<max_per_atom; la2++)
                                {
                                    if (index1==index2 && la1==la2)
                                        continue;

                                    atomimage atom2 = atomgrid[index2][la2];
                                    int image2 = atom2.first;
                                    int a2 = atom2.second;

                                    // if both atoms are in the native image,
                                    // don't check both directions.  (noting
                                    // that a1 is always in the native image).
                                    // it's only for periodic checks we need
                                    // to do both directions.
                                    if (image2 == nativeImage && a1>a2)
                                        continue;

                                    // get the element number of the
                                    // atoms.  for a2, must look up
                                    // from its *original* atom number
                                    int e1 = (int) elementnos[a1];
                                    int e2 = (int) elementnos[a2];

                                    // okay, now check for an actual match.
                                    // note that if it's not the native image,
                                    // we need to look up the new actual
                                    // point index.
                                    double dmin, dmax;
                                    bool possible =
                                        AtomBondDistances(e1,e2, dmin,dmax);
                                    if (possible)
                                    {
                                        int newA2 = a2;
                                        if (image2 != nativeImage)
                                            newA2 = imageMap[atomimage(image2,a2)];
                                        double p1[3];
                                        double p2[3];
                                        outPts->GetPoint(a1,p1);
                                        outPts->GetPoint(newA2,p2);
                                        if (ShouldAtomsBeBonded(dmin,dmax,p1,p2))
                                        {
                                            outLines->InsertNextCell(2);
                                            outLines->InsertCellPoint(a1);
                                            outLines->InsertCellPoint(newA2);
                                            ctr++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    delete[] atomgrid;

    return out;
}


// ****************************************************************************
//  Method: avtCreateBondsFilter::ExecuteData_Slow
//
//  Purpose:
//      N^2 brute-force algorithm search for bonds.
//
//  Arguments:
//      in        The input dataset.
//
//  Returns:       The output dataset.
//
//  Programmer: Jeremy Meredith
//  Creation:   Tue Apr 18 17:24:04 PST 2006
//
//  Modifications:
//    Jeremy Meredith, Mon Feb 11 16:39:51 EST 2008
//    The manual bonding matches now supports wildcards, alleviating the need
//    for a "simple" mode.  (The default for the manual mode is actually
//    exactly what the simple mode was going to be anyway.)
//
//    Jeremy Meredith, Wed Jan 27 10:37:03 EST 2010
//    Added periodic atom matching support.  This is a bit slow for the moment,
//    about a 10x penalty compared to the non-periodic matching, so we should
//    add a faster version at some point....
//
//    Jeremy Meredith, Wed Jan 27 16:03:04 EST 2010
//    Added a new fast version which subdivides space into cubes.
//    This is now known as the "slow" version.  Also, fixed a bug
//    with setting the periodic instance min/max.
//
//    Jeremy Meredith, Wed Aug 11 10:21:59 EDT 2010
//    Use the element variable specified in the attributes.
//    Support any type of input nodal array.
//
//    Kathleen Biagas, Tue Aug 21 16:08:27 MST 2012
//    Preserve coordinate type.
//
//    Eric Brugger, Wed Jul 23 11:25:16 PDT 2014
//    Modified the class to work with avtDataRepresentation.
//
//    Kathleen Biagas, Thu Aug 11, 2022
//    Support VTK9: use vtkCellArrayIterator.
//
// ****************************************************************************

vtkDataSet *
avtCreateBondsFilter::ExecuteData_Slow(vtkPolyData *in)
{
    //
    // Extract some input data
    //
    vtkPoints    *inPts = in->GetPoints();
    vtkPointData *inPD  = in->GetPointData();
    vtkCellData  *inCD  = in->GetCellData();
    int nPts   = in->GetNumberOfPoints();
    int nVerts = in->GetNumberOfVerts();

    vtkDataArray *element;
    if (atts.GetElementVariable() == "default")
        element = inPD->GetScalars();
    else
        element = inPD->GetArray(atts.GetElementVariable().c_str());
    if (!element)
    {
        debug1 << "avtCreateBondsFilter: did not find appropriate point array\n";
        debug1 << "   -- returning original data set with no bonds changed.\n";
        in->Register(NULL);
        return in;
    }

    vector<float> elementnos;
    elementnos.resize(nPts);
    for (int i=0; i<nPts; i++)
        elementnos[i] = element->GetTuple1(i);

    //
    // Set up the output stuff
    //
    vtkPolyData  *out      = in->NewInstance();
    vtkPointData *outPD    = out->GetPointData();
    vtkCellData  *outCD    = out->GetCellData();
    vtkPoints    *outPts   = vtkPoints::New(inPts->GetDataType());
    vtkCellArray *outVerts = vtkCellArray::New();
    vtkCellArray *outLines = vtkCellArray::New();
    out->GetFieldData()->ShallowCopy(in->GetFieldData());
    out->SetPoints(outPts);
    out->SetLines(outLines);
    out->SetVerts(outVerts);
    outPts->Delete();
    outLines->Delete();
    outVerts->Delete();

    //outPts->SetNumberOfPoints(nPts * 1.1);
    outPD->CopyAllocate(inPD,nPts);
    outCD->CopyAllocate(inCD,(int)(nVerts*1.2));

    //
    // Copy the input points and verts over.
    //
    for (int p=0; p<nPts; p++)
    {
        double pt[4] = {0,0,0,1};
        in->GetPoint(p, pt);
        int outpt = outPts->InsertNextPoint(pt);
        outPD->CopyData(inPD, p, outpt);
    }

    if (nVerts > 0)
    {
        auto inVerts = vtk::TakeSmartPointer(in->GetVerts()->NewIterator());
        for (inVerts->GoToFirstCell(); !inVerts->IsDoneWithTraversal(); inVerts->GoToNextCell())
        {
            vtkIdList *vertPtr = inVerts->GetCurrentCell();
            if (vertPtr->GetNumberOfIds() == 1)
            {
                int outcell = outVerts->InsertNextCell(1);
                outVerts->InsertCellPoint(vertPtr->GetId(0));
                outCD->CopyData(inCD, inVerts->GetCurrentCellId(), outcell);
            }
        }
    }

    int natoms = in->GetNumberOfPoints();


    //
    // Extract unit cell vectors in case we want to add bonds
    // for periodic atom images
    //
    bool addPeriodicBonds = atts.GetAddPeriodicBonds();
    double xv[3], yv[3], zv[3];
    for (int j=0; j<3; j++)
    {
        if (atts.GetUseUnitCellVectors())
        {
            const avtDataAttributes &datts = GetInput()->GetInfo().GetAttributes();
            const float *unitCell = datts.GetUnitCellVectors();
            xv[j] = unitCell[3*0+j];
            yv[j] = unitCell[3*1+j];
            zv[j] = unitCell[3*2+j];
        }
        else
        {
            xv[j] = atts.GetXVector()[j];
            yv[j] = atts.GetYVector()[j];
            zv[j] = atts.GetZVector()[j];
        }
    }

    map<pair<int, int>, int> imageMap;

    //
    // Loop over the (n^2)/2 atom pairs
    //
    int max_per_atom = atts.GetMaxBondsClamp();
    int timerMain = visitTimer->StartTimer();
    for (int i=0; i<natoms; i++)
    {
        int ctr = 0;
        for (int j=natoms-1; j>i && ctr<max_per_atom; j--)
        {
            if (i==j)
                continue;

            double p1[3];
            double p2[3];
            inPts->GetPoint(i,p1);
            inPts->GetPoint(j,p2);

            int e1 = (int) elementnos[i];
            int e2 = (int) elementnos[j];

            double dmin, dmax;
            bool possible = AtomBondDistances(e1,e2, dmin,dmax);
            if (possible && ShouldAtomsBeBonded(dmin,dmax, p1,p2))
            {
                outLines->InsertNextCell(2);
                outLines->InsertCellPoint(i);
                outLines->InsertCellPoint(j);
                ctr++;
            }
        }
    }
    visitTimer->StopTimer(timerMain, "CreateBonds: Main n^2 loop");

    //
    // If we're adding periodic bonds, loop over all n^2 atoms again.
    // This time, look for ones where one of the two atoms is not
    // in the native image (but the other one is).  Note that we're
    // not keeping any earlier bond-per-atom counters, so when this
    // is added, we might get 2*max total bonds.  Since this is
    // typically non-physical and is only a limit for sanity, no big
    // harm done.
    //
    int perXmin = 0, perXmax = 0;
    int perYmin = 0, perYmax = 0;
    int perZmin = 0, perZmax = 0;
    if (addPeriodicBonds && atts.GetPeriodicInX())
    {
        perXmin = -1;
        perXmax = +1;
    }
    if (addPeriodicBonds && atts.GetPeriodicInY())
    {
        perYmin = -1;
        perYmax = +1;
    }
    if (addPeriodicBonds && atts.GetPeriodicInZ())
    {
        perZmin = -1;
        perZmax = +1;
    }

    int timerPer = visitTimer->StartTimer();
    for (int j=0; j<natoms && addPeriodicBonds; j++)
    {
        int e2 = (int) elementnos[j];
        double p2[3];
        inPts->GetPoint(j,p2);

        int    newJ[27];
        double newX[27];
        double newY[27];
        double newZ[27];
        int image = 0;
        for (int px=perXmin; px<=perXmax; px++)
        {
            for (int py=perYmin; py<=perYmax; py++)
            {
                for (int pz=perZmin; pz<=perZmax; pz++)
                {
                    double pj[3] = {p2[0],p2[1],p2[2]};
                    for (int c=0; c<3; c++)
                    {
                        pj[c] += xv[c] * double(px);
                        pj[c] += yv[c] * double(py);
                        pj[c] += zv[c] * double(pz);
                    }
                    newJ[image] = -1;
                    newX[image] = pj[0];
                    newY[image] = pj[1];
                    newZ[image] = pj[2];
                    image++;
                }
            }
        }

        int ctr = 0;
        for (int i=0; i<natoms && addPeriodicBonds && ctr<max_per_atom; i++)
        {
            if (i==j)
                continue;

            int e1 = (int) elementnos[i];
            double p1[3];
            inPts->GetPoint(i,p1);

            double dmin, dmax;
            bool possible = AtomBondDistances(e1,e2, dmin,dmax);
            if (!possible)
                continue;

            int image = 0;
            for (int px=perXmin; px<=perXmax; px++)
            {
                for (int py=perYmin; py<=perYmax; py++)
                {
                    for (int pz=perZmin; pz<=perZmax; pz++, image++)
                    {
                        bool nativeImage = (px==0 && py==0 && pz==0);
                        if (nativeImage)
                            continue;

                        double *pi = p1;
                        double pj[3] = {newX[image],newY[image],newZ[image]};

                        if (ShouldAtomsBeBonded(dmin,dmax,pi,pj))
                        {
                            if (newJ[image] == -1)
                            {
                                pair<int,int> jthImage(image,j);
                                if (imageMap.count(jthImage)!=0)
                                {
                                    newJ[image] = imageMap[jthImage];
                                }
                                else
                                {
                                    newJ[image] = outPts->InsertNextPoint(pj);
                                    imageMap[jthImage] = newJ[image];
                                    outPD->CopyData(inPD, j, newJ[image]);

                                    // We're not adding a vertex cell here;
                                    // these are not real atoms, and the
                                    // molecule plot will now avoid drawing
                                    // them if no Vert is associated w/ it.
                                    //int outcell = outVerts->InsertNextCell(1);
                                    //outVerts->InsertCellPoint(newJ[image]);
                                    //outCD->CopyData(inCD, j, outcell);
                                }
                            }
                            outLines->InsertNextCell(2);
                            outLines->InsertCellPoint(i);
                            outLines->InsertCellPoint(newJ[image]);
                            ctr++;
                        }
                    }
                }
            }
        }
    }
    visitTimer->StopTimer(timerPer, "CreateBonds: periodic n^2 * 27images loop");

    vtkDataArray *origCellNums = outPD->GetArray("avtOriginalNodeNumbers");
    if (origCellNums)
    {
        for (int i=nPts; i<out->GetNumberOfPoints(); i++)
            origCellNums->SetTuple1(i,-1);
    }

    return out;
}

// ****************************************************************************
//  Method:  avtCreateBondsFilter::ModifyContract
//
//  Purpose:
//    Add element numbers to the secondary variable request.
//
//  Arguments:
//    spec       the pipeline specification
//
//  Programmer:  Jeremy Meredith
//  Creation:    August 29, 2006
//
//  Modifications:
//    Jeremy Meredith, Wed Aug 11 10:21:59 EDT 2010
//    Use the element variable specified in the attributes.
//    Skip the removal of a "bonds" variable -- this was obsolete long ago.
//
// ****************************************************************************
avtContract_p
avtCreateBondsFilter::ModifyContract(avtContract_p spec)
{
    //
    // Get the old specification.
    //
    avtDataRequest_p ds = spec->GetDataRequest();
    const char *primaryVariable = ds->GetVariable();

    //
    // Make a new one
    //
    avtDataRequest_p nds = new avtDataRequest(ds);

    // We need those element numbers.
    if (string(primaryVariable) != atts.GetElementVariable() &&
        atts.GetElementVariable() != "default")
    {
        nds->AddSecondaryVariable(atts.GetElementVariable().c_str());
    }

    //
    // Create the new pipeline spec from the data spec, and return
    //
    avtContract_p rv = new avtContract(spec, nds);

    return rv;
}

// ****************************************************************************
// Method: avtCreateBondsFilter::UpdateDataObjectInfo
//
// Purpose:
//   Update the data object information.
//
// Note:       Work partially supported by DOE Grant SC0007548.
//
// Programmer: Brad Whitlock
// Creation:   Tue Mar 18 10:53:05 PDT 2014
//
// Modifications:
//
// ****************************************************************************

void
avtCreateBondsFilter::UpdateDataObjectInfo(void)
{
    avtPluginDataTreeIterator::UpdateDataObjectInfo();

    GetOutput()->GetInfo().GetAttributes().AddFilterMetaData("CreateBonds");
}
