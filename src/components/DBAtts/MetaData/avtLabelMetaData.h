/*****************************************************************************
*
* Copyright (c) 2000 - 2007, The Regents of the University of California
* Produced at the Lawrence Livermore National Laboratory
* All rights reserved.
*
* This file is part of VisIt. For details, see http://www.llnl.gov/visit/. The
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
*    documentation and/or materials provided with the distribution.
*  - Neither the name of the UC/LLNL nor  the names of its contributors may be
*    used to  endorse or  promote products derived from  this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  REGENTS  OF  THE  UNIVERSITY OF
* CALIFORNIA, THE U.S.  DEPARTMENT  OF  ENERGY OR CONTRIBUTORS BE  LIABLE  FOR
* ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

#ifndef AVTLABELMETADATA_H
#define AVTLABELMETADATA_H
#include <dbatts_exports.h>
#include <string>
#include <avtTypes.h>
#include <AttributeSubject.h>

// ****************************************************************************
// Class: avtLabelMetaData
//
// Purpose:
//    Contains label metadata attributes
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   Fri Mar 9 16:13:16 PST 2007
//
// Modifications:
//   
// ****************************************************************************

class DBATTS_API avtLabelMetaData : public AttributeSubject
{
public:
    avtLabelMetaData();
    avtLabelMetaData(const avtLabelMetaData &obj);
    virtual ~avtLabelMetaData();

    virtual avtLabelMetaData& operator = (const avtLabelMetaData &obj);
    virtual bool operator == (const avtLabelMetaData &obj) const;
    virtual bool operator != (const avtLabelMetaData &obj) const;

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();

    // User-defined methods
    avtLabelMetaData(const std::string &, const std::string &, avtCentering);
    void Print(ostream &, int = 0) const;
public:
    std::string  name;
    std::string  originalName;
    bool         validVariable;
    std::string  meshName;
    avtCentering centering;

private:
    // Static class format string for type map.
    static const char *TypeMapFormatString;
};

#endif
