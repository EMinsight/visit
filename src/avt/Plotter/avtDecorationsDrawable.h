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
//                        avtDecorationsDrawable.h                           //
// ************************************************************************* //

#ifndef AVT_DECORATIONS_DRAWABLE_H
#define AVT_DECORATIONS_DRAWABLE_H

#include <plotter_exports.h>

#include <avtDrawable.h>
#include <avtLabelActor.h>


class     avtDecorationsMapper;


// ****************************************************************************
//  Class: avtDecorationsDrawable
//
//  Purpose:
//      A concrete type of avtDrawable, this allows for adding and removing
//      to/from a renderer the actors needed for decorations.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   July 12, 2002 
//
//  Modifications:
//    Kathleen Bonnell, Fri Jul 19 08:39:04 PDT 2002
//    Added method UpdateScaleFactor.
//
//    Kathleen Bonnell, Tue Aug 13 15:15:37 PDT 2002  
//    Added methods in support of lighting.
//
//    Mark C. Miller, Tue May 11 20:21:24 PDT 2004
//    Removed method to set externally rendered images actor
//
// ****************************************************************************

class PLOTTER_API avtDecorationsDrawable : public avtDrawable
{
  public:
                                avtDecorationsDrawable(std::vector<avtLabelActor_p> &);
    virtual                    ~avtDecorationsDrawable();

    void                        SetMapper(avtDecorationsMapper *);

    virtual bool                Interactive(void)  { return true; };

    virtual void                Add(vtkRenderer *);
    virtual void                Remove(vtkRenderer *);

    virtual void                VisibilityOn(void);
    virtual void                VisibilityOff(void);
    virtual int                 SetTransparencyActor(avtTransparencyActor *)
                                    { return -1; };

    virtual void                ShiftByVector(const double [3]);
    virtual void                ScaleByVector(const double [3]);
    virtual void                UpdateScaleFactor();

    virtual void                TurnLightingOn(void);
    virtual void                TurnLightingOff(void);
    virtual void                SetAmbientCoefficient(const double);

    virtual avtDataObject_p     GetDataObject(void);

  protected:
    std::vector<avtLabelActor_p> actors;
    vtkRenderer                 *renderer;
    avtDecorationsMapper        *mapper;
};


#endif


