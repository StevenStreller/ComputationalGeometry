

#include "CgQtGLRenderWidget.h"
#include "CgQtGui.h"
#include "CgQtMainApplication.h"
#include "../CgBase/CgEnums.h"
#include "../CgEvents/CgMouseEvent.h"
#include "../CgEvents/CgKeyEvent.h"
#include "../CgEvents/CgWindowResizeEvent.h"
#include "../CgEvents/CgLoadMeshEvent.h"
#include "../CgEvents/CgLoadHalfEdgeMeshEvent.h"
#include "../CgEvents/CgLoadPointCloudEvent.h"
#include "../CgEvents/CgTrackballEvent.h"
#include "../CgEvents/CgSplatEvent.h"
#include "../CgEvents/CgPickRayEvent.h"
#include "../CgEvents/CgButtonClickEvent.h"
#include "../CgEvents/CgSpinBoxChangedEvent.h"
#include "../CgEvents/CgCheckboxClickedEvent.h"
#include "../CgEvents/CgRadioButtonClickedEvent.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QMenuBar>
#include <QActionGroup>
#include <QFileDialog>
#include <iostream>



CgQtGui::CgQtGui(CgQtMainApplication *mw)
    : m_mainWindow(mw)
{
    m_glRenderWidget = new CgQtGLRenderWidget;


    connect(m_glRenderWidget, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(mouseEvent(QMouseEvent*)));
    connect(m_glRenderWidget, SIGNAL(viewportChanged(int,int)), this, SLOT(viewportChanged(int,int)));
    connect(m_glRenderWidget, SIGNAL(trackballChanged()), this, SLOT(slotTrackballChanged()));


    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *container = new QHBoxLayout;


    QWidget *opt = new QWidget;
    createOptionPanelExample1(opt);

    QWidget *otheropt = new QWidget;
    createOptionPanelExample2(otheropt);

    QTabWidget* m_tabs = new QTabWidget();
    m_tabs->addTab(opt,"&My Tab1");
    m_tabs->addTab(otheropt,"&My Tab2");
    container->addWidget(m_tabs);

    m_tabs->setMaximumWidth(400);

    container->addWidget(m_glRenderWidget);

    QWidget *w = new QWidget;
    w->setLayout(container);
    mainLayout->addWidget(w);

    setLayout(mainLayout);
    setWindowTitle(tr("Übung Computational Geometry -  Sommer 2023"));


    /* create Menu Bar */
    m_menuBar = new QMenuBar;
    QMenu *file_menu = new QMenu("&File" );
    file_menu->addAction("&Open Mesh Model", this, SLOT(slotLoadMeshFile()));
    file_menu->addAction("&Open HalfEdge Mesh Model", this, SLOT(slotLoadHalfEdgeFile()));
    file_menu->addAction("&Open Point Cloud", this, SLOT(slotLoadPointCloudFile()));


    // todo: Add Quit-Action
    m_menuBar->addMenu( file_menu );
    QMenu *settings_menu = new QMenu("&Setting" );
    QMenu *polygon_mode_menu = new QMenu("&Polygon Mode" );

    QAction* m_custom_rot=settings_menu->addAction("&Backface Culling", m_glRenderWidget, SLOT(slotBackfaceCulling()));
    m_custom_rot->setCheckable(true);
    m_custom_rot->setChecked(false);

    QAction* m_lighting=settings_menu->addAction("&Lighting on", m_glRenderWidget, SLOT(slotLighting()));
    m_lighting->setCheckable(true);
    m_lighting->setChecked(false);

   // QAction* use_splats=settings_menu->addAction("&show splats", this, SLOT(slotSplatting()));
   // use_splats->setCheckable(true);
   // use_splats->setChecked(false);

    QAction* show_pickray=settings_menu->addAction("&show pick ray", this, SLOT(slotPickRay()));
    show_pickray->setCheckable(true);
    show_pickray->setChecked(false);


    QActionGroup* polygonmode_group = new QActionGroup(this);
    polygonmode_group->setExclusive(true);

    QAction* points=polygon_mode_menu->addAction("&Points", m_glRenderWidget, SLOT(slotRenderPoints()));
    points->setCheckable(true);
    points->setChecked(false);

    QAction* lines=polygon_mode_menu->addAction("&Lines", m_glRenderWidget, SLOT(slotRenderLines()));
    lines->setCheckable(true);
    lines->setChecked(true);

    QAction* filled=polygon_mode_menu->addAction("&Filled", m_glRenderWidget, SLOT(slotRenderFilled()));
    filled->setCheckable(true);
    filled->setChecked(false);


    polygonmode_group->addAction(points);
    polygonmode_group->addAction(lines);
    polygonmode_group->addAction(filled);



    // todo: Add Quit-Action
    m_menuBar->addMenu( file_menu );
    m_menuBar->addMenu( settings_menu );
    m_menuBar->addMenu( polygon_mode_menu );


    m_mainWindow->setMenuBar(m_menuBar);



}

