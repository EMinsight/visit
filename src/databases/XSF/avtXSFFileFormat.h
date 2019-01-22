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
//                            avtXSFFileFormat.h                           //
// ************************************************************************* //

#ifndef AVT_XSF_FILE_FORMAT_H
#define AVT_XSF_FILE_FORMAT_H

#include <avtMTSDFileFormat.h>

#include <vector>
#include <string>


// ****************************************************************************
//  Class: avtXSFFileFormat
//
//  Purpose:
//      Reads in XSF files as a plugin to VisIt.
//
//  Programmer: js9 -- generated by xml2avt
//  Creation:   Wed Sep 28 10:30:07 PDT 2011
//
// ****************************************************************************

class avtXSFFileFormat : public avtMTSDFileFormat
{
  public:
                       avtXSFFileFormat(const char *);
    virtual           ~avtXSFFileFormat();

    //
    // This is used to return unconvention data -- ranging from material
    // information to information about block connectivity.
    //
    // virtual void      *GetAuxiliaryData(const char *var, int timestep, 
    //                                     const char *type, void *args, 
    //                                     DestructorFunction &);
    //

    //
    // If you know the times and cycle numbers, overload this function.
    // Otherwise, VisIt will make up some reasonable ones for you.
    //
    // virtual void        GetCycles(std::vector<int> &);
    // virtual void        GetTimes(std::vector<double> &);
    //

    virtual int            GetNTimesteps(void);

    virtual const char    *GetType(void)   { return "XSF"; };
    virtual void           FreeUpResources(void); 

    virtual vtkDataSet    *GetMesh(int, const char *);
    virtual vtkDataArray  *GetVar(int, const char *);
    virtual vtkDataArray  *GetVectorVar(int, const char *);

  protected:
    virtual void           PopulateDatabaseMetaData(avtDatabaseMetaData *, int);

    void OpenFileAtBeginning();
    void ReadAllMetaData();
    void ReadAtomsForTimestep(int);


  protected:
    struct Atom
    {
        int   e;
        float x;
        float y;
        float z; 
        float fx;
        float fy;
        float fz;
    };

    struct UCV
    {
        double v[3][3];
        double *operator[](int i) { return v[i]; }
    };

    struct Mesh
    {
        char  name[1024];
        int   dim;
        int   numnodes[3];
        float origin[3];
        UCV   ucv;
        std::vector<std::string>       var_names;
        std::vector<istream::pos_type> var_filepos;
        void clear()
        {
            dim         = -1;
            name[0]     = '\0';
            numnodes[0] = 0;
            numnodes[1] = 0;
            numnodes[2] = 0;
        }
    };

    ifstream         in;
    std::string      filename;
    bool             metadata_read;

    int              ntimesteps;
    int              natoms;
    bool             hasforces;

    std::vector<Atom> currentAtoms;

    std::vector<Mesh> allMeshes; // one per BLOCK_DATAGRID (i.e. a "mesh"

    std::vector<UCV> unitCell; // one, or one per timestep (or maybe none??)

    std::vector<istream::pos_type> atom_filepos; // one per timestep
};


#endif
