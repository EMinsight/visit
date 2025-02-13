// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <LabelAttributes.h>
#include <DataNode.h>

//
// Enum conversion methods for LabelAttributes::LabelIndexDisplay
//

static const char *LabelIndexDisplay_strings[] = {
"Natural", "LogicalIndex", "Index"
};

std::string
LabelAttributes::LabelIndexDisplay_ToString(LabelAttributes::LabelIndexDisplay t)
{
    int index = int(t);
    if(index < 0 || index >= 3) index = 0;
    return LabelIndexDisplay_strings[index];
}

std::string
LabelAttributes::LabelIndexDisplay_ToString(int t)
{
    int index = (t < 0 || t >= 3) ? 0 : t;
    return LabelIndexDisplay_strings[index];
}

bool
LabelAttributes::LabelIndexDisplay_FromString(const std::string &s, LabelAttributes::LabelIndexDisplay &val)
{
    val = LabelAttributes::Natural;
    for(int i = 0; i < 3; ++i)
    {
        if(s == LabelIndexDisplay_strings[i])
        {
            val = (LabelIndexDisplay)i;
            return true;
        }
    }
    return false;
}

//
// Enum conversion methods for LabelAttributes::LabelHorizontalAlignment
//

static const char *LabelHorizontalAlignment_strings[] = {
"HCenter", "Left", "Right"
};

std::string
LabelAttributes::LabelHorizontalAlignment_ToString(LabelAttributes::LabelHorizontalAlignment t)
{
    int index = int(t);
    if(index < 0 || index >= 3) index = 0;
    return LabelHorizontalAlignment_strings[index];
}

std::string
LabelAttributes::LabelHorizontalAlignment_ToString(int t)
{
    int index = (t < 0 || t >= 3) ? 0 : t;
    return LabelHorizontalAlignment_strings[index];
}

bool
LabelAttributes::LabelHorizontalAlignment_FromString(const std::string &s, LabelAttributes::LabelHorizontalAlignment &val)
{
    val = LabelAttributes::HCenter;
    for(int i = 0; i < 3; ++i)
    {
        if(s == LabelHorizontalAlignment_strings[i])
        {
            val = (LabelHorizontalAlignment)i;
            return true;
        }
    }
    return false;
}

//
// Enum conversion methods for LabelAttributes::LabelVerticalAlignment
//

static const char *LabelVerticalAlignment_strings[] = {
"VCenter", "Top", "Bottom"
};

std::string
LabelAttributes::LabelVerticalAlignment_ToString(LabelAttributes::LabelVerticalAlignment t)
{
    int index = int(t);
    if(index < 0 || index >= 3) index = 0;
    return LabelVerticalAlignment_strings[index];
}

std::string
LabelAttributes::LabelVerticalAlignment_ToString(int t)
{
    int index = (t < 0 || t >= 3) ? 0 : t;
    return LabelVerticalAlignment_strings[index];
}

bool
LabelAttributes::LabelVerticalAlignment_FromString(const std::string &s, LabelAttributes::LabelVerticalAlignment &val)
{
    val = LabelAttributes::VCenter;
    for(int i = 0; i < 3; ++i)
    {
        if(s == LabelVerticalAlignment_strings[i])
        {
            val = (LabelVerticalAlignment)i;
            return true;
        }
    }
    return false;
}

//
// Enum conversion methods for LabelAttributes::LabelDrawFacing
//

static const char *LabelDrawFacing_strings[] = {
"Front", "Back", "FrontAndBack"
};

std::string
LabelAttributes::LabelDrawFacing_ToString(LabelAttributes::LabelDrawFacing t)
{
    int index = int(t);
    if(index < 0 || index >= 3) index = 0;
    return LabelDrawFacing_strings[index];
}

std::string
LabelAttributes::LabelDrawFacing_ToString(int t)
{
    int index = (t < 0 || t >= 3) ? 0 : t;
    return LabelDrawFacing_strings[index];
}

bool
LabelAttributes::LabelDrawFacing_FromString(const std::string &s, LabelAttributes::LabelDrawFacing &val)
{
    val = LabelAttributes::Front;
    for(int i = 0; i < 3; ++i)
    {
        if(s == LabelDrawFacing_strings[i])
        {
            val = (LabelDrawFacing)i;
            return true;
        }
    }
    return false;
}

