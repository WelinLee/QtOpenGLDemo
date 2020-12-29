#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <gl/GL.h>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWindow(QVector<float> vPoints, QWidget *parent = 0);
    ~GLWindow();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void cleanup();
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void slot_cam_X(int translateX);
    void slot_cam_Y(int translateY);
    void slot_cam_Z(int translateZ);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void setupVertexAttribs();

    int m_xRot;
    int m_yRot;
    int m_zRot;
    QPoint m_lastPos;
    QVector<float> m_vPoints;
    QOpenGLBuffer m_Vbo;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    QVector3D m_camera_pos;

};

#endif // GLWINDOW_H
