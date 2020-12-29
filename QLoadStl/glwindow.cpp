#include "glwindow.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QDebug>

GLWindow::GLWindow(QVector<float> vPoints, QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(nullptr)
{
    m_vPoints.clear();
    m_vPoints = vPoints;
}

GLWindow::~GLWindow()
{
    cleanup();
}

QSize GLWindow::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWindow::sizeHint() const
{
    return QSize(600, 600);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWindow::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot)
    {
        m_xRot = angle;
        update();
    }
}

void GLWindow::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot)
    {
        m_yRot = angle;
        update();
    }
}

void GLWindow::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot)
    {
        m_zRot = angle;
        update();
    }
}

void GLWindow::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_Vbo.destroy();
    delete m_program;
    m_program = nullptr;
    doneCurrent();
}

static const char *vertexShaderSourceCore =
        "#version 150\n"
        "in vec4 vertex;\n"
        "in vec3 normal;\n"
        "out vec3 vert;\n"
        "out vec3 vertNormal;\n"
        "uniform mat4 projMatrix;\n"
        "uniform mat4 mvMatrix;\n"
        "uniform mat3 normalMatrix;\n"
        "void main() {\n"
        "   vert = vertex.xyz;\n"
        "   vertNormal = normalMatrix * normal;\n"
        "   gl_Position = projMatrix * mvMatrix * vertex;\n"
        "}\n";

static const char *fragmentShaderSourceCore =
        "#version 150\n"
        "in highp vec3 vert;\n"
        "in highp vec3 vertNormal;\n"
        "out highp vec4 fragColor;\n"
        "uniform highp vec3 lightPos;\n"
        "void main() {\n"
        "   highp vec3 L = normalize(lightPos - vert);\n"
        "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
        "   highp vec3 color = vec3(1.0, 1.0, 0.0);\n"
        "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
        "   fragColor = vec4(col, 1.0);\n"
        "}\n";

void GLWindow::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWindow::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 0);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    if (m_program->link())
    {
        qDebug() << "link success!";
    }
    else
    {
        qDebug() << "link failed";
    }
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();
    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    // Setup our vertex buffer object.
    m_Vbo.create();
    m_Vbo.bind();
    m_Vbo.allocate(m_vPoints.data(), m_vPoints.size() * sizeof(float));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(10, 10, 10));

    m_program->release();
}

void GLWindow::setupVertexAttribs()
{
    m_Vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_Vbo.release();
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_world.setToIdentity();
    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    m_camera.setToIdentity();
    m_camera.lookAt(m_camera_pos, QVector3D(0, 0, 0), QVector3D(1, 0, 0));

    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    glDrawArrays(GL_TRIANGLES, 0, m_vPoints.size()/3);

    m_program->release();
}

void GLWindow::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWindow::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton)
    {
        setXRotation(m_xRot + dy);
        setYRotation(m_yRot + dx);
    }
    else if (event->buttons() & Qt::RightButton)
    {
        setXRotation(m_xRot + dy);
        setZRotation(m_zRot + dx);
    }
    m_lastPos = event->pos();
}

void GLWindow::slot_cam_X(int translateX)
{
    m_camera_pos.setX(translateX);
    update();
}

void GLWindow::slot_cam_Y(int translateY)
{
    m_camera_pos.setY(translateY);
    update();
}

void GLWindow::slot_cam_Z(int translateZ)
{
    m_camera_pos.setZ(translateZ);
    update();
}