//
// Enum conversion methods for LabelAttributes::VariableType
//

static const char *VariableType_strings[] = {
"LABEL_VT_MESH", "LABEL_VT_SCALAR_VAR", "LABEL_VT_VECTOR_VAR",
"LABEL_VT_TENSOR_VAR", "LABEL_VT_SYMMETRIC_TENSOR_VAR", "LABEL_VT_ARRAY_VAR",
"LABEL_VT_LABEL_VAR", "LABEL_VT_MATERIAL", "LABEL_VT_SUBSET",
"LABEL_VT_UNKNOWN_TYPE"};

std::string
LabelAttributes::VariableType_ToString(LabelAttributes::VariableType t)
{
    int index = int(t);
    if(index < 0 || index >= 10) index = 0;
    return VariableType_strings[index];
}

std::string
LabelAttributes::VariableType_ToString(int t)
{
    int index = (t < 0 || t >= 10) ? 0 : t;
    return VariableType_strings[index];
}

bool
LabelAttributes::VariableType_FromString(const std::string &s, LabelAttributes::VariableType &val)
{
    val = LabelAttributes::LABEL_VT_MESH;
    for(int i = 0; i < 10; ++i)
    {
        if(s == VariableType_strings[i])
        {
            val = (VariableType)i;
            return true;
        }
    }
    return false;
}

//
// Enum conversion methods for LabelAttributes::DepthTestMode
//

static const char *DepthTestMode_strings[] = {
"LABEL_DT_AUTO", "LABEL_DT_ALWAYS", "LABEL_DT_NEVER"
};

std::string
LabelAttributes::DepthTestMode_ToString(LabelAttributes::DepthTestMode t)
{
    int index = int(t);
    if(index < 0 || index >= 3) index = 0;
    return DepthTestMode_strings[index];
}

std::string
LabelAttributes::DepthTestMode_ToString(int t)
{
    int index = (t < 0 || t >= 3) ? 0 : t;
    return DepthTestMode_strings[index];
}

bool
LabelAttributes::DepthTestMode_FromString(const std::string &s, LabelAttributes::DepthTestMode &val)
{
    val = LabelAttributes::LABEL_DT_AUTO;
    for(int i = 0; i < 3; ++i)
    {
        if(s == DepthTestMode_strings[i])
        {
            val = (DepthTestMode)i;
            return true;
        }
    }
    return false;
}

// ****************************************************************************
// Method: LabelAttributes::LabelAttributes
//
// Purpose:
//   Init utility for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

void LabelAttributes::Init()
{
    varType = LABEL_VT_UNKNOWN_TYPE;
    legendFlag = true;
    showNodes = false;
    showCells = true;
    restrictNumberOfLabels = true;
    drawLabelsFacing = Front;
    labelDisplayFormat = Natural;
    numberOfLabels = 200;
    textFont1.GetColor().SetRgb(255, 0, 0);
    textFont1.SetScale(4);
    textFont2.GetColor().SetRgb(0, 0, 255);
    textFont2.SetScale(4);
    horizontalJustification = HCenter;
    verticalJustification = VCenter;
    depthTestMode = LABEL_DT_AUTO;
    formatTemplate = "%g";

    LabelAttributes::SelectAll();
}

// ****************************************************************************
// Method: LabelAttributes::LabelAttributes
//
// Purpose:
//   Copy utility for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

void LabelAttributes::Copy(const LabelAttributes &obj)
{
    varType = obj.varType;
    legendFlag = obj.legendFlag;
    showNodes = obj.showNodes;
    showCells = obj.showCells;
    restrictNumberOfLabels = obj.restrictNumberOfLabels;
    drawLabelsFacing = obj.drawLabelsFacing;
    labelDisplayFormat = obj.labelDisplayFormat;
    numberOfLabels = obj.numberOfLabels;
    textFont1 = obj.textFont1;
    textFont2 = obj.textFont2;
    horizontalJustification = obj.horizontalJustification;
    verticalJustification = obj.verticalJustification;
    depthTestMode = obj.depthTestMode;
    formatTemplate = obj.formatTemplate;

    LabelAttributes::SelectAll();
}

