/*****************************************************************************
*
* Copyright (c) 2000 - 2019, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                            avtGTCFileFormat.h                             //
// ************************************************************************* //

#ifndef AVT_GTC_FILE_FORMAT_H
#define AVT_GTC_FILE_FORMAT_H

#include <avtSTMDFileFormat.h>

#include <hdf5.h>

#include <visit-hdf5.h>

#include <string>

class parallelBuffer;

// ****************************************************************************
//  Class: avtGTCFileFormat
//
//  Purpose:
//      Reads in GTC files as a plugin to VisIt.
//
//  Programmer: dpn -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

class avtGTCFileFormat : public avtSTMDFileFormat
{
  public:
                       avtGTCFileFormat(const char *filename);
    virtual           ~avtGTCFileFormat() {;};

    virtual const char    *GetType(void)   { return "GTC"; };
    virtual void           FreeUpResources(void);

    virtual vtkDataSet    *GetMesh(int, const char *);
    virtual vtkDataArray  *GetVar( int, const char *);
    virtual vtkDataArray  *GetVectorVar( int, const char *);

  protected:
    // DATA MEMBERS
    void                   Initialize();
    virtual void           PopulateDatabaseMetaData(avtDatabaseMetaData *);
    void                   ReadVariable( int domain, int varIdx, int varDim, float **ptrVar );
    
    std::string            IndexToVarName( int idx ) const;
    int                    VarNameToIndex( const std::string &var ) const;

    int                    nVars, nTotalPoints, nPoints;
    bool                   initialized;
    hsize_t                startOffset;                    

#if PARALLEL
    int                    nProcs, rank;

    void                   ParallelReadVariable( int domain, int varDim, float *var, float *ids );
    void                   ParallelReadParticles( int domain, float *pts, float *ids );
    void                   BinData( int dim, parallelBuffer **array, float *vars, float *ids,
                                    float **myVars, float **myIds );
    int *                  GetDataShareMatrix( parallelBuffer **array );
    void                   CommunicateData( int dim, int *dataShare, parallelBuffer **array,
                                            float **myPts, float **myIds );
#endif
    
};

#ifdef PARALLEL

// ****************************************************************************
//  Class: parallelBuffer
//
//  Purpose:
//      Dynamic array.
//
//  Programmer: Dave Pugmire
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

class parallelBuffer
{
public:
    parallelBuffer( int elemSz );
    ~parallelBuffer();

    int ElemSize() const { return elemSize; }
    int Size() const { return size; }
    float* Get( int i ) { return &(pArray[i*elemSize]); }
    void Add( float *data ) { AddElement( data ); }
    
protected:
    void AddElement( float *data );
    
    float *pArray;
    int size, buffSize, elemSize;
};

#endif

#endif
