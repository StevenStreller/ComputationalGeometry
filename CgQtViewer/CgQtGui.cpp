

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
#include "../CgSceneGraph/CgHalfEdgeTriangleMesh.h"

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
        : m_mainWindow(mw) {
    m_glRenderWidget = new CgQtGLRenderWidget;


    connect(m_glRenderWidget, SIGNAL(mouseEvent(QMouseEvent * )), this, SLOT(mouseEvent(QMouseEvent * )));
    connect(m_glRenderWidget, SIGNAL(viewportChanged(int, int)), this, SLOT(viewportChanged(int, int)));
    connect(m_glRenderWidget, SIGNAL(trackballChanged()), this, SLOT(slotTrackballChanged()));


    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *container = new QHBoxLayout;


    QWidget *opt = new QWidget;
    createOptionPanelExample1(opt);

    QWidget *otheropt = new QWidget;
    createOptionPanelExample2(otheropt);

    QTabWidget *m_tabs = new QTabWidget();
    m_tabs->addTab(opt, "&My Tab1");
    m_tabs->addTab(otheropt, "&My Tab2");
    container->addWidget(m_tabs);

    m_tabs->setMaximumWidth(400);

    container->addWidget(m_glRenderWidget);

    QWidget *w = new QWidget;
    w->setLayout(container);
    mainLayout->addWidget(w);

    setLayout(mainLayout);
    setWindowTitle(tr("Übung Computational Geometry -  Sommer 2022"));


    /* create Menu Bar */
    m_menuBar = new QMenuBar;
    QMenu *file_menu = new QMenu("&File");
    file_menu->addAction("&Open Mesh Model", this, SLOT(slotLoadMeshFile()));
    file_menu->addAction("&Open HalfEdge Mesh Model", this, SLOT(slotLoadHalfEdgeFile()));
    file_menu->addAction("&Open Point Cloud", this, SLOT(slotLoadPointCloudFile()));


    // todo: Add Quit-Action
    m_menuBar->addMenu(file_menu);
    QMenu *settings_menu = new QMenu("&Setting");
    QMenu *polygon_mode_menu = new QMenu("&Polygon Mode");

    QAction *m_custom_rot = settings_menu->addAction("&Backface Culling", m_glRenderWidget,
                                                     SLOT(slotBackfaceCulling()));
    m_custom_rot->setCheckable(true);
    m_custom_rot->setChecked(false);

    QAction *m_lighting = settings_menu->addAction("&Lighting on", m_glRenderWidget, SLOT(slotLighting()));
    m_lighting->setCheckable(true);
    m_lighting->setChecked(false);

    // QAction* use_splats=settings_menu->addAction("&show splats", this, SLOT(slotSplatting()));
    // use_splats->setCheckable(true);
    // use_splats->setChecked(false);

    QAction *show_pickray = settings_menu->addAction("&show pick ray", this, SLOT(slotPickRay()));
    show_pickray->setCheckable(true);
    show_pickray->setChecked(false);


    QActionGroup *polygonmode_group = new QActionGroup(this);
    polygonmode_group->setExclusive(true);

    QAction *points = polygon_mode_menu->addAction("&Points", m_glRenderWidget, SLOT(slotRenderPoints()));
    points->setCheckable(true);
    points->setChecked(false);

    QAction *lines = polygon_mode_menu->addAction("&Lines", m_glRenderWidget, SLOT(slotRenderLines()));
    lines->setCheckable(true);
    lines->setChecked(true);

    QAction *filled = polygon_mode_menu->addAction("&Filled", m_glRenderWidget, SLOT(slotRenderFilled()));
    filled->setCheckable(true);
    filled->setChecked(false);


    polygonmode_group->addAction(points);
    polygonmode_group->addAction(lines);
    polygonmode_group->addAction(filled);



    // todo: Add Quit-Action
    m_menuBar->addMenu(file_menu);
    m_menuBar->addMenu(settings_menu);
    m_menuBar->addMenu(polygon_mode_menu);


    m_mainWindow->setMenuBar(m_menuBar);


}

QSlider *CgQtGui::createSlider() {
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}


void CgQtGui::createOptionPanelExample1(QWidget *parent) {
    QVBoxLayout *tab1_control = new QVBoxLayout();


    /*Example for using a label */

    QLabel *options_label = new QLabel("Options");
    tab1_control->addWidget(options_label);
    options_label->setAlignment(Qt::AlignCenter);


    /*Example for using a spinbox */

    mySpinBox1 = new QSpinBox();
    tab1_control->addWidget(mySpinBox1);
    mySpinBox1->setMinimum(1);
    mySpinBox1->setMaximum(50);
    mySpinBox1->setValue(3);
    // mySpinBox1->setSuffix("   suffix");
    // mySpinBox1->setPrefix("Prefix:  ");
    connect(mySpinBox1, SIGNAL(valueChanged(int)), this, SLOT(slotMySpinBox1Changed()));
    tab1_control->addWidget(mySpinBox1);


    /*Example for using a checkbox */

    myCheckBox1 = new QCheckBox("enable Option 1");
    myCheckBox1->setCheckable(true);
    myCheckBox1->setChecked(false);
    connect(myCheckBox1, SIGNAL(clicked()), this, SLOT(slotMyCheckBox1Changed()));
    tab1_control->addWidget(myCheckBox1);


    /*Example for using a button */

    QPushButton *myButton1 = new QPushButton("Subdivide");
    tab1_control->addWidget(myButton1);

    connect(myButton1, SIGNAL(clicked()), this, SLOT(slotMyButton1Pressed()));


    parent->setLayout(tab1_control);

}