// Type map format string
const char *LabelAttributes::TypeMapFormatString = LABELATTRIBUTES_TMFS;
const AttributeGroup::private_tmfs_t LabelAttributes::TmfsStruct = {LABELATTRIBUTES_TMFS};


// ****************************************************************************
// Method: LabelAttributes::LabelAttributes
//
// Purpose:
//   Default constructor for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

LabelAttributes::LabelAttributes() :
    AttributeSubject(LabelAttributes::TypeMapFormatString)
{
    LabelAttributes::Init();
}

// ****************************************************************************
// Method: LabelAttributes::LabelAttributes
//
// Purpose:
//   Constructor for the derived classes of LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

LabelAttributes::LabelAttributes(private_tmfs_t tmfs) :
    AttributeSubject(tmfs.tmfs)
{
    LabelAttributes::Init();
}

// ****************************************************************************
// Method: LabelAttributes::LabelAttributes
//
// Purpose:
//   Copy constructor for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

LabelAttributes::LabelAttributes(const LabelAttributes &obj) :
    AttributeSubject(LabelAttributes::TypeMapFormatString)
{
    LabelAttributes::Copy(obj);
}

// ****************************************************************************
// Method: LabelAttributes::LabelAttributes
//
// Purpose:
//   Copy constructor for derived classes of the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

LabelAttributes::LabelAttributes(const LabelAttributes &obj, private_tmfs_t tmfs) :
    AttributeSubject(tmfs.tmfs)
{
    LabelAttributes::Copy(obj);
}

// ****************************************************************************
// Method: LabelAttributes::~LabelAttributes
//
// Purpose:
//   Destructor for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

LabelAttributes::~LabelAttributes()
{
    // nothing here
}

// ****************************************************************************
// Method: LabelAttributes::operator =
//
// Purpose:
//   Assignment operator for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

LabelAttributes&
LabelAttributes::operator = (const LabelAttributes &obj)
{
    if (this == &obj) return *this;

    LabelAttributes::Copy(obj);

    return *this;
}

// ****************************************************************************
// Method: LabelAttributes::operator ==
//
// Purpose:
//   Comparison operator == for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

bool
LabelAttributes::operator == (const LabelAttributes &obj) const
{
    // Create the return value
    return (true /* can ignore varType */ &&
            (legendFlag == obj.legendFlag) &&
            (showNodes == obj.showNodes) &&
            (showCells == obj.showCells) &&
            (restrictNumberOfLabels == obj.restrictNumberOfLabels) &&
            (drawLabelsFacing == obj.drawLabelsFacing) &&
            (labelDisplayFormat == obj.labelDisplayFormat) &&
            (numberOfLabels == obj.numberOfLabels) &&
            (textFont1 == obj.textFont1) &&
            (textFont2 == obj.textFont2) &&
            (horizontalJustification == obj.horizontalJustification) &&
            (verticalJustification == obj.verticalJustification) &&
            (depthTestMode == obj.depthTestMode) &&
            (formatTemplate == obj.formatTemplate));
}

// ****************************************************************************
// Method: LabelAttributes::operator !=
//
// Purpose:
//   Comparison operator != for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

bool
LabelAttributes::operator != (const LabelAttributes &obj) const
{
    return !(this->operator == (obj));
}

// ****************************************************************************
// Method: LabelAttributes::TypeName
//
// Purpose:
//   Type name method for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

const std::string
LabelAttributes::TypeName() const
{
    return "LabelAttributes";
}

// ****************************************************************************
// Method: LabelAttributes::CopyAttributes
//
// Purpose:
//   CopyAttributes method for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

bool
LabelAttributes::CopyAttributes(const AttributeGroup *atts)
{
    if(TypeName() != atts->TypeName())
        return false;

    // Call assignment operator.
    const LabelAttributes *tmp = (const LabelAttributes *)atts;
    *this = *tmp;

    return true;
}

// ****************************************************************************
// Method: LabelAttributes::CreateCompatible
//
// Purpose:
//   CreateCompatible method for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

