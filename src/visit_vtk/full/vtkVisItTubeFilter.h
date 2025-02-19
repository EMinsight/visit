/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVisItTubeFilter - filter that generates tubes around lines
// .SECTION Description
// vtkVisItTubeFilter is a filter that generates a tube around each input line. 
// The tubes are made up of triangle strips and rotate around the tube with
// the rotation of the line normals. (If no normals are present, they are
// computed automatically.) The radius of the tube can be set to vary with 
// scalar or vector value. If the radius varies with scalar value the radius
// is linearly adjusted. If the radius varies with vector value, a mass
// flux preserving variation is used. The number of sides for the tube also 
// can be specified. You can also specify which of the sides are visible. This
// is useful for generating interesting striping effects. Other options
// include the ability to cap the tube and generate texture coordinates.
// Texture coordinates can be used with an associated texture map to create
// interesting effects such as marking the tube with stripes corresponding
// to length or time.
//
// This filter is typically used to create thick or dramatic lines. Another
// common use is to combine this filter with vtkStreamLine to generate
// streamtubes.

// .SECTION Caveats
// The number of tube sides must be greater than 3. If you wish to use fewer
// sides (i.e., a ribbon), use vtkRibbonFilter.
//
// The input line must not have duplicate points, or normals at points that
// are parallel to the incoming/outgoing line segments. (Duplicate points
// can be removed with vtkCleanPolyData.) If a line does not meet this
// criteria, then that line is not tubed.

// .SECTION See Also
// vtkRibbonFilter vtkStreamLine

// .SECTION Thanks
// Michael Finch for absolute scalar radius

#ifndef __vtkVisItTubeFilter_h
#define __vtkVisItTubeFilter_h

#include "vtkPolyDataAlgorithm.h"
#include <visit_vtk_exports.h>

#define VTK_VARY_RADIUS_OFF 0
#define VTK_VARY_RADIUS_BY_SCALAR 1
#define VTK_VARY_RADIUS_BY_VECTOR 2
#define VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR 3

#define VTK_TCOORDS_OFF                    0
#define VTK_TCOORDS_FROM_NORMALIZED_LENGTH 1
#define VTK_TCOORDS_FROM_LENGTH            2
#define VTK_TCOORDS_FROM_SCALARS           3

class vtkCellArray;
class vtkCellData;
class vtkDataArray;
class vtkFloatArray;
class vtkPointData;
class vtkPoints;

// Modifications:
//   Jeremy Meredith, Wed May 26 14:52:29 EDT 2010
//   Allow cell scalars for tube radius.
//
//   Kathleen Biagas, Tue Aug  7 10:55:50 PDT 2012
//   Added ScalarsForRadius var, so that a non-default scalar can be used 
//    to scale the radius.
// 

