// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "demicylindre.h"
#include "cylindre.h"

class GLArea : public QOpenGLWidget,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLArea(QWidget *parent = 0);
    ~GLArea();

public slots:
    void setRadius(double radius);


signals:  // On ne les implémente pas, elles seront générées par MOC ;
          // les paramètres seront passés aux slots connectés.
    void radiusChanged(double newRadius);

protected slots:
    void onTimeout();

protected:
    void initializeGL() override;
    void doProjection();
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
public :
    QTimer *m_timer = nullptr;

private:
/*
    double m_x = 0;
    double m_y = 0;
    double m_z = 0;
    double m_angleX = 0;
    double m_angleY = 0;
    double m_angleZ = 0;
*/
    QOpenGLTexture *m_textures[2];
    double m_x = 1.3;
    double m_y = 2.3;
    double m_z = -4;
    double m_angleX = 45;
    double m_angleY = 135;
    double m_angleZ = 0;


    int m_anim = 0;
    double m_radius = 0.5;
    double m_ratio = 1;

    DemiCylindre *dc1   = new DemiCylindre  (2  , 0.5,0.1f, 20, 230, 240, 255);         //cylindre coupé
    DemiCylindre *dc2   = new DemiCylindre  (2  , 0.5,0.1f, 20, 230, 240, 255);         //cylindre coupé (rouge)

    Cylindre *c1        = new Cylindre      (0.7, 0.1,      20, 180, 200, 200);         //grand tourillon
    Cylindre *c2        = new Cylindre      (0.3, 0.1,      20, 180, 200, 200);         //petit tourillon et maneton
    Cylindre *c3        = new Cylindre      (0.8, 0.15,     4 ,  90, 100, 100, false);  //masses vilbrequin
    Cylindre *c4        = new Cylindre      (0.2, 2  ,      40, 115, 130, 130);         //c1 roue moteur vilbrequin
    Cylindre *c5        = new Cylindre      (2  , 0.1,      20, 240, 250, 250);         //bielle
    Cylindre *c6        = new Cylindre      (0.8, 0.4 ,     40,  70,  80,  80);         //piston
    Cylindre *c7        = new Cylindre      (0.2, 1  ,      8 ,  80,  90,  90, false);  //c2 roue moteur vilbrequin

    int  red_location=2;

    QOpenGLShaderProgram *m_program;

    void makeGLObjects();
    void tearGLObjects();
    QOpenGLBuffer m_vbo;
};

#endif // GLAREA_H