AttributeSubject *
LabelAttributes::CreateCompatible(const std::string &tname) const
{
    AttributeSubject *retval = 0;
    if(TypeName() == tname)
        retval = new LabelAttributes(*this);
    // Other cases could go here too.

    return retval;
}

// ****************************************************************************
// Method: LabelAttributes::NewInstance
//
// Purpose:
//   NewInstance method for the LabelAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

AttributeSubject *
LabelAttributes::NewInstance(bool copy) const
{
    AttributeSubject *retval = 0;
    if(copy)
        retval = new LabelAttributes(*this);
    else
        retval = new LabelAttributes;

    return retval;
}

// ****************************************************************************
// Method: LabelAttributes::SelectAll
//
// Purpose:
//   Selects all attributes.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

void
LabelAttributes::SelectAll()
{
    Select(ID_varType,                 (void *)&varType);
    Select(ID_legendFlag,              (void *)&legendFlag);
    Select(ID_showNodes,               (void *)&showNodes);
    Select(ID_showCells,               (void *)&showCells);
    Select(ID_restrictNumberOfLabels,  (void *)&restrictNumberOfLabels);
    Select(ID_drawLabelsFacing,        (void *)&drawLabelsFacing);
    Select(ID_labelDisplayFormat,      (void *)&labelDisplayFormat);
    Select(ID_numberOfLabels,          (void *)&numberOfLabels);
    Select(ID_textFont1,               (void *)&textFont1);
    Select(ID_textFont2,               (void *)&textFont2);
    Select(ID_horizontalJustification, (void *)&horizontalJustification);
    Select(ID_verticalJustification,   (void *)&verticalJustification);
    Select(ID_depthTestMode,           (void *)&depthTestMode);
    Select(ID_formatTemplate,          (void *)&formatTemplate);
}

///////////////////////////////////////////////////////////////////////////////
// Persistence methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: LabelAttributes::CreateNode
//
// Purpose:
//   This method creates a DataNode representation of the object so it can be saved to a config file.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

bool
LabelAttributes::CreateNode(DataNode *parentNode, bool completeSave, bool forceAdd)
{
    if(parentNode == 0)
        return false;

    LabelAttributes defaultObject;
    bool addToParent = false;
    // Create a node for LabelAttributes.
    DataNode *node = new DataNode("LabelAttributes");

    if(completeSave || !FieldsEqual(ID_varType, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("varType", VariableType_ToString(varType)));
    }

    if(completeSave || !FieldsEqual(ID_legendFlag, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("legendFlag", legendFlag));
    }

    if(completeSave || !FieldsEqual(ID_showNodes, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("showNodes", showNodes));
    }

    if(completeSave || !FieldsEqual(ID_showCells, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("showCells", showCells));
    }

    if(completeSave || !FieldsEqual(ID_restrictNumberOfLabels, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("restrictNumberOfLabels", restrictNumberOfLabels));
    }

    if(completeSave || !FieldsEqual(ID_drawLabelsFacing, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("drawLabelsFacing", LabelDrawFacing_ToString(drawLabelsFacing)));
    }

    if(completeSave || !FieldsEqual(ID_labelDisplayFormat, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("labelDisplayFormat", LabelIndexDisplay_ToString(labelDisplayFormat)));
    }

    if(completeSave || !FieldsEqual(ID_numberOfLabels, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("numberOfLabels", numberOfLabels));
    }

    if(completeSave || !FieldsEqual(ID_textFont1, &defaultObject))
    {
        DataNode *textFont1Node = new DataNode("textFont1");
        if(textFont1.CreateNode(textFont1Node, completeSave, false))
        {
            addToParent = true;
            node->AddNode(textFont1Node);
        }
        else
            delete textFont1Node;
    }

    if(completeSave || !FieldsEqual(ID_textFont2, &defaultObject))
    {
        DataNode *textFont2Node = new DataNode("textFont2");
        if(textFont2.CreateNode(textFont2Node, completeSave, false))
        {
            addToParent = true;
            node->AddNode(textFont2Node);
        }
        else
            delete textFont2Node;
    }

    if(completeSave || !FieldsEqual(ID_horizontalJustification, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("horizontalJustification", LabelHorizontalAlignment_ToString(horizontalJustification)));
    }

    if(completeSave || !FieldsEqual(ID_verticalJustification, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("verticalJustification", LabelVerticalAlignment_ToString(verticalJustification)));
    }

    if(completeSave || !FieldsEqual(ID_depthTestMode, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("depthTestMode", DepthTestMode_ToString(depthTestMode)));
    }

    if(completeSave || !FieldsEqual(ID_formatTemplate, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("formatTemplate", formatTemplate));
    }


    // Add the node to the parent node.
    if(addToParent || forceAdd)
        parentNode->AddNode(node);
    else
        delete node;

    return (addToParent || forceAdd);
}