class VISIT_VTK_API vtkVisItTubeFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkVisItTubeFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Description:
  // Construct object with radius 0.5, radius variation turned off, the
  // number of sides set to 3, and radius factor of 10.
  static vtkVisItTubeFilter *New();

  // Description:
  // Set the minimum tube radius (minimum because the tube radius may vary).
  vtkSetClampMacro(Radius,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(Radius,double);

  // Description:
  // Turn on/off the variation of tube radius with scalar value.
  vtkSetClampMacro(VaryRadius,int,
                   VTK_VARY_RADIUS_OFF,VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR);
  vtkGetMacro(VaryRadius,int);
  void SetVaryRadiusToVaryRadiusOff()
    {this->SetVaryRadius(VTK_VARY_RADIUS_OFF);};
  void SetVaryRadiusToVaryRadiusByScalar()
    {this->SetVaryRadius(VTK_VARY_RADIUS_BY_SCALAR);};
  void SetVaryRadiusToVaryRadiusByVector()
    {this->SetVaryRadius(VTK_VARY_RADIUS_BY_VECTOR);};
  void SetVaryRadiusToVaryRadiusByAbsoluteScalar()
    {this->SetVaryRadius(VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR);};
  const char *GetVaryRadiusAsString();

  // Description:
  // Set the number of sides for the tube. At a minimum, number of sides is 3.
  vtkSetClampMacro(NumberOfSides,int,3,VTK_INT_MAX);
  vtkGetMacro(NumberOfSides,int);

  // Description:
  // Set the maximum tube radius in terms of a multiple of the minimum radius.
  vtkSetMacro(RadiusFactor,double);
  vtkGetMacro(RadiusFactor,double);

  // Description:
  // Set the default normal to use if no normals are supplied, and the
  // DefaultNormalOn is set.
  vtkSetVector3Macro(DefaultNormal,double);
  vtkGetVectorMacro(DefaultNormal,double,3);

  // Description:
  // Set a boolean to control whether to use default normals.
  // DefaultNormalOn is set.
  vtkSetMacro(UseDefaultNormal,bool);
  vtkGetMacro(UseDefaultNormal,bool);
  vtkBooleanMacro(UseDefaultNormal,bool);

  // Description:
  // Set a boolean to control whether tube sides should share vertices.
  // This creates independent strips, with constant normals so the
  // tube is always faceted in appearance.
  vtkSetMacro(SidesShareVertices, bool);
  vtkGetMacro(SidesShareVertices, bool);
  vtkBooleanMacro(SidesShareVertices, bool);

  // Description:
  // Turn on/off whether to cap the ends with polygons.
  vtkSetMacro(Capping,bool);
  vtkGetMacro(Capping,bool);
  vtkBooleanMacro(Capping,bool);

  // Description:
  // Control the striping of the tubes. If OnRatio is greater than 1,
  // then every nth tube side is turned on, beginning with the Offset
  // side.
  vtkSetClampMacro(OnRatio,int,1,VTK_INT_MAX);
  vtkGetMacro(OnRatio,int);

  // Description:
  // Control the striping of the tubes. The offset sets the
  // first tube side that is visible. Offset is generally used with
  // OnRatio to create nifty striping effects.
  vtkSetClampMacro(Offset,int,0,VTK_INT_MAX);
  vtkGetMacro(Offset,int);

  // Description:
  // Control whether and how texture coordinates are produced. This is
  // useful for striping the tube with length textures, etc. If you
  // use scalars to create the texture, the scalars are assumed to be
  // monotonically increasing (or decreasing).
  vtkSetClampMacro(GenerateTCoords,int,VTK_TCOORDS_OFF,
                   VTK_TCOORDS_FROM_SCALARS);
  vtkGetMacro(GenerateTCoords,int);
  void SetGenerateTCoordsToOff()
    {this->SetGenerateTCoords(VTK_TCOORDS_OFF);}
  void SetGenerateTCoordsToNormalizedLength()
    {this->SetGenerateTCoords(VTK_TCOORDS_FROM_NORMALIZED_LENGTH);}
  void SetGenerateTCoordsToUseLength()
    {this->SetGenerateTCoords(VTK_TCOORDS_FROM_LENGTH);}
  void SetGenerateTCoordsToUseScalars()
    {this->SetGenerateTCoords(VTK_TCOORDS_FROM_SCALARS);}
  const char *GetGenerateTCoordsAsString();

  // Description:
  // Control the conversion of units during the texture coordinates
  // calculation. The TextureLength indicates what length (whether 
  // calculated from scalars or length) is mapped to the [0,1)
  // texture space.
  vtkSetClampMacro(TextureLength,double,0.000001,VTK_INT_MAX);
  vtkGetMacro(TextureLength,double);

  vtkSetStringMacro(ScalarsForRadius);
  vtkGetStringMacro(ScalarsForRadius);

protected:
  vtkVisItTubeFilter();
  ~vtkVisItTubeFilter();

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double Radius; //minimum radius of tube
  int VaryRadius; //controls radius variation
  int NumberOfSides; //number of sides to create tube
  double RadiusFactor; //maxium allowablew radius
  double DefaultNormal[3];
  bool UseDefaultNormal;
  bool SidesShareVertices;
  bool Capping; //control whether tubes are capped
  int OnRatio; //control the generation of the sides of the tube
  int Offset;  //control the generation of the sides
  int GenerateTCoords; //control texture coordinate generation
  double TextureLength; //this length is mapped to [0,1) texture space

  char *ScalarsForRadius; 

  // Helper methods
  int GeneratePoints(vtkIdType offset, vtkIdType inCellId,
                     vtkIdType npts, const vtkIdType *pts,
                     vtkPoints *inPts, vtkPoints *newPts,
                     vtkPointData *pd, vtkPointData *outPD,
                     vtkFloatArray *newNormals,
                     vtkDataArray *inScalars, bool cellScalars,
                     double range[2], vtkDataArray *inVectors, double maxNorm,
                     vtkDataArray *inNormals);
  void GenerateStrips(vtkIdType offset, vtkIdType npts, const vtkIdType *pts,
                      vtkIdType inCellId, vtkCellData *cd, vtkCellData *outCD,
                      vtkCellArray *newStrips);
  void GenerateTextureCoords(vtkIdType offset, vtkIdType npts,
                             const vtkIdType *pts,
                             vtkPoints *inPts,
                             vtkDataArray *inScalars, bool cellScalars,
                            vtkFloatArray *newTCoords);
  vtkIdType ComputeOffset(vtkIdType offset,vtkIdType npts);
  
  // Helper data members
  double Theta;

private:
  vtkVisItTubeFilter(const vtkVisItTubeFilter&) = delete;
  void operator=(const vtkVisItTubeFilter&) = delete;
};

#endif
