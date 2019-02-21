// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
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

private:
/*
    double m_x = 0;
    double m_y = 0;
    double m_z = 0;
    double m_angleX = 0;
    double m_angleY = 0;
    double m_angleZ = 0;
*/
    double m_x = 1.3;
    double m_y = 2.3;
    double m_z = -4;
    double m_angleX = 45;
    double m_angleY = 135;
    double m_angleZ = 0;


    QTimer *m_timer = nullptr;
    int m_anim = 0;
    double m_radius = 0.5;
    double m_ratio = 1;

    DemiCylindre *dc1   = new DemiCylindre  (2  , 0.5, 0.1, 20, 230, 240, 255);
    DemiCylindre *dc2   = new DemiCylindre  (2  , 0.5, 0.1, 20, 230, 240, 255);

    Cylindre *c1        = new Cylindre      (0.7, 0.1,      40, 180, 200, 200);
    Cylindre *c2        = new Cylindre      (0.3, 0.1,      40, 180, 200, 200);
    Cylindre *c3        = new Cylindre      (0.8, 0.15,     4 ,  90, 100, 100, false);
    Cylindre *c4        = new Cylindre      (0.2, 2  ,      20, 120, 130, 130, false);
    Cylindre *c5        = new Cylindre      (2  , 0.1,      40, 240, 250, 250);
    Cylindre *c6        = new Cylindre      (0.8, 0.4 ,     40,  50,  60,  60);

    int  red_location=2;

    QOpenGLShaderProgram *m_program;

    void makeGLObjects();
    void tearGLObjects();
    QOpenGLBuffer m_vbo;
};

#endif // GLAREA_H
