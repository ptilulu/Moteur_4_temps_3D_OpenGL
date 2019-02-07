// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVector>
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
    void drawCylindre(Cylindre *c, QMatrix4x4 matrix);

private:
    double m_angle = 0;
    QTimer *m_timer = nullptr;
    float m_anim = 0;
    double m_radius = 0.5;
    double m_ratio = 1;
    float  m_x = 0;
    float  m_y = 0;
    float  m_z = 0;


    // Pour utiliser les shaders
    QOpenGLShaderProgram *m_program;
    int m_posAttr;
    int m_colAttr;
    int m_matrixUniform;

    Cylindre *c1 =new Cylindre (0.25, 1   , 20 , 255, 0  , 0  );
    Cylindre *c2 =new Cylindre (1.4 , 0.08, 8  , 255, 255, 255);
    Cylindre *c3 =new Cylindre (0.8 , 0.06, 6  , 0  , 255, 255);
    Cylindre *c4 =new Cylindre (0.2 , 0.3 , 10 , 120, 0  , 255);
    Cylindre *c5 =new Cylindre (0.2 , 0.3 , 10 , 120, 0  , 255);
    Cylindre *c6 =new Cylindre (0.2 , 0.2 , 20 , 0  , 0  , 255);
    Cylindre *c7 =new Cylindre (0.7 , 0.04, 6  , 255, 255, 0  );
    Cylindre *c8 =new Cylindre (2   , 0.14, 4  , 120, 0  , 255);
    Cylindre *c9 =new Cylindre (2   , 0.14, 4  , 0  , 0  , 255);
    Cylindre *c10 =new Cylindre(2   , 0.3 , 4  , 0  , 120, 255);
};

#endif // GLAREA_H
