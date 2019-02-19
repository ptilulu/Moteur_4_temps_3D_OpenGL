// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QVector3D>

static const QString vertexShaderFile   = ":/vertex.glsl";
static const QString fragmentShaderFile = ":/fragment.glsl";


GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    qDebug() << "init GLArea" ;

    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);  // nb de sample par pixels : suréchantillonnage por l'antialiasing, en décalant à chaque fois le sommet
                        // cf https://www.khronos.org/opengl/wiki/Multisampling et https://stackoverflow.com/a/14474260
    setFormat(sf);
    qDebug() << "Depth is"<< format().depthBufferSize();

    setEnabled(true);  // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus); // accepte focus
    setFocus();                      // donne le focus

    m_timer = new QTimer(this);
    m_timer->setInterval(42);  // msec
    connect (m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect (this, SIGNAL(radiusChanged(double)), this, SLOT(setRadius(double)));
}

GLArea::~GLArea()
{
    qDebug() << "destroy GLArea";

    delete m_timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();
    tearGLObjects();
    doneCurrent();
}


void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();

    // shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);  // compile
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);
    if (! m_program->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }

    // récupère identifiants de "variables" dans les shaders
    //m_posAttr = m_program->attributeLocation("posAttr");
    //m_colAttr = m_program->attributeLocation("colAttr");
    //m_matrixUniform = m_program->uniformLocation("matrix");
}


void GLArea::makeGLObjects()
{
    qDebug() << __FUNCTION__ ;

    QVector<GLfloat> vertData;
    dc1->construire_demiCylindre(&vertData);nb_triangles= dc1->nb_triangles;
    c1->construire_Cylindre(&vertData);nb_triangles += c1->nb_triangles;

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}


void GLArea::tearGLObjects()
{
    m_vbo.destroy();
}


void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    m_ratio = (double) w / h;
    // doProjection();
}

void GLArea::paintGL()
{
    qDebug() << __FUNCTION__ ;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind(); // active le shader program

    QMatrix4x4 proj_mat;
    GLfloat hr = m_radius, wr = hr * m_ratio;
    proj_mat.frustum(-wr, wr, -hr, hr, 1.0, 10.0);

    QMatrix4x4 cam_mat;
    cam_mat.translate(0, 0, -3.0);


    QMatrix4x4 world_mat;
    world_mat.translate(m_x,m_y,m_z);

    world_mat.rotate(m_angleX, 1, 0, 0);
    world_mat.rotate(m_angleY, 0, 1, 0);
    world_mat.rotate(m_angleZ, 0, 0, 1);

    QMatrix3x3 normal_mat = world_mat.normalMatrix();

    m_program->setUniformValue("projMatrix", proj_mat);
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);

    m_program->setAttributeBuffer("posAttr",
        GL_FLOAT, 0 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
    m_program->setAttributeBuffer("colAttr",
        GL_FLOAT, 3 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
    m_program->setAttributeBuffer("norAttr",
        GL_FLOAT, 6 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
    m_program->enableAttributeArray("posAttr");
    m_program->enableAttributeArray("colAttr");
    m_program->enableAttributeArray("norAttr");
    int decal=0;

    glDrawArrays(GL_TRIANGLES, decal, dc1->nb_triangles*3);decal +=dc1->nb_triangles*3;


    world_mat.translate(0,0,-0.8001);

    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);



    glDrawArrays(GL_TRIANGLES, decal, c1->nb_triangles*3);decal +=c1->nb_triangles*3;

    m_program->disableAttributeArray("posAttr");
    m_program->disableAttributeArray("colAttr");
    m_program->disableAttributeArray("norAttr");

    m_program->release();
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    //qDebug() << __FUNCTION__ << ev->text();

    switch(ev->key()) {
        case Qt::Key_Space :
            m_angleY += 1;
            if (m_angleY >= 360) m_angleY -= 360;
            update();
            break;
        case Qt::Key_A :
            if (m_timer->isActive())
                m_timer->stop();
            else m_timer->start();
            break;
        case Qt::Key_R :
            if (ev->text() == "r")
                 setRadius(m_radius-0.05);
            else setRadius(m_radius+0.05);
            break;
        case Qt::Key_6 :
            m_angleY += 1;
            if (m_angleY >= 360) m_angleY -= 360;
            update();
            break;
        case Qt::Key_4 :
            m_angleY -= 1;
            if (m_angleY <= -1) m_angleY += 360;
            update();
            break;
        case Qt::Key_8 :
            m_angleX += 1;
            if (m_angleX >= 360) m_angleX -= 360;
            update();
            break;
        case Qt::Key_2 :
            m_angleX -= 1;
            if (m_angleX <= -1) m_angleX += 360;
            update();
            break;
        case Qt::Key_9 :
            m_angleZ += 1;
            if (m_angleZ >= 360) m_angleZ -= 360;
            update();
            break;
        case Qt::Key_7 :
            m_angleZ -= 1;
            if (m_angleZ <= -1) m_angleZ += 360;
            update();
            break;
        case Qt::Key_S :
            m_z-=0.1f;
            update();
            break;
        case Qt::Key_Z :
            m_z+=0.1f;
            update();
            break;
        case Qt::Key_Q :
            m_x-=0.1f;
            update();
            break;
        case Qt::Key_D :
            m_x+=0.1f;
            update();
            break;
        case Qt::Key_W :
            m_y-=0.1f;
            update();
            break;
        case Qt::Key_X :
            m_y+=0.1f;
            update();
            break;
        case Qt::Key_Return :
            m_x=0;
            m_y=0;
            m_z=0;
            m_angleX=0;
            m_angleY=0;
            m_angleZ=0;
            update();
            break;
    }
}

void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    (void) ev;
    //qDebug() << __FUNCTION__ << ev->text();
}

void GLArea::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y();
}

void GLArea::onTimeout()
{
    qDebug() << __FUNCTION__ ;
    m_anim += 0.01;
    if (m_anim > 1) m_anim = 0;
    update();
}

void GLArea::setRadius(double radius)
{
    qDebug() << __FUNCTION__ << radius << sender();
    if (radius != m_radius && radius > 0.01 && radius <= 10) {
        m_radius = radius;
        qDebug() << "  emit radiusChanged()";
        emit radiusChanged(radius);
        update();
    }
}