void CgQtGui::createOptionPanelExample2(QWidget *parent) {
    QVBoxLayout *tab2_control = new QVBoxLayout();
    QHBoxLayout *subBox = new QHBoxLayout();

    /*Example for using a button group */

    QGroupBox *myGroupBox = new QGroupBox("Radiobutton Group Example ");

    myButtonGroup = new QButtonGroup(subBox);
    myButtonGroup->setExclusive(true);

    QRadioButton *radiobutton1 = new QRadioButton("&Option1");
    QRadioButton *radiobutton2 = new QRadioButton("&Option2");
    QRadioButton *radiobutton3 = new QRadioButton("&Option3");
    QRadioButton *radiobutton4 = new QRadioButton("&Option4");
    QRadioButton *radiobutton5 = new QRadioButton("&Option5");

    radiobutton2->setChecked(true);

    myButtonGroup->addButton(radiobutton1, 0);
    myButtonGroup->addButton(radiobutton2, 1);
    myButtonGroup->addButton(radiobutton3, 2);
    myButtonGroup->addButton(radiobutton4, 3);
    myButtonGroup->addButton(radiobutton5, 4);


    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radiobutton1);
    vbox->addWidget(radiobutton2);
    vbox->addWidget(radiobutton3);
    vbox->addWidget(radiobutton4);
    vbox->addWidget(radiobutton5);
    vbox->addStretch(1);
    myGroupBox->setLayout(vbox);
    subBox->addWidget(myGroupBox);
    tab2_control->addLayout(subBox);

    connect(myButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonGroupSelectionChanged()));
    parent->setLayout(tab2_control);

}


void CgQtGui::slotButtonGroupSelectionChanged() {

}

void CgQtGui::slotMySpinBox1Changed() {

}

void CgQtGui::slotMyCheckBox1Changed() {

}


void CgQtGui::slotLoadPointCloudFile() {


    QString file = QFileDialog::getOpenFileName(this, tr("Open Obj-File"), "", tr("Model Files (*.obj)"));


    CgBaseEvent *e = new CgLoadPointCloudEvent(Cg::CgLoadPointCloudEvent, file.toStdString());

    notifyObserver(e);
}


void CgQtGui::slotSplatting() {
    m_use_spats = !m_use_spats;

    CgBaseEvent *e = new CgSplatEvent(Cg::CgSplatEvent, m_use_spats);

    notifyObserver(e);

}

void CgQtGui::slotPickRay() {
    m_show_pickray = !m_show_pickray;

    CgBaseEvent *e = new CgPickRayEvent(Cg::CgPickRayEvent, m_show_pickray);

    notifyObserver(e);

}


void CgQtGui::slotLoadMeshFile() {


    QString file = QFileDialog::getOpenFileName(this, tr("Open Obj-File"), "", tr("Model Files (*.obj)"));


    CgBaseEvent *e = new CgLoadMeshEvent(Cg::CgLoadMeshEvent, file.toStdString());

    notifyObserver(e);
}


void CgQtGui::slotLoadHalfEdgeFile() {


    QString file = QFileDialog::getOpenFileName(this, tr("Open Obj-File"), "", tr("Model Files (*.obj)"));


    CgBaseEvent *e = new CgLoadHalfEdgeMeshEvent(Cg::CgLoadHalfEdgeMeshEvent, file.toStdString());

    notifyObserver(e);
}


void CgQtGui::slotTrackballChanged() {
    CgBaseEvent *e = new CgTrackballEvent(Cg::CgTrackballEvent, m_glRenderWidget->getTrackballRotation());
    notifyObserver(e);
}

void CgQtGui::slotMyButton1Pressed() {
    std::cout << "button 1 pressed " << std::endl;
    CgBaseEvent *e = new CgButtonClickEvent(Cg::CgButtonClickEvent, CgButtonClickEvent::SubdivButton);
    notifyObserver(e);
}


void CgQtGui::mouseEvent(QMouseEvent *event) {

    // std::cout << QApplication::keyboardModifiers() << std::endl;

    //  if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)==true)
    //    std::cout << Cg::ControlModifier << endl;


    if (event->type() == QEvent::MouseButtonPress) {


        CgBaseEvent *e = new CgMouseEvent(Cg::CgMouseButtonPress,
                                          glm::vec2(event->localPos().x(), event->localPos().y()),
                                          (Cg::MouseButtons) event->button());

        notifyObserver(e);
    }

    if (event->type() == QEvent::MouseMove) {
        CgBaseEvent *e = new CgMouseEvent(Cg::CgMouseMove,
                                          glm::vec2(event->localPos().x(), event->localPos().y()),
                                          (Cg::MouseButtons) event->button());
        notifyObserver(e);
    }


}

void CgQtGui::keyPressEvent(QKeyEvent *event) {
    CgBaseEvent *e = new CgKeyEvent(Cg::CgKeyPressEvent, (Cg::Key) event->key(),
                                    (Cg::KeyboardModifiers) event->nativeModifiers(), event->text().toStdString());
    notifyObserver(e);
}


void CgQtGui::viewportChanged(int w, int h) {
    CgBaseEvent *e = new CgWindowResizeEvent(Cg::WindowResizeEvent, w, h);
    notifyObserver(e);
}


CgBaseRenderer *CgQtGui::getRenderer() {
    return m_glRenderWidget;
}