QSlider *CgQtGui::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}







void CgQtGui::createOptionPanelExample1(QWidget* parent)
{
    QVBoxLayout *tab1_control = new QVBoxLayout();


    /*Example for using a label */

    QLabel *options_label = new QLabel("Options");
    tab1_control->addWidget(options_label);
    options_label->setAlignment(Qt::AlignCenter);


    /*Example for using a spinbox */

    QLabel *kNearestIndexLabel = new QLabel("K-nearest point index");
    tab1_control->addWidget(kNearestIndexLabel);

    kNearestSelectedIndexSpinBox = new QSpinBox();
    tab1_control->addWidget(kNearestSelectedIndexSpinBox);
    kNearestSelectedIndexSpinBox->setMinimum(0);
    kNearestSelectedIndexSpinBox->setMaximum(100000);
    kNearestSelectedIndexSpinBox->setValue(0);
    connect(kNearestSelectedIndexSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotKNearestSpinboxIndexChanged()));

    QLabel *kNearestKLabel = new QLabel("K-nearest K value");
    tab1_control->addWidget(kNearestKLabel);

    kNearestSelectedKSpinBox = new QSpinBox();
    tab1_control->addWidget(kNearestSelectedKSpinBox);
    kNearestSelectedKSpinBox->setMinimum(1);
    kNearestSelectedKSpinBox->setMaximum(10000);
    kNearestSelectedKSpinBox->setValue(20);
    connect(kNearestSelectedKSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotKNearestSpinboxKValueChanged()));




   // mySpinBox1->setSuffix("   suffix");
   // mySpinBox1->setPrefix("Prefix:  ");
    //connect(kNearestSelectedIndexSpinBox, SIGNAL(valueChanged(int) ), this, SLOT(slotMySpinBox1Changed()) );
    //tab1_control->addWidget(kNearestSelectedIndexSpinBox);


    /*Example for using a checkbox */

    //checkboxShowKdTree = new QCheckBox("&Show KdTree");
    //checkboxShowKdTree->setCheckable(true);
    //checkboxShowKdTree->setChecked(false);
    //connect(checkboxShowKdTree, SIGNAL( clicked() ), this, SLOT(slotCheckboxShowKdTreeClicked()) );
    //tab1_control->addWidget(checkboxShowKdTree);
    QLabel *kdLevelsLabel = new QLabel("Show kd-tree depth");
    tab1_control->addWidget(kdLevelsLabel);

    kdDepthSpinBox = new QSpinBox();
    kdDepthSpinBox->setMinimum(0);
    kdDepthSpinBox->setMaximum(50);
    kdDepthSpinBox->setValue(0);
    //kdDepthSpinBox->setEnabled(false);
    tab1_control->addWidget(kdDepthSpinBox);
    connect(kdDepthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotKdDepthSpinBoxValueChanged()));

    //myCheckBox1 = new QCheckBox("enable Option 1");
    //myCheckBox1->setCheckable(true);
    //myCheckBox1->setChecked(false);
    //connect(myCheckBox1, SIGNAL( clicked() ), this, SLOT(slotMyCheckBox1Changed()) );
    //tab1_control->addWidget(myCheckBox1);


    /*Example for using a button */

    subdivideButton = new QPushButton("&Subdivide");
    subdivideButton->setEnabled(true);
    tab1_control->addWidget(subdivideButton);

    connect(subdivideButton, SIGNAL( clicked() ), this, SLOT(slotSubdivideButtonPressed()) );


    QHBoxLayout *subBox = new QHBoxLayout();

    /*Example for using a button group */

    QGroupBox* myGroupBox = new QGroupBox("Smoothing type ");

    smoothingRadioButtonGroup = new QButtonGroup(subBox);
    smoothingRadioButtonGroup->setExclusive(true);

    QRadioButton* radiobutton1 = new QRadioButton( "&Biquadratic Smoothing");
    QRadioButton* radiobutton2 = new QRadioButton( "&Bicubic Smoothing");


    radiobutton1->setChecked(true);

    smoothingRadioButtonGroup->addButton(radiobutton1,0);
    smoothingRadioButtonGroup->addButton(radiobutton2,1);


    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radiobutton1);
    vbox->addWidget(radiobutton2);

    vbox->addStretch(1);
    myGroupBox->setLayout(vbox);
    subBox->addWidget(myGroupBox);
    tab1_control->addLayout(subBox);

    connect(smoothingRadioButtonGroup, SIGNAL( buttonClicked(int) ), this, SLOT( slotButtonGroupSmoothingSelectionChanged()));

    showFitFunctionCheckbox = new QCheckBox("&Show fitted function");
    showFitFunctionCheckbox->setChecked(false);
    connect(showFitFunctionCheckbox, SIGNAL( clicked() ), this, SLOT( slotCheckboxShowSmoothingClicked()));
    tab1_control->addWidget(showFitFunctionCheckbox);


    smoothButton = new QPushButton("Least Square Smoothing");
    connect(smoothButton, SIGNAL( clicked() ), this, SLOT(slotSmoothButtonPressed()));
    tab1_control->addWidget(smoothButton);




    parent->setLayout(tab1_control);

}

