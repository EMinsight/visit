// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                        VisWindowProtectionProxy.C                         //
// ************************************************************************* //

#include <VisWindow.h>
#include <VisWindowProtectionProxy.h>


using  std::vector;


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetMode
//
//  Purpose:
//      Gets the VisWindow's window mode through its friend access.
//
//  Returns:    The VisWindow's window mode.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

WINDOW_MODE
VisWindowProtectionProxy::ProxiedGetMode()
{
    return viswin->mode;
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedChangeMode
//
//  Purpose:
//      Changes the VisWindow's window mode through its friend access.
//
//  Arguments:
//      mode    The new window mode.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedChangeMode(WINDOW_MODE mode)
{
    viswin->ChangeMode(mode);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedSetInteractor
//
//  Purpose:
//      Sets the VisWindow's interactor through its friend access.
//
//  Arguments:
//      interactor   The new interactor.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedSetInteractor(VisitInteractor *interactor)
{
    viswin->SetInteractor(interactor);
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetBoundingBoxMode
//
// Purpose:
//   Returns the bounding box mode.
//
// Returns:    The bounding box mode.
//
// Programmer: Brad Whitlock
// Creation:   Thu Oct 4 08:43:20 PDT 2001
//
// Modifications:
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetBoundingBoxMode() const
{
    return viswin->GetBoundingBoxMode();
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetForegroundColor
//
//  Purpose:
//      Gets the foreground color of the VisWindow through its friend access.
//
//  Arguments:
//      fg      An array to put the rgb values into.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedGetForegroundColor(double fg[3])
{
    viswin->GetForegroundColor(fg);
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::GetHotPoint
//
// Purpose:
//   Returns a pointer to the hotpoint that is clicked at point (x,y).
//
// Arguments:
//   x : The x location of the click in device coordinates.
//   y : The y location of the click in device coordinates.
//   h : A reference to the return hotpoint.
//
// Returns:   A boolean value indicating whether or not a hotpoint was clicked.
//
// Programmer: Brad Whitlock
// Creation:   Mon Oct 1 14:14:29 PST 2001
//
// Modifications:
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetHotPoint(int x, int y, HotPoint &h) const
{
    return viswin->GetHotPoint(x, y, h);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetViewport
//
//  Purpose:
//      Gets the viewport of the VisWindow through its friend access.
//
//  Arguments:
//      vport   An array to put the viewport into.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedGetViewport(double vport[4])
{
    viswin->GetViewport(vport);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetSize
//
//  Purpose: Gets pixel dimensions of the renderable portion of the VisWindow
//           through its friend access.
//
//  Arguments:
//      width  : A reference to an int that is used to return the subwindow width.
//      height : A reference to an int that is used to return the subwindow height.
//
//  Programmer: Mark Blair
//  Creation:   Wed Aug 30 14:09:00 PDT 2006
//
// ****************************************************************************

void VisWindowProtectionProxy::ProxiedGetSize(int &width, int &height) const
{
    viswin->GetSize(width, height);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetWindowSize
//
//  Purpose: Gets pixel dimensions of the VisWindow through its friend access.
//
//  Arguments:
//      width  : A reference to an int that is used to return the window width.
//      height : A reference to an int that is used to return the window height.
//
//  Programmer: Mark Blair
//  Creation:   Wed Aug 30 14:09:00 PDT 2006
//
// ****************************************************************************

void VisWindowProtectionProxy::ProxiedGetWindowSize(int &width, int &height) const
{
    viswin->GetWindowSize(width, height);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetBackground
//
//  Purpose:
//      Gets the background renderer of the VisWindow through its friend
//      access.
//
//  Returns:    The background renderer.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

vtkRenderer *
VisWindowProtectionProxy::ProxiedGetBackground()
{
    return viswin->GetBackground();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetForeground
//
//  Purpose:
//      Gets the foreground renderer of the VisWindow through its friend
//      access.
//
//  Returns:    The foreground renderer.
//
//  Programmer: Hank Childs
//  Creation:   August 1, 2000
//
// ****************************************************************************

vtkRenderer *
VisWindowProtectionProxy::ProxiedGetForeground()
{
    return viswin->GetForeground();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetCanvas
//
//  Purpose:
//      Gets the canvas renderer of the VisWindow through its friend access.
//
//  Returns:    The canvas renderer.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

vtkRenderer *
VisWindowProtectionProxy::ProxiedGetCanvas()
{
    return viswin->GetCanvas();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedStartRender
//
//  Purpose:
//      Invokes the VisWindow's mechanism for starting rendering.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedStartRender()
{
    viswin->StartRender();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedRender
//
//  Purpose:
//      Makes the VisWindow re-render.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedRender()
{
    viswin->Render();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedRecalculateRenderOrder
//
//  Purpose:
//      Makes the VisWindow recalculate the order each of its actors should be
//      drawn in.
//
//  Programmer: Hank Childs
//  Creation:   September 4, 2002
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedRecalculateRenderOrder()
{
    viswin->RecalculateRenderOrder();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedStartBoundingBox
//
//  Purpose:
//      Makes the VisWindow start bounding box mode.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedStartBoundingBox()
{
    viswin->StartBoundingBox();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedEndBoundingBox
//
//  Purpose:
//      Makes the VisWindow end bounding box mode.
//
//  Programmer: Hank Childs
//  Creation:   June 9, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedEndBoundingBox()
{
    viswin->EndBoundingBox();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedHasPlots
//
//  Purpose:
//      Uses friend access to see if the vis window has plots.
//
//  Returns:    The value of VisWindow::hasPlots.
//
//  Programmer: Hank Childs
//  Creation:   July 6, 2000
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedHasPlots()
{
    return viswin->hasPlots;
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedUpdatesEnabled
//
//  Purpose:
//      Uses friend access to see if the vis window has updates enabled.
//
//  Returns:    The value of VisWindow::updatesEnabled.
//
//  Programmer: Hank Childs
//  Creation:   July 6, 2000
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedUpdatesEnabled()
{
    return viswin->updatesEnabled;
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedDisableUpdates
//
// Purpose:
//   Disables updates in the VisWindow.
//
// Programmer: Brad Whitlock
// Creation:   Fri Oct 12 09:46:15 PDT 2001
//
// Modifications:
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedDisableUpdates()
{
    viswin->DisableUpdates();
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedEnableUpdates
//
// Purpose:
//   Enables updates in the VisWindow.
//
// Programmer: Brad Whitlock
// Creation:   Fri Oct 12 09:46:15 PDT 2001
//
// Modifications:
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedEnableUpdates()
{
    viswin->EnableUpdates();
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedHasPlots
//
//  Purpose:
//      Uses friend access to tell the VisWindow whether or not it has plots.
//
//  Arguments:
//      p     A boolean indicating if the vis window has plots.
//
//  Programmer: Hank Childs
//  Creation:   July 6, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedHasPlots(bool p)
{
    viswin->HasPlots(p);
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetBounds
//
//  Purpose:
//      Uses friend access to get the bounds of the plots.
//
//  Arguments:
//      bounds     A place to store the bounds of the plots.
//
//  Programmer: Hank Childs
//  Creation:   November 8, 2000
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedGetBounds(double bounds[6])
{
    viswin->GetBounds(bounds);
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedSetHighlightEnabled
//
// Purpose:
//   Turns the hotpoint highlights on/off.
//
// Arguments:
//   val : Whether or not to enable hotpoint highlights.
//
// Programmer: Brad Whitlock
// Creation:   Wed Oct 3 00:03:32 PDT 2001
//
// Modifications:
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedSetHighlightEnabled(bool val)
{
    viswin->SetHighlightEnabled(val);
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedPick
//
// Purpose:
//   Tells the viswin to perform a Pick.
//
// Arguments:
//   x, y  The screen coordinates of the picked point.
//
//
// Programmer:  Kathleen Bonnell
// Creation:    November 12, 2001
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedPick(int x, int y)
{
    viswin->Pick(x, y);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedUpdatePlotList
//
//  Purpose:
//      Tells the vis window to update its plot list.
//
//  Arguments:
//      lst     A list of the current actors.
//
//  Programmer: Hank Childs
//  Creation:   March 12, 2002
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedUpdatePlotList(vector<avtActor_p> &lst)
{
    viswin->UpdatePlotList(lst);
}



// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedLineout
//
// Purpose:
//   Tells the viswin to perform a Lineout.
//
// Arguments:
//   x1, y1, x2, y2    The screen coordinate endpoints of the picked line.
//
// Programmer:  Kathleen Bonnell
// Creation:    December 17, 2001
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedLineout(int x1, int y1, int x2, int y2)
{
    viswin->Lineout(x1, y1, x2, y2);
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedComputeVectorTextScaleFactor
//
// Purpose:     Have the VisWindow's Compute a scale factor for vector text.
//
// Returns:     The scale factor.
//
// Arguments:
//   p          A 3d-world coordinate position to use in the calculation.
//   v          An alternate viewport (optional).
//
// Programmer:  Kathleen Bonnell
// Creation:    May 8, 2002
//
// ****************************************************************************

double
VisWindowProtectionProxy::ProxiedComputeVectorTextScaleFactor(const double *p,
                                                              const double *v)
{
    return viswin->ComputeVectorTextScaleFactor(p, v);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedMotionBegin
//
//  Purpose:
//      Tells the vis window that motion began.
//
//  Programmer:  Hank Childs
//  Creation:    July 11, 2002
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedMotionBegin(void)
{
    viswin->MotionBegin();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedMotionEnd
//
//  Purpose:
//      Tells the vis window that motion end.
//
//  Programmer:  Hank Childs
//  Creation:    July 11, 2002
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedMotionEnd(void)
{
    viswin->MotionEnd();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetAmbientOn
//
//  Purpose:
//      Retrieve the flag specifying where ambient lighting is on.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    August 12, 2002
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetAmbientOn(void)
{
    return viswin->GetAmbientOn();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetAmbientCoefficient
//
//  Purpose:
//      Retrieve the ambient lighting coefficient.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    August 12, 2002
//
// ****************************************************************************

double
VisWindowProtectionProxy::ProxiedGetAmbientCoefficient(void)
{
    return viswin->GetAmbientCoefficient();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetLighting
//
//  Purpose:
//      Retrieve the flag specifying whether lighting is on or off.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    August 12, 2002
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetLighting(void)
{
    return viswin->GetLighting();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedUpateLightPositions
//
//  Purpose:
//     Notifies VisWindow that lights should update position.
//
//  Programmer:  Kathleen Bonnell
//  Creation:    August 12, 2002
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedUpdateLightPositions()
{
    viswin->UpdateLightPositions();
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetSurfaceRepresentation
//
// Purpose:
//   Returns the surface representation.
//
// Programmer: Brad Whitlock
// Creation:   Mon Sep 23 16:32:24 PST 2002
//
// Modifications:
//
// ****************************************************************************

int
VisWindowProtectionProxy::ProxiedGetSurfaceRepresentation()
{
    return viswin->GetSurfaceRepresentation();
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedDisableExternalRenderRequests
//
// Purpose:
//    Disable external render requests
//
// Programmer: Mark C. Miller
// Creation:   February 5, 2003
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedDisableExternalRenderRequests(bool bClearImage)
{
   return viswin->DisableExternalRenderRequests(bClearImage);
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedEnableExternalRenderRequests
//
// Purpose:
//    Enable external render requests
//
// Programmer: Mark C. Miller
// Creation:   February 5, 2003
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedEnableExternalRenderRequests(void)
{
   return viswin->EnableExternalRenderRequests();
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetScaleFactorAndType
//
// Purpose:     Gets the VisWindow's axis scale factor and scale type.
//
// Returns:     (via args) The current scale factor and scale type used in the
//              VisWindow.
//
// Programmer:  Kathleen Bonnell
// Creation:    May 13, 2003
//
// Modifications:
//   Kathleen Bonnell, Mon May 19 09:00:54 PDT 2003
//   Removed 'return'.
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedGetScaleFactorAndType(double &s, int &t)
{
    viswin->GetScaleFactorAndType(s, t);
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedReAddColleaguesToRenderWindow
//
// Purpose:     Tells the VisWindow to allow colleagues to re-add themsevles
//              to the render window.  For anti-aliasing mode.
//
// Programmer:  Kathleen Bonnell
// Creation:    May 28, 2003
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedReAddColleaguesToRenderWindow()
{
    viswin->ReAddColleaguesToRenderWindow();
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetAntialiasing
//
// Purpose:     Returns the antialising mode.
//
// Returns:     The antialising mode.
//
// Programmer:  Kathleen Bonnell
// Creation:    May 28, 2003
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetAntialiasing()
{
    return viswin->GetAntialiasing();
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetFullFrameMode
//
// Purpose:     Returns the fullframe mode.
//
// Returns:     The fullframe mode.
//
// Programmer:  Kathleen Bonnell
// Creation:    June 6, 2003.
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetFullFrameMode()
{
    return viswin->GetFullFrameMode();
}

// ****************************************************************************
//  Method:  VisWindowProtectionProxy::ProxiedGetSpecularFlag
//
//  Purpose:
//    Returns the specular flag
//
//  Programmer:  Jeremy Meredith
//  Creation:    November 14, 2003
//
// ****************************************************************************
bool
VisWindowProtectionProxy::ProxiedGetSpecularFlag()
{
    return viswin->GetSpecularFlag();
}

// ****************************************************************************
//  Method:  VisWindowProtectionProxy::ProxiedGetSpecularCoeff
//
//  Purpose:
//    Returns the specular coefficient
//
//  Programmer:  Jeremy Meredith
//  Creation:    November 14, 2003
//
// ****************************************************************************
double
VisWindowProtectionProxy::ProxiedGetSpecularCoeff()
{
    return viswin->GetSpecularCoeff();
}

// ****************************************************************************
//  Method:  VisWindowProtectionProxy::ProxiedGetSpecularPower
//
//  Purpose:
//    Returns the specular exponenet (power)
//
//  Programmer:  Jeremy Meredith
//  Creation:    November 14, 2003
//
// ****************************************************************************
double
VisWindowProtectionProxy::ProxiedGetSpecularPower()
{
    return viswin->GetSpecularPower();
}

// ****************************************************************************
//  Method:  VisWindowProtectionProxy::ProxiedGetSpecularColor
//
//  Purpose:
//    Returns the specular color
//
//  Programmer:  Jeremy Meredith
//  Creation:    November 14, 2003
//
// ****************************************************************************
const ColorAttribute &
VisWindowProtectionProxy::ProxiedGetSpecularColor()
{
    return viswin->GetSpecularColor();
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::TransparenciesExist
//
// Purpose:     Returns whether or not there are transparent actors.
//
// Programmer:  Kathleen Bonnell
// Creation:    December 3, 2003
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedTransparenciesExist()
{
    return viswin->TransparenciesExist();
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedReAddToolsToRenderWindow
//
// Purpose:     Tells the VisWindow to allow tools to re-add themsevles
//              to the render window.
//
// Programmer:  Kathleen Bonnell
// Creation:    May 28, 2003
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedReAddToolsToRenderWindow()
{
    viswin->ReAddToolsToRenderWindow();
}


// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetFrameAndState
//
// Purpose:
//   Returns the vis window's frame and state information.
//
// Programmer: Brad Whitlock
// Creation:   Thu Nov 6 14:16:54 PST 2003
//
// Modifications:
//
// ****************************************************************************

void
VisWindowProtectionProxy::ProxiedGetFrameAndState(int &nFrames,
    int &startFrame, int &curFrame, int &endFrame,
    int &startState, int &curState, int &endState) const
{
    viswin->GetFrameAndState(nFrames, startFrame, curFrame, endFrame,
                             startState, curState, endState);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedSuspendOpaqueGeometry
//
//  Purpose:
//    Make opaque geometry invisible
//
//  Programmer: Chris Wojtan
//  Creation:   Mon Jul 26 16:07:57 PDT 2004
//
// ***************************************************************************

void
VisWindowProtectionProxy::ProxiedSuspendOpaqueGeometry()
{
    viswin->SuspendOpaqueGeometry();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedSuspendTranslucentGeometry
//
//  Purpose:
//    Make translucent geometry invisible
//
//  Programmer: Chris Wojtan
//  Creation:   Mon Jul 26 16:08:05 PDT 2004
//
// ***************************************************************************

void
VisWindowProtectionProxy::ProxiedSuspendTranslucentGeometry()
{
    viswin->SuspendTranslucentGeometry();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedResumeOpaqueGeometry
//
//  Purpose:
//    Make opaque geometry visible again
//
//  Programmer: Chris Wojtan
//  Creation:   Mon Jul 26 16:07:47 PDT 2004
//
// ***************************************************************************

void
VisWindowProtectionProxy::ProxiedResumeOpaqueGeometry()
{
    viswin->ResumeOpaqueGeometry();
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedResumeTranslucentGeometry
//
//  Purpose:
//    Make translucent geometry visible again
//
//  Programmer: Chris Wojtan
//  Creation:   Mon Jul 26 16:07:52 PDT 2004
//
// ***************************************************************************

void
VisWindowProtectionProxy::ProxiedResumeTranslucentGeometry()
{
    viswin->ResumeTranslucentGeometry();
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetMaxPlotZShift
//
//  Purpose:
//    Returns the maximum amount the plots have been shifted in Z.
//
//  Programmer: Kathleen Bonnell
//  Creation:   June 27, 2005
//
// ***************************************************************************

double
VisWindowProtectionProxy::ProxiedGetMaxPlotZShift()
{
    return viswin->GetMaxPlotZShift();
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedIsMakingExternalRenderRequests
//
//  Purpose: Query of the window is making external render requests
//
//  Programmer: Mark C. Miller
//  Creation:   March 28, 2007
//
// ***************************************************************************
bool
VisWindowProtectionProxy::ProxiedIsMakingExternalRenderRequests(void) const
{
    return viswin->IsMakingExternalRenderRequests();
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetAverageExternalRenderingTime
//
//  Purpose: Return average rendering time for external render requests
//
//  Programmer: Mark C. Miller
//  Creation:   March 28, 2007
//
// ***************************************************************************
double
VisWindowProtectionProxy::ProxiedGetAverageExternalRenderingTime(void) const
{
    return viswin->GetAverageExternalRenderingTime();
}

// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedDoNextExternalRenderAsVisualQueue
//
//  Purpose: Force next render to render only the 'in-progress' visual queue
//
//  Programmer: Mark C. Miller
//  Creation:   March 28, 2007
//
// ***************************************************************************
void
VisWindowProtectionProxy::ProxiedDoNextExternalRenderAsVisualQueue(
    int w, int h, const double *c)
{
    viswin->DoNextExternalRenderAsVisualQueue(w,h,c);
};

// ****************************************************************************
// Method: VisWindowProtectionProxy::GetBackgroundMode
//
// Purpose:
//   Return the background mode.
//
// Returns:    The background mode.
//
// Programmer: Brad Whitlock
// Creation:   Mon Nov 19 17:45:21 PST 2007
//
// Modifications:
//
// ****************************************************************************

int
VisWindowProtectionProxy::ProxiedGetBackgroundMode() const
{
    return viswin->GetBackgroundMode();
}

// ****************************************************************************
// Method:  VisWindowProtectionProxy::ProxiedGet3DAxisScalingFactors
//
// Purpose:
//   Return the 3D axis scaling factors and enabled flag.
//
// Arguments:
//   s       (o) the scaling factors
//
// Returns:  true if scaling is enabled
//
// Programmer:  Jeremy Meredith
// Creation:    May 19, 2010
//
// ****************************************************************************
bool
VisWindowProtectionProxy::ProxiedGet3DAxisScalingFactors(double s[3])
{
    return viswin->Get3DAxisScalingFactors(s);
}


// ****************************************************************************
//  Method: VisWindowProtectionProxy::ProxiedGetToolUpdateMode
//
//  Purpose:
//      Gets the VisWindow's tool update mode through its friend access.
//
//  Returns:    The VisWindow's tool update mode.
//
//  Programmer: Jeremy Meredith
//  Creation:   February  2, 2010
//
// ****************************************************************************

TOOLUPDATE_MODE
VisWindowProtectionProxy::ProxiedGetToolUpdateMode() const
{
    return viswin->GetToolUpdateMode();
}

vtkPolyDataMapper2D *
VisWindowProtectionProxy::ProxiedCreateRubberbandMapper()
{
     return viswin->CreateRubberbandMapper();
}

vtkPolyDataMapper2D *
VisWindowProtectionProxy::ProxiedCreateXorGridMapper()
{
     return viswin->CreateXorGridMapper();
}

void // TODO: Remove with VTK8
VisWindowProtectionProxy::ProxiedGetExtents(double ext[2])
{
     return viswin->GetExtents(ext);
}

#if defined(HAVE_OSPRAY)
// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetOsprayRendering
//
// Purpose:     Returns the OSPRay rendering flag
//
// Returns:     The OSPRay rendering flag
//
// Programmer:  Alok Hota
// Creation:    Tue 24 Apr 2018 11:59:54 AM EDT
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetOsprayRendering()
{
    return viswin->GetOsprayRendering();
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetOspraySPP
//
// Purpose:     Returns the OSPRay samples per pixel
//
// Returns:     The OSPRay samples per pixel
//
// Programmer:  Alok Hota
// Creation:    Tue 24 Apr 2018 11:59:54 AM EDT
//
// ****************************************************************************

int
VisWindowProtectionProxy::ProxiedGetOspraySPP()
{
    return viswin->GetOspraySPP();
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetOsprayAO
//
// Purpose:     Returns the OSPRay ambient occlusion samples
//
// Returns:     The OSPRay ambient occlusion samples
//
// Programmer:  Alok Hota
// Creation:    Tue 24 Apr 2018 11:59:54 AM EDT
//
// ****************************************************************************

int
VisWindowProtectionProxy::ProxiedGetOsprayAO()
{
    return viswin->GetOsprayAO();
}

// ****************************************************************************
// Method: VisWindowProtectionProxy::ProxiedGetOsprayShadows
//
// Purpose:     Returns the OSPRay shadows flag
//
// Returns:     The OSPRay shadows flag
//
// Programmer:  Alok Hota
// Creation:    Wed 02 May 2018 10:10:29 AM EDT
//
// ****************************************************************************

bool
VisWindowProtectionProxy::ProxiedGetOsprayShadows()
{
    return viswin->GetOsprayShadows();
}
#endif
