// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <math.h>

static const QString vertexShaderFile   = ":/basic.vsh";
static const QString fragmentShaderFile = ":/basic.fsh";


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
    m_timer->setInterval(50);  // msec
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

    // ici destructions de ressources GL

    doneCurrent();
}


void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    // shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);  // compile
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);
    if (! m_program->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }

    // récupère identifiants de "variables" dans les shaders
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
}

void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    m_ratio = (double) w / h;
    // doProjection();
}

void GLArea::drawCylindre(Cylindre *c, QMatrix4x4 matrix){
    m_program->bind(); // active le shader program
    m_program->setUniformValue(m_matrixUniform, matrix);
    c->construire_cylindre();
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, c->vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, c->colors);
    glEnableVertexAttribArray(m_posAttr);  // rend le VAA accessible pour glDrawArrays
    glEnableVertexAttribArray(m_colAttr);
    glDrawArrays(GL_TRIANGLES, 0, 12*c->nb_fac);
    m_program->release();
}

void GLArea::paintGL()
{
    qDebug() << __FUNCTION__ ;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind(); // active le shader program

    QMatrix4x4 matrix;
    GLfloat hr = m_radius, wr = hr * m_ratio;            // = glFrustum
    matrix.frustum(-wr, wr, -hr, hr, 1.0, 10.0);
    //matrix.perspective(60.0f, m_ratio, 0.1f, 100.0f);  // = gluPerspective
    // Remplace gluLookAt (0, 0, 3.0, 0, 0, 0, 0, 1, 0);
    matrix.translate(0, 0, -3.0);
    // Rotation de la scène pour l'animation
    matrix.rotate(m_angle, 0, 1, 0);
    matrix.translate(-m_x, -m_y, -m_z);

    double GH=0.8;
    double HJ=2;
    double HI=GH*sin(-m_anim*M_PI/(180));;
    double xJ=-((-1*GH*cos(-m_anim*M_PI/(180))) + sqrt(pow(HJ,2)-pow(HI,2)));;
    double JI=(xJ-GH*cos(-m_anim*M_PI/(180)));
    double beta=atan(HI/JI)*180/M_PI;

    QMatrix4x4 minit=matrix;

    // cylindre principal et axe
    matrix.rotate(-m_anim, 0, 0, 1);
        drawCylindre(c1,matrix);

        matrix.translate(0, 0, -0.5);
            drawCylindre(c2,matrix);
    matrix=minit;

    // cylindre rotatif et axe
    matrix.translate(GH*cos(-m_anim*M_PI/(180)), GH*sin(-m_anim*M_PI/(180)), 0.3);
        QMatrix4x4 mcylindrerotatif=matrix;
        matrix.translate(0, 0, -0.05);
        matrix.rotate(-m_anim, 0, 0, 1);
            drawCylindre(c3,matrix);
        matrix=mcylindrerotatif;
        matrix.rotate(-beta, 0, 0, 1);
        matrix.translate(0, 0, 0.2);
            drawCylindre(c4,matrix);
    matrix=minit;

    //cylindres exterieur + piece HJ
    matrix.translate(xJ,0,0.5);
        QMatrix4x4 mcylindreexterieur=matrix;
            matrix.rotate(-beta, 0, 0, 1);
            drawCylindre(c5,matrix);
        matrix=mcylindreexterieur;
            matrix.translate(0,0,0.4);
            drawCylindre(c6,matrix);
        matrix=mcylindreexterieur;
            matrix.translate(0,0,0.16);
            drawCylindre(c7,matrix);
    matrix=minit;

    //piece HJ
    matrix.translate((xJ+GH*cos(-m_anim*M_PI/(180)))/2,(GH*sin(-m_anim*M_PI/(180)))/2,0.5); // xJ+GH*cos(-m_alpha*M_PI/(180)))/2 signe inverser pour GH*cos(-m_alpha*M_PI/(180)))/2 mais ça marche de cette manière seulement
    matrix.rotate(-beta , 0, 0, 1);
    matrix.rotate(90, 0, 1, 0);
    matrix.rotate(45, 0, 0, 1);
        drawCylindre(c8, matrix);
    matrix=minit;

    //le piston et sa base
    matrix.translate(xJ-1,0,0.9);
    matrix.rotate(90, 0, 1, 0);
    matrix.rotate(45, 0, 0, 1);
        drawCylindre(c9,matrix);
    matrix=minit;
    matrix.translate(-4.1,0,0.9);
    matrix.rotate(90, 0, 1, 0);
    matrix.rotate(45, 0, 0, 1);
        drawCylindre(c10,matrix);
    matrix=minit;

}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();

    switch(ev->key()) {
        case Qt::Key_4 :
            m_angle += 1;
            if (m_angle >= 360) m_angle -= 360;
            update();
            break;
        case Qt::Key_6 :
            m_angle -= 1;
            if (m_angle <= -1) m_angle += 360;
            update();
            break;
        case Qt::Key_8 :
            m_y += 0.1f;
            update();
            break;
        case Qt::Key_2 :
            m_y -= 0.1f;
            update();
            break;
        case Qt::Key_A :
            if (m_timer->isActive())
                m_timer->stop();
            else m_timer->start();
            break;
        case Qt::Key_R :
            if (ev->text() == "r")
                 setRadius(m_radius-0.05f);
            else setRadius(m_radius+0.05f);
            break;
        case Qt::Key_Z :
            m_z+=0.1f;
            update();
            break;
        case Qt::Key_Q :
            m_x-=0.1f;
            update();
            break;
        case Qt::Key_S :
            m_z-=0.1f;
            update();
            break;
        case Qt::Key_D :
            m_x+=0.1f;
            update();
            break;
    }
}

void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();
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
    m_anim += 2;
    if (m_anim > 359) m_anim = 0;
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