void CgQtGui::createOptionPanelExample2(QWidget* parent)
{
    //QVBoxLayout *tab2_control = new QVBoxLayout();





    //parent->setLayout(tab2_control);

}



void CgQtGui::slotButtonGroupSmoothingSelectionChanged()
{
    CgBaseEvent* e;
    //Id 0 - quad, id 1 - cubic
    if(smoothingRadioButtonGroup->checkedId() == 0){
        e = new CgRadioButtonClickedEvent(Cg::CgRadioButtonActivateEvent, CgRadioButtonClickedEvent::least_smoothing_biquadratic);
    } else {
        e = new CgRadioButtonClickedEvent(Cg::CgRadioButtonActivateEvent, CgRadioButtonClickedEvent::least_smoothing_bicubic);
    }
    notifyObserver(e);
}


void CgQtGui::slotCheckboxShowSmoothingClicked()
{
    CgBaseEvent* e = new CgCheckboxClickedEvent(Cg::CgCheckBoxClickEvent, CgCheckboxClickedEvent::showSmoothingMesh, showFitFunctionCheckbox->isChecked());
    notifyObserver(e);
}


void CgQtGui::slotLoadPointCloudFile()
{



   QString file=  QFileDialog::getOpenFileName(this, tr("Open Obj-File"),"",tr("Model Files (*.obj)"));


    CgBaseEvent* e = new CgLoadPointCloudEvent(Cg::CgLoadPointCloudEvent, file.toStdString());

    subdivideButton->setEnabled(false);
    notifyObserver(e);
}


void CgQtGui::slotSplatting()
{
  m_use_spats=!m_use_spats;

  CgBaseEvent* e = new CgSplatEvent(Cg::CgSplatEvent, m_use_spats);

  notifyObserver(e);

}

void CgQtGui::slotPickRay()
{
  m_show_pickray=!m_show_pickray;

  CgBaseEvent* e = new CgPickRayEvent(Cg::CgPickRayEvent, m_show_pickray);

  notifyObserver(e);

}


void CgQtGui::slotLoadMeshFile()
{



   QString file=  QFileDialog::getOpenFileName(this, tr("Open Obj-File"),"",tr("Model Files (*.obj)"));


    CgBaseEvent* e = new CgLoadMeshEvent(Cg::CgLoadMeshEvent, file.toStdString());

    subdivideButton->setEnabled(false);
    notifyObserver(e);
}


