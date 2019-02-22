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
    int decal=0;
    c1->construire_Cylindre(&vertData);     c1->start=decal;    decal+=c1->nb_triangles*3;      //grand tourillon
    c2->construire_Cylindre(&vertData);     c2->start=decal;    decal+=c2->nb_triangles*3;      //petit tourillon et maneton
    c3->construire_Cylindre(&vertData);     c3->start=decal;    decal+=c3->nb_triangles*3;      //masses vilbrequin
    c4->construire_Cylindre(&vertData);     c4->start=decal;    decal+=c4->nb_triangles*3;      //c1 roue moteur vilbrequin
    c5->construire_Cylindre(&vertData);     c5->start=decal;    decal+=c5->nb_triangles*3;      //bielle
    c6->construire_Cylindre(&vertData);     c6->start=decal;    decal+=c6->nb_triangles*3;      //piston
    c7->construire_Cylindre(&vertData);     c7->start=decal;    decal+=c7->nb_triangles*3;      //c2 roue moteur vilbrequin
    dc1->construire_demiCylindre(&vertData);dc1->start=decal;   decal+=dc1->nb_triangles*3;     //cylindre coupé

    dc2->red=0.5;
    dc2->construire_demiCylindre(&vertData);dc2->start=decal;   decal+=dc2->nb_triangles*3;     //cylindre coupé (rouge)

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
    proj_mat.frustum(-wr, wr, -hr, hr, 1.0, 40.0);

    QMatrix4x4 cam_mat;
    cam_mat.translate(0,0,0);

    QMatrix4x4 world_mat;
    world_mat.translate(m_x,m_y,m_z     );
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

    QMatrix4x4 base_world_mat=world_mat;

    world_mat.rotate(m_anim,0,0,1);
    double d_z=0;      //décalage incrémenté du vilbrequin
    double c3_dz=c3->r_cyl*(sqrt(2)/2);
    double t_x1=c3->lar_cyl/2-c2->r_cyl;   //espacement des masses
    double t_x2=t_x1+c3->lar_cyl/2-c2->r_cyl;   //espacement de maneton
    double z_maneton_1,z_maneton_2,z_maneton_3,z_maneton_4;

    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    QMatrix4x4 base_vilbrequin_mat=world_mat;
    glDrawArrays(GL_TRIANGLES, c1->start, c1->nb_triangles*3);
    d_z-=c1->lar_cyl/2; //tourillon 1

    d_z-=c3_dz;
    world_mat.translate(t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 1 (gros pavé gris foncé)

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(t_x2,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    z_maneton_1=d_z;
    d_z-=c2->lar_cyl/2;//maneton 1

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz; //masse 2

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(0,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    d_z-=c2->lar_cyl/2;  //tourillon 2

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(-t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 3

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(-t_x2,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    z_maneton_2=d_z;
    d_z-=c2->lar_cyl/2;  //maneton 2

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(-t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 4

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(0,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    d_z-=c2->lar_cyl/2;  //tourillon 3

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(-t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 5

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(-t_x2,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    z_maneton_3=d_z;
    d_z-=c2->lar_cyl/2;  //maneton 3

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(-t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 6

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(0,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    d_z-=c2->lar_cyl/2;  //tourillon 4

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 7

    d_z-=c2->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(t_x2,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c2->start, c2->nb_triangles*3);
    z_maneton_4=d_z;
    d_z-=c2->lar_cyl/2;  //maneton 4

    d_z-=c3_dz;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(t_x1,0,d_z);
    world_mat.rotate(45,1,0,0);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c3->start, c3->nb_triangles*3);
    d_z-=c3_dz;  //masse 8

    d_z-=c1->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(0,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c1->start, c1->nb_triangles*3);
    d_z-=c1->lar_cyl/2;  //tourillon 5

    d_z-=c4->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(0,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c4->start, c4->nb_triangles*3);
    d_z-=c4->lar_cyl/2;  //cylindre1 vilbrequin

    d_z-=c7->lar_cyl/2;
    world_mat=base_vilbrequin_mat;
    world_mat.translate(0,0,d_z);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c7->start, c7->nb_triangles*3);//cylindre2 vilbrequin


    double alpha1=(m_anim*M_PI/180);
    double alpha2=alpha1+M_PI; if(alpha2>=2*M_PI){alpha2=alpha1-M_PI;}
    double taille_bielle=2;
    double h_1 = t_x2*sin(alpha1);
    double h_2 = t_x2*sin(alpha2);
    double x_piston_1 = t_x2*cos(alpha1)-sqrt(pow(taille_bielle,2)-pow(h_1,2));
    double x_piston_2 = t_x2*cos(alpha2)-sqrt(pow(taille_bielle,2)-pow(h_2,2));

    double beta1=asin(h_1/taille_bielle);//if(alpha1>=M_PI){beta1=-beta1;}
    double beta2=asin(h_2/taille_bielle);//if(alpha2>=M_PI){beta2=-beta2;}
    beta1=beta1*180/M_PI;
    beta2=beta2*180/M_PI;


    world_mat=base_world_mat;
    world_mat.translate((x_piston_1+t_x2*cos(alpha1))/2,h_1/2,z_maneton_1);
    world_mat.rotate(90,0,1,0);
    world_mat.rotate(-beta1,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c5->start, c5->nb_triangles*3);
    //bielle 1

    world_mat=base_world_mat;
    world_mat.translate((x_piston_2+t_x2*cos(alpha2))/2,h_2/2,z_maneton_2);
    world_mat.rotate(90,0,1,0);
    world_mat.rotate(-beta2,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c5->start, c5->nb_triangles*3);
    //bielle 2

    world_mat=base_world_mat;
    world_mat.translate((x_piston_2+t_x2*cos(alpha2))/2,h_2/2,z_maneton_3);
    world_mat.rotate(90,0,1,0);
    world_mat.rotate(-beta2,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c5->start, c5->nb_triangles*3);
    //bielle 3

    world_mat=base_world_mat;
    world_mat.translate((x_piston_1+t_x2*cos(alpha1))/2,h_1/2,z_maneton_4);
    world_mat.rotate(90,0,1,0);
    world_mat.rotate(-beta1,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c5->start, c5->nb_triangles*3);
    //bielle 4

    world_mat=base_world_mat;
    world_mat.translate(x_piston_1-c6->lar_cyl/2,0,z_maneton_1);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c6->start, c6->nb_triangles*3);
    //piston 1

    world_mat=base_world_mat;
    world_mat.translate(x_piston_2-c6->lar_cyl/2,0,z_maneton_2);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c6->start, c6->nb_triangles*3);
    //piston 2

    world_mat=base_world_mat;
    world_mat.translate(x_piston_2-c6->lar_cyl/2,0,z_maneton_3);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c6->start, c6->nb_triangles*3);
    //piston 3

    world_mat=base_world_mat;
    world_mat.translate(x_piston_1-c6->lar_cyl/2,0,z_maneton_4);
    world_mat.rotate(90,0,1,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    glDrawArrays(GL_TRIANGLES, c6->start, c6->nb_triangles*3);
    //piston 4


    world_mat=base_world_mat;
    world_mat.translate(dc1->lar_cyl/2-c6->lar_cyl-t_x2-taille_bielle-0.001,0,z_maneton_1);
    world_mat.rotate(-90,0,1,0);
    world_mat.rotate(180,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    if(red_location==1){
        glDrawArrays(GL_TRIANGLES, dc2->start, dc2->nb_triangles*3);
    }else{
        glDrawArrays(GL_TRIANGLES, dc1->start, dc1->nb_triangles*3);
    }
    //chambre 1

    world_mat=base_world_mat;
    world_mat.translate(dc1->lar_cyl/2-c6->lar_cyl-t_x2-taille_bielle-0.001,0,z_maneton_2);
    world_mat.rotate(-90,0,1,0);
    world_mat.rotate(180,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    if(red_location==2){
        glDrawArrays(GL_TRIANGLES, dc2->start, dc2->nb_triangles*3);
    }else{
        glDrawArrays(GL_TRIANGLES, dc1->start, dc1->nb_triangles*3);
    }
    //chambre 2

    world_mat=base_world_mat;
    world_mat.translate(dc1->lar_cyl/2-c6->lar_cyl-t_x2-taille_bielle-0.001,0,z_maneton_3);
    world_mat.rotate(-90,0,1,0);
    world_mat.rotate(180,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    if(red_location==3){
        glDrawArrays(GL_TRIANGLES, dc2->start, dc2->nb_triangles*3);
    }else{
        glDrawArrays(GL_TRIANGLES, dc1->start, dc1->nb_triangles*3);
    }//chambre 3

    world_mat=base_world_mat;
    world_mat.translate(dc1->lar_cyl/2-c6->lar_cyl-t_x2-taille_bielle-0.001,0,z_maneton_4);
    world_mat.rotate(-90,0,1,0);
    world_mat.rotate(180,1,0,0);
    normal_mat = world_mat.normalMatrix();
    m_program->setUniformValue("mvMatrix", cam_mat*world_mat);
    m_program->setUniformValue("norMatrix", normal_mat);
    if(red_location==4){
        glDrawArrays(GL_TRIANGLES, dc2->start, dc2->nb_triangles*3);
    }else{
        glDrawArrays(GL_TRIANGLES, dc1->start, dc1->nb_triangles*3);
    }//chambre 4

    m_program->disableAttributeArray("posAttr");
    m_program->disableAttributeArray("colAttr");
    m_program->disableAttributeArray("norAttr");

    m_program->release();
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();

    GLfloat value;
    switch(ev->key()) {
        case Qt::Key_Space :
            m_angleY += 1;
            if (m_angleY >= 360) m_angleY -= 360;
            update();
            break;
        case Qt::Key_S :
                m_timer->stop();
            break;
        case Qt::Key_D :
                m_timer->start();
            break;
        case Qt::Key_A :
            value = m_timer->interval();
            value = value*10000;
            if (ev->text() == "a"){
                value=value*1.1f;
                m_timer->setInterval(value/10000+1);
            }
            else {
                value=value*0.8f;
                m_timer->setInterval(value/10000);
            }
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
        case Qt::Key_G :
            m_z-=0.1f;
            update();
            break;
        case Qt::Key_T :
            m_z+=0.1f;
            update();
            break;
        case Qt::Key_F :
            m_x+=0.1f;
            update();
            break;
        case Qt::Key_H :
            m_x-=0.1f;
            update();
            break;
        case Qt::Key_U :
            m_y-=0.1f;
            update();
            break;
        case Qt::Key_J :
            m_y+=0.1f;
            update();
            break;
        case Qt::Key_Return :
            m_x=0;
            m_y=0;
            m_z=-3;
            m_angleX=0;
            m_angleY=0;
            m_angleZ=0;
            update();
            break;
        case Qt::Key_Escape:
            qDebug() <<"m_x" << m_x
                     <<"m_y" << m_y
                     <<"m_z" << m_z
                     <<"a_x" << m_angleX
                     <<"a_y" << m_angleY
                     <<"a_z" << m_angleZ;

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
    m_anim += 2;
    if (m_anim > 360) m_anim = 0;
    if(m_anim==0 || m_anim==180){
        if (red_location==1) red_location=3;
        else if (red_location==2) red_location=1;
        else if (red_location==3) red_location=4;
        else if (red_location==4) red_location=2;
    }
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