// ****************************************************************************
// Method: LabelAttributes::SetFromNode
//
// Purpose:
//   This method sets attributes in this object from values in a DataNode representation of the object.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

void
LabelAttributes::SetFromNode(DataNode *parentNode)
{
    if(parentNode == 0)
        return;

    DataNode *searchNode = parentNode->GetNode("LabelAttributes");
    if(searchNode == 0)
        return;

    DataNode *node;
    if((node = searchNode->GetNode("varType")) != 0)
    {
        // Allow enums to be int or string in the config file
        if(node->GetNodeType() == INT_NODE)
        {
            int ival = node->AsInt();
            if(ival >= 0 && ival < 10)
                SetVarType(VariableType(ival));
        }
        else if(node->GetNodeType() == STRING_NODE)
        {
            VariableType value;
            if(VariableType_FromString(node->AsString(), value))
                SetVarType(value);
        }
    }
    if((node = searchNode->GetNode("legendFlag")) != 0)
        SetLegendFlag(node->AsBool());
    if((node = searchNode->GetNode("showNodes")) != 0)
        SetShowNodes(node->AsBool());
    if((node = searchNode->GetNode("showCells")) != 0)
        SetShowCells(node->AsBool());
    if((node = searchNode->GetNode("restrictNumberOfLabels")) != 0)
        SetRestrictNumberOfLabels(node->AsBool());
    if((node = searchNode->GetNode("drawLabelsFacing")) != 0)
    {
        // Allow enums to be int or string in the config file
        if(node->GetNodeType() == INT_NODE)
        {
            int ival = node->AsInt();
            if(ival >= 0 && ival < 3)
                SetDrawLabelsFacing(LabelDrawFacing(ival));
        }
        else if(node->GetNodeType() == STRING_NODE)
        {
            LabelDrawFacing value;
            if(LabelDrawFacing_FromString(node->AsString(), value))
                SetDrawLabelsFacing(value);
        }
    }
    if((node = searchNode->GetNode("labelDisplayFormat")) != 0)
    {
        // Allow enums to be int or string in the config file
        if(node->GetNodeType() == INT_NODE)
        {
            int ival = node->AsInt();
            if(ival >= 0 && ival < 3)
                SetLabelDisplayFormat(LabelIndexDisplay(ival));
        }
        else if(node->GetNodeType() == STRING_NODE)
        {
            LabelIndexDisplay value;
            if(LabelIndexDisplay_FromString(node->AsString(), value))
                SetLabelDisplayFormat(value);
        }
    }
    if((node = searchNode->GetNode("numberOfLabels")) != 0)
        SetNumberOfLabels(node->AsInt());
    if((node = searchNode->GetNode("textFont1")) != 0)
        textFont1.SetFromNode(node);
    if((node = searchNode->GetNode("textFont2")) != 0)
        textFont2.SetFromNode(node);
    if((node = searchNode->GetNode("horizontalJustification")) != 0)
    {
        // Allow enums to be int or string in the config file
        if(node->GetNodeType() == INT_NODE)
        {
            int ival = node->AsInt();
            if(ival >= 0 && ival < 3)
                SetHorizontalJustification(LabelHorizontalAlignment(ival));
        }
        else if(node->GetNodeType() == STRING_NODE)
        {
            LabelHorizontalAlignment value;
            if(LabelHorizontalAlignment_FromString(node->AsString(), value))
                SetHorizontalJustification(value);
        }
    }
    if((node = searchNode->GetNode("verticalJustification")) != 0)
    {
        // Allow enums to be int or string in the config file
        if(node->GetNodeType() == INT_NODE)
        {
            int ival = node->AsInt();
            if(ival >= 0 && ival < 3)
                SetVerticalJustification(LabelVerticalAlignment(ival));
        }
        else if(node->GetNodeType() == STRING_NODE)
        {
            LabelVerticalAlignment value;
            if(LabelVerticalAlignment_FromString(node->AsString(), value))
                SetVerticalJustification(value);
        }
    }
    if((node = searchNode->GetNode("depthTestMode")) != 0)
    {
        // Allow enums to be int or string in the config file
        if(node->GetNodeType() == INT_NODE)
        {
            int ival = node->AsInt();
            if(ival >= 0 && ival < 3)
                SetDepthTestMode(DepthTestMode(ival));
        }
        else if(node->GetNodeType() == STRING_NODE)
        {
            DepthTestMode value;
            if(DepthTestMode_FromString(node->AsString(), value))
                SetDepthTestMode(value);
        }
    }
    if((node = searchNode->GetNode("formatTemplate")) != 0)
        SetFormatTemplate(node->AsString());
}

