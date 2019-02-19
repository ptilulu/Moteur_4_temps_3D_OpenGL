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

/*
    QVector<GLfloat> vertData;
    GLfloat alpha = M_PI/c1->nb_fac;
    int A=0,B=1,C=2,D=3,E=4,F=5,G=6,H=7,I=8;
    GLfloat r1_cyl=c1->r1_cyl, r2_cyl=c1->r2_cyl, lar_cyl=c1->lar_cyl;
    for(int i=0;i<2;i++){
    GLfloat vs[] = {
        0,                          0,                          -lar_cyl/2,     //A
        r1_cyl*cos(i*alpha),        r1_cyl*sin(i*alpha),        -lar_cyl/2,     //B
        r1_cyl*cos((i+1)*alpha),    r1_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //C
        r1_cyl*cos((i+1)*alpha),    r1_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //D
        r1_cyl*cos((i+2)*alpha),    r1_cyl*sin((i+2)*alpha),    +lar_cyl/2,     //E
        r2_cyl*cos(i*alpha),        r2_cyl*sin(i*alpha),        -lar_cyl/2,     //F
        r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //G
        r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //H
        r2_cyl*cos((i+2)*alpha),    r2_cyl*sin((i+2)*alpha),    +lar_cyl/2,     //I
    };
    int ind_ver[] = { A,C,B, B,C,D, C,D,E, F,G,H, G,H,I, D,H,I, D,E,I };

    GLdouble colors[] = {
        c1->color[0]/255., c1->color[1]/255., c1->color[2]/255.,      // triangle 0
    };
    int ind_col[] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0};

    QVector3D vAB(vs[B*3+0]-vs[A*3+0], vs[B*3+1]-vs[A*3+1], vs[B*3+2]-vs[A*3+2]);
    QVector3D vBC(vs[C*3+0]-vs[B*3+0], vs[C*3+1]-vs[B*3+1], vs[C*3+2]-vs[B*3+2]);
    QVector3D vBD(vs[D*3+0]-vs[B*3+0], vs[D*3+1]-vs[B*3+1], vs[D*3+2]-vs[B*3+2]);
    QVector3D vCE(vs[E*3+0]-vs[C*3+0], vs[E*3+1]-vs[C*3+1], vs[E*3+2]-vs[C*3+2]);
    QVector3D vDE(vs[E*3+0]-vs[D*3+0], vs[E*3+1]-vs[D*3+1], vs[E*3+2]-vs[D*3+2]);
    QVector3D vFH(vs[H*3+0]-vs[F*3+0], vs[H*3+1]-vs[F*3+1], vs[H*3+2]-vs[F*3+2]);
    QVector3D vGH(vs[H*3+0]-vs[G*3+0], vs[H*3+1]-vs[G*3+1], vs[H*3+2]-vs[G*3+2]);
    QVector3D vGI(vs[I*3+0]-vs[G*3+0], vs[B*3+1]-vs[G*3+1], vs[I*3+2]-vs[G*3+2]);
    QVector3D vDI(vs[I*3+0]-vs[D*3+0], vs[B*3+1]-vs[D*3+1], vs[I*3+2]-vs[D*3+2]);
    QVector3D vDH(vs[H*3+0]-vs[D*3+0], vs[H*3+1]-vs[D*3+1], vs[H*3+2]-vs[D*3+2]);

    //ABC:AB,AC; ABC:AC,CB; ABC:AB,CB;
    QVector3D nACB = QVector3D::normal(vAB, vBC);
    QVector3D nBCD = QVector3D::normal(vBC, vBD);
    QVector3D nCED = QVector3D::normal(vCE, vDE);
    QVector3D nFHG = QVector3D::normal(vFH, vGH);
    QVector3D nGHI = QVector3D::normal(vGH, vGI);
    QVector3D nDIH = QVector3D::normal(vDI, vDH);
    QVector3D nDEI = QVector3D::normal(vDE, vDI);

#if 1
    // Normales des triangles
    GLfloat normals[] = {
            nACB.x(), nACB.y(), nACB.z(),
            nBCD.x(), nBCD.y(), nBCD.z(),
            nCED.x(), nCED.y(), nCED.z(),
            nFHG.x(), nFHG.y(), nFHG.z(),
            nGHI.x(), nGHI.y(), nGHI.z(),
            nDIH.x(), nDIH.y(), nDIH.z(),
            nDEI.x(), nDEI.y(), nDEI.z(),
    };
    int ind_nor[] = { 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7};
#else
    // Pour lissage de Phong
    QVector3D nAB = (nACB + nABD) / 2.0;

    GLfloat normals[] = { nACB.x(), nACB.y(), nACB.z(),
                          nABD.x(), nABD.y(), nABD.z(),
                          nAB.x(), nAB.y(), nAB.z()};
                   // A  C  B  A  B  D
    int ind_nor[] = { 2, 0, 2, 2, 2, 1 };
#endif

    for (int i = 0; i < 7*3; ++i) {
        // coordonnées sommets
        for (int j = 0; j < 3; j++)
            vertData.append(vs[ind_ver[i]*3+j]);
        // couleurs sommets
        for (int j = 0; j < 3; j++)
            vertData.append(colors[ind_col[i]*3+j]);
        // normales sommets
        for (int j = 0; j < 3; j++)
            vertData.append(normals[ind_nor[i]*3+j]);
    }
}
*/

    qDebug() << "yolo" <<nb_triangles;
    QVector<GLfloat> vertData;
    c1->construire_demiCylindre(&vertData);

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

    glDrawArrays(GL_TRIANGLES, 0, nb_triangles*3);

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




