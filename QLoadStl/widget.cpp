#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QFileDialog>
#include "qobjload.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->lineEdit_file, SIGNAL(returnPressed()), this, SLOT(slot_choose_model()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_choose_model()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr, "open", ".", "*.obj");
    ui->lineEdit_file->setText(file_name);

    if (file_name.isEmpty())
    {
        ui->label_log->setText("File is not existed!");
        return;
    }

    QObjLoad objLoader_;
    QVector<float> vPoints_;
    if (!objLoader_.load(file_name, vPoints_))
    {
        ui->label_log->setText("Load model file failed!");
        return;
    }
    else
        ui->label_log->setText("Load model file successed!");

    if (vPoints_.size() < 3)
    {
        ui->label_log->setText("Model file is not correct!");
        return;
    }

    m_glWin = new GLWindow(vPoints_);
    ui->verticalLayout_model->addWidget(m_glWin);
    connect(ui->horizontalSlider_X, SIGNAL(valueChanged(int)), m_glWin, SLOT(slot_cam_X(int)));
    connect(ui->horizontalSlider_Y, SIGNAL(valueChanged(int)), m_glWin, SLOT(slot_cam_Y(int)));
    connect(ui->horizontalSlider_Z, SIGNAL(valueChanged(int)), m_glWin, SLOT(slot_cam_Z(int)));
}
