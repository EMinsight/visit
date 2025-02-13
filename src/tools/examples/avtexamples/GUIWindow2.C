// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <GUIWindow2.h>

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <vtkQtRenderWindow.h>

#include <array>
#include <iostream>

GUIWindow2::GUIWindow2() : QMainWindow()
{
    CreateMainWindow();
}

GUIWindow2::~GUIWindow2()
{
}

void
GUIWindow2::spinBoxChanged(int val)
{
    std::cerr << "New spin box value = " << val << std::endl;
}

void
GUIWindow2::DrawCylinder(vtkRenderWindow *renderWindow)
{
    vtkNew<vtkNamedColors> colors;

    //
    // Set the background color.
    //
    std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
    colors->SetColor("BkgColor", bkg.data());

    //
    // Create a polygonal cylinder with eight circumferential facets.
    //
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);

    //
    // Map the cylinder.
    //
    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    //
    // Create the actor, setting the collor and rotating it.
    //
    vtkNew<vtkActor> cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(
    colors->GetColor4d("Tomato").GetData());
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(-45.0);

    //
    // Create the renderer.
    //
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cylinderActor);
    renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
    // Zoom in a little by accessing the camera and invoking its "Zoom" method.
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Zoom(1.5);

    //
    // Attach the renderer to the render window.
    //
    renderWindow->AddRenderer(renderer);

    //
    // Render the image.
    //
    renderWindow->Render();
}

void
GUIWindow2::CreateMainWindow()
{
    QWidget *central = new QWidget(this);
    this->setCentralWidget(central);

    //
    // Create a horizontal split, with controls on the left
    // and the visualization window on the right.
    //
    QHBoxLayout *hLayout = new QHBoxLayout(central);
    hLayout->setContentsMargins(10,10,10,10);
    hLayout->setSpacing(10);
    QVBoxLayout *leftLayout = new QVBoxLayout(0);
    leftLayout->setSpacing(10);
    hLayout->addLayout(leftLayout);

    //
    // Create tabs on the left side of the window.
    //
    QTabWidget *tabs = new QTabWidget(central);
    leftLayout->addWidget(tabs);

    //
    // Create the general tab.
    //
    QWidget *pageGeneral = new QWidget(central);
    tabs->addTab(pageGeneral, "General");

    QWidget *generalWidget = new QWidget(pageGeneral);

    QVBoxLayout *gLayout = new QVBoxLayout(generalWidget);
    gLayout->setContentsMargins(5,5,5,5);
    gLayout->setSpacing(10);

    //
    // Create a layout for the remaining widgets.
    //
    QGridLayout *mainLayout = new QGridLayout(0);
    gLayout->addLayout(mainLayout);

    //
    // Add the spin box widgets.
    //
    QLabel *spinBoxLabel = new QLabel("SpinBox", generalWidget);
    mainLayout->addWidget(spinBoxLabel, 0,0);

    QSpinBox *spinBox = new QSpinBox(generalWidget);
    spinBox->setRange(0, 10);
    spinBox->setSingleStep(1);
    this->connect(spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(spinBoxChanged(int)));
    mainLayout->addWidget(spinBox, 0,1);

    //
    // Create the visualization window.
    //
    vtkQtRenderWindow *viswin = vtkQtRenderWindow::New(false);
    viswin->setMinimumSize(QSize(500,500));
    hLayout->addWidget(viswin, 100);

    //
    // Draw a cylinder in the render window.
    //
    DrawCylinder(viswin->GetRenderWindow());

    //
    // Create the menus.
    //
    QMenu *fileMenu = this->menuBar()->addMenu("File");
    fileMenu->addAction("Quit", qApp, SLOT(quit()));
}