void CgQtGui::slotLoadHalfEdgeFile()
{



   QString file=  QFileDialog::getOpenFileName(this, tr("Open Obj-File"),"",tr("Model Files (*.obj)"));


    CgBaseEvent* e = new CgLoadHalfEdgeMeshEvent(Cg::CgLoadHalfEdgeMeshEvent, file.toStdString());
    subdivideButton->setEnabled(true);
    notifyObserver(e);

}



void CgQtGui::slotTrackballChanged()
{
    CgBaseEvent* e = new CgTrackballEvent(Cg::CgTrackballEvent, m_glRenderWidget->getTrackballRotation());
    notifyObserver(e);
}

//void CgQtGui::slotCheckboxShowKdTreeClicked() {
//    CgBaseEvent* e = new CgCheckboxClickedEvent(Cg::CgCheckBoxClickEvent, CgCheckboxClickedEvent::showKdTree, checkboxShowKdTree->isChecked());
//    kdDepthSpinBox->setEnabled(checkboxShowKdTree->isChecked());
//    notifyObserver(e);
//}

void CgQtGui::slotKdDepthSpinBoxValueChanged(){
    int selectedValue = kdDepthSpinBox->value();
    CgBaseEvent* e = new CgSpinBoxChangedEvent(Cg::CgSpinBoxChangedEvent, CgSpinBoxChangedEvent::kdDepth, selectedValue);
    notifyObserver(e);
}


void CgQtGui::slotSubdivideButtonPressed()
{
    CgBaseEvent* e = new CgButtonClickEvent(Cg::CgButtonClickEvent, CgButtonClickEvent::SubdivideButton);
    notifyObserver(e);
}

void CgQtGui::slotSmoothButtonPressed()
{
    CgBaseEvent* e = new CgButtonClickEvent(Cg::CgButtonClickEvent, CgButtonClickEvent::SmoothButton);
    notifyObserver(e);
}


void CgQtGui::slotKNearestSpinboxIndexChanged()
{
    int selectedIndex = kNearestSelectedIndexSpinBox->value();
    CgBaseEvent* e = new CgSpinBoxChangedEvent(Cg::CgSpinBoxChangedEvent, CgSpinBoxChangedEvent::kIndex, selectedIndex);
    notifyObserver(e);
    //Two events to give both items to function
}

void CgQtGui::slotKNearestSpinboxKValueChanged()
{
    int selectedK = kNearestSelectedKSpinBox->value();
    CgBaseEvent* e = new CgSpinBoxChangedEvent(Cg::CgSpinBoxChangedEvent, CgSpinBoxChangedEvent::kValue, selectedK);
    notifyObserver(e);
    //Two events to give both items to function
}


void CgQtGui::mouseEvent(QMouseEvent* event)
{

   // std::cout << QApplication::keyboardModifiers() << std::endl;

  //  if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)==true)
    //    std::cout << Cg::ControlModifier << endl;


   if(event->type()==QEvent::MouseButtonPress)
   {


        CgBaseEvent* e = new CgMouseEvent(Cg::CgMouseButtonPress,
                                          glm::vec2(event->localPos().x() ,event->localPos().y()),
                                          (Cg::MouseButtons)event->button());

        notifyObserver(e);
   }

   if(event->type()==QEvent::MouseMove)
   {
       CgBaseEvent* e= new CgMouseEvent(Cg::CgMouseMove,
                                        glm::vec2(event->localPos().x() ,event->localPos().y()),
                                        (Cg::MouseButtons)event->button());
       notifyObserver(e);
   }



}

void CgQtGui::keyPressEvent(QKeyEvent *event)
{
   CgBaseEvent* e= new CgKeyEvent(Cg::CgKeyPressEvent,(Cg::Key)event->key(),(Cg::KeyboardModifiers)event->nativeModifiers(),event->text().toStdString());
   notifyObserver(e);
}


void CgQtGui::viewportChanged(int w, int h)
{
     CgBaseEvent* e = new CgWindowResizeEvent(Cg::WindowResizeEvent,w,h);
     notifyObserver(e);
}




CgBaseRenderer* CgQtGui::getRenderer()
{
    return m_glRenderWidget;
}