///////////////////////////////////////////////////////////////////////////////
// Set property methods
///////////////////////////////////////////////////////////////////////////////

void
LabelAttributes::SetVarType(LabelAttributes::VariableType varType_)
{
    varType = varType_;
    Select(ID_varType, (void *)&varType);
}

void
LabelAttributes::SetLegendFlag(bool legendFlag_)
{
    legendFlag = legendFlag_;
    Select(ID_legendFlag, (void *)&legendFlag);
}

void
LabelAttributes::SetShowNodes(bool showNodes_)
{
    showNodes = showNodes_;
    Select(ID_showNodes, (void *)&showNodes);
}

void
LabelAttributes::SetShowCells(bool showCells_)
{
    showCells = showCells_;
    Select(ID_showCells, (void *)&showCells);
}

void
LabelAttributes::SetRestrictNumberOfLabels(bool restrictNumberOfLabels_)
{
    restrictNumberOfLabels = restrictNumberOfLabels_;
    Select(ID_restrictNumberOfLabels, (void *)&restrictNumberOfLabels);
}

void
LabelAttributes::SetDrawLabelsFacing(LabelAttributes::LabelDrawFacing drawLabelsFacing_)
{
    drawLabelsFacing = drawLabelsFacing_;
    Select(ID_drawLabelsFacing, (void *)&drawLabelsFacing);
}

void
LabelAttributes::SetLabelDisplayFormat(LabelAttributes::LabelIndexDisplay labelDisplayFormat_)
{
    labelDisplayFormat = labelDisplayFormat_;
    Select(ID_labelDisplayFormat, (void *)&labelDisplayFormat);
}

void
LabelAttributes::SetNumberOfLabels(int numberOfLabels_)
{
    numberOfLabels = numberOfLabels_;
    Select(ID_numberOfLabels, (void *)&numberOfLabels);
}

void
LabelAttributes::SetTextFont1(const FontAttributes &textFont1_)
{
    textFont1 = textFont1_;
    Select(ID_textFont1, (void *)&textFont1);
}

void
LabelAttributes::SetTextFont2(const FontAttributes &textFont2_)
{
    textFont2 = textFont2_;
    Select(ID_textFont2, (void *)&textFont2);
}

void
LabelAttributes::SetHorizontalJustification(LabelAttributes::LabelHorizontalAlignment horizontalJustification_)
{
    horizontalJustification = horizontalJustification_;
    Select(ID_horizontalJustification, (void *)&horizontalJustification);
}

void
LabelAttributes::SetVerticalJustification(LabelAttributes::LabelVerticalAlignment verticalJustification_)
{
    verticalJustification = verticalJustification_;
    Select(ID_verticalJustification, (void *)&verticalJustification);
}

void
LabelAttributes::SetDepthTestMode(LabelAttributes::DepthTestMode depthTestMode_)
{
    depthTestMode = depthTestMode_;
    Select(ID_depthTestMode, (void *)&depthTestMode);
}

