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

#ifndef SELECTIONSUMMARY_H
#define SELECTIONSUMMARY_H
#include <state_exports.h>
#include <string>
#include <AttributeSubject.h>

class SelectionVariableSummary;

// ****************************************************************************
// Class: SelectionSummary
//
// Purpose:
//    Contains attributes for a selection
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

class STATE_API SelectionSummary : public AttributeSubject
{
public:
    // These constructors are for objects of this class
    SelectionSummary();
    SelectionSummary(const SelectionSummary &obj);
protected:
    // These constructors are for objects derived from this class
    SelectionSummary(private_tmfs_t tmfs);
    SelectionSummary(const SelectionSummary &obj, private_tmfs_t tmfs);
public:
    virtual ~SelectionSummary();

    virtual SelectionSummary& operator = (const SelectionSummary &obj);
    virtual bool operator == (const SelectionSummary &obj) const;
    virtual bool operator != (const SelectionSummary &obj) const;
private:
    void Init();
    void Copy(const SelectionSummary &obj);
public:

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();
    void SelectName();
    void SelectVariables();
    void SelectHistogramValues();

    // Property setting methods
    void SetName(const std::string &name_);
    void SetCellCount(int cellCount_);
    void SetTotalCellCount(int totalCellCount_);
    void SetHistogramValues(const doubleVector &histogramValues_);
    void SetHistogramMinBin(double histogramMinBin_);
    void SetHistogramMaxBin(double histogramMaxBin_);

    // Property getting methods
    const std::string  &GetName() const;
          std::string  &GetName();
    const AttributeGroupVector &GetVariables() const;
          AttributeGroupVector &GetVariables();
    int                GetCellCount() const;
    int                GetTotalCellCount() const;
    const doubleVector &GetHistogramValues() const;
          doubleVector &GetHistogramValues();
    double             GetHistogramMinBin() const;
    double             GetHistogramMaxBin() const;

    // Persistence methods
    virtual bool CreateNode(DataNode *node, bool completeSave, bool forceAdd);
    virtual void SetFromNode(DataNode *node);


    // Attributegroup convenience methods
    void AddVariables(const SelectionVariableSummary &);
    void ClearVariables();
    void RemoveVariables(int i);
    int  GetNumVariables() const;
    SelectionVariableSummary &GetVariables(int i);
    const SelectionVariableSummary &GetVariables(int i) const;

    SelectionVariableSummary &operator [] (int i);
    const SelectionVariableSummary &operator [] (int i) const;


    // Keyframing methods
    virtual std::string               GetFieldName(int index) const;
    virtual AttributeGroup::FieldType GetFieldType(int index) const;
    virtual std::string               GetFieldTypeName(int index) const;
    virtual bool                      FieldsEqual(int index, const AttributeGroup *rhs) const;


    // IDs that can be used to identify fields in case statements
    enum {
        ID_name = 0,
        ID_variables,
        ID_cellCount,
        ID_totalCellCount,
        ID_histogramValues,
        ID_histogramMinBin,
        ID_histogramMaxBin,
        ID__LAST
    };

protected:
    AttributeGroup *CreateSubAttributeGroup(int index);
private:
    std::string          name;
    AttributeGroupVector variables;
    int                  cellCount;
    int                  totalCellCount;
    doubleVector         histogramValues;
    double               histogramMinBin;
    double               histogramMaxBin;

    // Static class format string for type map.
    static const char *TypeMapFormatString;
    static const private_tmfs_t TmfsStruct;
};
#define SELECTIONSUMMARY_TMFS "sa*iid*dd"

#endif
