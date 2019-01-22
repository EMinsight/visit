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
//                            avtHistogramPlot.h                             //
// ************************************************************************* //

#ifndef AVT_Histogram_PLOT_H
#define AVT_Histogram_PLOT_H


#include <avtLegend.h>
#include <avtPlot.h>

#include <HistogramAttributes.h>

class     avtHistogramFilter;
class     avtHistogramMapper;


// ****************************************************************************
//  Class:  avtHistogramPlot
//
//  Purpose:
//      A concrete type of avtPlot, this is the Histogram plot.
//
//  Programmer: childs -- generated by xml2info
//  Creation:   Thu Jun 26 10:33:56 PDT 2003
//
//  Modifications:
//    Kathleen Biagas, Wed May 11 11:48:15 MST 2016
//    Use avtSimpleMapper instead of custom renderer.
//
// ****************************************************************************

class avtHistogramPlot : public avtLineDataPlot
{
  public:
                                avtHistogramPlot();
    virtual                    ~avtHistogramPlot();

    virtual const char         *GetName(void) { return "HistogramPlot"; };

    static avtPlot             *Create();
    virtual void                ReleaseData(void);

    virtual void                SetAtts(const AttributeGroup*);
    virtual bool                SetForegroundColor(const double *);

  protected:
    HistogramAttributes         atts;

    avtHistogramMapper         *mapper;
    avtHistogramFilter         *HistogramFilter;
    avtFilter                  *amountFilter;
    double                      fgColor[3];

    virtual avtMapperBase      *GetMapper(void);
    virtual avtDataObject_p     ApplyOperators(avtDataObject_p);
    virtual avtDataObject_p     ApplyRenderingTransformation(avtDataObject_p);
    virtual void                CustomizeBehavior(void);

    virtual avtLegend_p         GetLegend(void) { return NULL; };
};


#endif