void
LabelAttributes::SetFormatTemplate(const std::string &formatTemplate_)
{
    formatTemplate = formatTemplate_;
    Select(ID_formatTemplate, (void *)&formatTemplate);
}

///////////////////////////////////////////////////////////////////////////////
// Get property methods
///////////////////////////////////////////////////////////////////////////////

LabelAttributes::VariableType
LabelAttributes::GetVarType() const
{
    return VariableType(varType);
}

bool
LabelAttributes::GetLegendFlag() const
{
    return legendFlag;
}

bool
LabelAttributes::GetShowNodes() const
{
    return showNodes;
}

bool
LabelAttributes::GetShowCells() const
{
    return showCells;
}

bool
LabelAttributes::GetRestrictNumberOfLabels() const
{
    return restrictNumberOfLabels;
}

LabelAttributes::LabelDrawFacing
LabelAttributes::GetDrawLabelsFacing() const
{
    return LabelDrawFacing(drawLabelsFacing);
}

LabelAttributes::LabelIndexDisplay
LabelAttributes::GetLabelDisplayFormat() const
{
    return LabelIndexDisplay(labelDisplayFormat);
}

int
LabelAttributes::GetNumberOfLabels() const
{
    return numberOfLabels;
}

const FontAttributes &
LabelAttributes::GetTextFont1() const
{
    return textFont1;
}

FontAttributes &
LabelAttributes::GetTextFont1()
{
    return textFont1;
}

const FontAttributes &
LabelAttributes::GetTextFont2() const
{
    return textFont2;
}

FontAttributes &
LabelAttributes::GetTextFont2()
{
    return textFont2;
}

LabelAttributes::LabelHorizontalAlignment
LabelAttributes::GetHorizontalJustification() const
{
    return LabelHorizontalAlignment(horizontalJustification);
}

LabelAttributes::LabelVerticalAlignment
LabelAttributes::GetVerticalJustification() const
{
    return LabelVerticalAlignment(verticalJustification);
}

LabelAttributes::DepthTestMode
LabelAttributes::GetDepthTestMode() const
{
    return DepthTestMode(depthTestMode);
}

const std::string &
LabelAttributes::GetFormatTemplate() const
{
    return formatTemplate;
}

std::string &
LabelAttributes::GetFormatTemplate()
{
    return formatTemplate;
}

///////////////////////////////////////////////////////////////////////////////
// Select property methods
///////////////////////////////////////////////////////////////////////////////

void
LabelAttributes::SelectTextFont1()
{
    Select(ID_textFont1, (void *)&textFont1);
}

void
LabelAttributes::SelectTextFont2()
{
    Select(ID_textFont2, (void *)&textFont2);
}

void
LabelAttributes::SelectFormatTemplate()
{
    Select(ID_formatTemplate, (void *)&formatTemplate);
}

///////////////////////////////////////////////////////////////////////////////
// Keyframing methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: LabelAttributes::GetFieldName
//
// Purpose:
//   This method returns the name of a field given its index.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

