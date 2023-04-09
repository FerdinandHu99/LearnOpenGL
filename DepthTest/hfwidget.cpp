#include "hfwidget.h"
#include "ui_hfwidget.h"

HFWidget::HFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HFWidget), OpenGLWidget(new HFOpenGLWidget(this))
{
    ui->setupUi(this);
    OpenGLWidget->setGeometry(QRect(0, 0, 1280, 720));
    OpenGLWidget->show();
}

HFWidget::~HFWidget()
{
    delete ui;
    delete OpenGLWidget;
}
