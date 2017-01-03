/*****************************************************************************
*
* Copyright (c) 2000 - 2017, Lawrence Livermore National Security, LLC
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
//                            avtXDATFileFormat.h                           //
// ************************************************************************* //

#ifndef AVT_XDAT_FILE_FORMAT_H
#define AVT_XDAT_FILE_FORMAT_H

#include <avtFileFormatInterface.h>
#include <avtMTSDFileFormat.h>

// ****************************************************************************
//  Class: avtXDATFileFormat
//
//  Purpose:
//      Reads in XDAT files as a plugin to VisIt.
//
//  Programmer: Jeremy Meredith
//  Creation:   March  4, 2016
//
//  Modifications:
//
// ****************************************************************************

class avtXDATFileFormat : public avtMTSDFileFormat
{
  public:
    static bool        Identify(const std::string&);
    static avtFileFormatInterface *CreateInterface(
                       const char *const *list, int nList, int nBlock);

                       avtXDATFileFormat(const char *filename);
    virtual           ~avtXDATFileFormat() {;};

    virtual int            GetNTimesteps(void);

    virtual const char    *GetType(void)   { return "XDAT"; };
    virtual void           FreeUpResources(void); 

    virtual vtkDataSet    *GetMesh(int, const char *);
    virtual vtkDataArray  *GetVar(int, const char *);
    virtual vtkDataArray  *GetVectorVar(int, const char *);

  protected:
    virtual void           PopulateDatabaseMetaData(avtDatabaseMetaData *, int);

    std::vector<istream::pos_type>   file_positions;

    double unitCell[3][3];

    ifstream in;
    std::string filename;
    bool metadata_read;

    bool selective_dynamics;
    bool cartesian;
    double scale;
    double lat[3][3];
    
    int natoms;

    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;
    std::vector<int> species;

    std::vector<int> cx;
    std::vector<int> cy;
    std::vector<int> cz;

    std::vector<int> species_counts;
    std::vector<int> element_map;

    void OpenFileAtBeginning();
    void ReadMetaData();
    void ReadTimestep(int);
};


#endif