std::string
LabelAttributes::GetFieldName(int index) const
{
    switch (index)
    {
    case ID_varType:                 return "varType";
    case ID_legendFlag:              return "legendFlag";
    case ID_showNodes:               return "showNodes";
    case ID_showCells:               return "showCells";
    case ID_restrictNumberOfLabels:  return "restrictNumberOfLabels";
    case ID_drawLabelsFacing:        return "drawLabelsFacing";
    case ID_labelDisplayFormat:      return "labelDisplayFormat";
    case ID_numberOfLabels:          return "numberOfLabels";
    case ID_textFont1:               return "textFont1";
    case ID_textFont2:               return "textFont2";
    case ID_horizontalJustification: return "horizontalJustification";
    case ID_verticalJustification:   return "verticalJustification";
    case ID_depthTestMode:           return "depthTestMode";
    case ID_formatTemplate:          return "formatTemplate";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: LabelAttributes::GetFieldType
//
// Purpose:
//   This method returns the type of a field given its index.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

AttributeGroup::FieldType
LabelAttributes::GetFieldType(int index) const
{
    switch (index)
    {
    case ID_varType:                 return FieldType_enum;
    case ID_legendFlag:              return FieldType_bool;
    case ID_showNodes:               return FieldType_bool;
    case ID_showCells:               return FieldType_bool;
    case ID_restrictNumberOfLabels:  return FieldType_bool;
    case ID_drawLabelsFacing:        return FieldType_enum;
    case ID_labelDisplayFormat:      return FieldType_enum;
    case ID_numberOfLabels:          return FieldType_int;
    case ID_textFont1:               return FieldType_att;
    case ID_textFont2:               return FieldType_att;
    case ID_horizontalJustification: return FieldType_enum;
    case ID_verticalJustification:   return FieldType_enum;
    case ID_depthTestMode:           return FieldType_enum;
    case ID_formatTemplate:          return FieldType_string;
    default:  return FieldType_unknown;
    }
}

// ****************************************************************************
// Method: LabelAttributes::GetFieldTypeName
//
// Purpose:
//   This method returns the name of a field type given its index.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

std::string
LabelAttributes::GetFieldTypeName(int index) const
{
    switch (index)
    {
    case ID_varType:                 return "enum";
    case ID_legendFlag:              return "bool";
    case ID_showNodes:               return "bool";
    case ID_showCells:               return "bool";
    case ID_restrictNumberOfLabels:  return "bool";
    case ID_drawLabelsFacing:        return "enum";
    case ID_labelDisplayFormat:      return "enum";
    case ID_numberOfLabels:          return "int";
    case ID_textFont1:               return "att";
    case ID_textFont2:               return "att";
    case ID_horizontalJustification: return "enum";
    case ID_verticalJustification:   return "enum";
    case ID_depthTestMode:           return "enum";
    case ID_formatTemplate:          return "string";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: LabelAttributes::FieldsEqual
//
// Purpose:
//   This method compares two fields and return true if they are equal.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

bool
LabelAttributes::FieldsEqual(int index_, const AttributeGroup *rhs) const
{
    const LabelAttributes &obj = *((const LabelAttributes*)rhs);
    bool retval = false;
    switch (index_)
    {
    case ID_varType:
        {  // new scope
        retval = (varType == obj.varType);
        }
        break;
    case ID_legendFlag:
        {  // new scope
        retval = (legendFlag == obj.legendFlag);
        }
        break;
    case ID_showNodes:
        {  // new scope
        retval = (showNodes == obj.showNodes);
        }
        break;
    case ID_showCells:
        {  // new scope
        retval = (showCells == obj.showCells);
        }
        break;
    case ID_restrictNumberOfLabels:
        {  // new scope
        retval = (restrictNumberOfLabels == obj.restrictNumberOfLabels);
        }
        break;
    case ID_drawLabelsFacing:
        {  // new scope
        retval = (drawLabelsFacing == obj.drawLabelsFacing);
        }
        break;
    case ID_labelDisplayFormat:
        {  // new scope
        retval = (labelDisplayFormat == obj.labelDisplayFormat);
        }
        break;
    case ID_numberOfLabels:
        {  // new scope
        retval = (numberOfLabels == obj.numberOfLabels);
        }
        break;
    case ID_textFont1:
        {  // new scope
        retval = (textFont1 == obj.textFont1);
        }
        break;
    case ID_textFont2:
        {  // new scope
        retval = (textFont2 == obj.textFont2);
        }
        break;
    case ID_horizontalJustification:
        {  // new scope
        retval = (horizontalJustification == obj.horizontalJustification);
        }
        break;
    case ID_verticalJustification:
        {  // new scope
        retval = (verticalJustification == obj.verticalJustification);
        }
        break;
    case ID_depthTestMode:
        {  // new scope
        retval = (depthTestMode == obj.depthTestMode);
        }
        break;
    case ID_formatTemplate:
        {  // new scope
        retval = (formatTemplate == obj.formatTemplate);
        }
        break;
    default: retval = false;
    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////
// User-defined methods.
///////////////////////////////////////////////////////////////////////////////

bool
LabelAttributes::ChangesRequireRecalculation(const LabelAttributes &) const
{
    return false;
}

bool
LabelAttributes::VarChangeRequiresReset()
{
    return true;
}

