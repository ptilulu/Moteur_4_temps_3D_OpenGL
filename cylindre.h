#ifndef CYLINDRE_H
#define CYLINDRE_H

#include <QDebug>
#include <GL/glu.h>
#include <math.h>
#include <QVector3D>

class Cylindre
{
public:
    GLfloat lar_cyl; // epaisseur du coté
    GLfloat r_cyl; // rayon
    GLint nb_fac;   // nombre de facettes
    GLint tf=(1+2+1); //triangle d'une face
    GLint nb_triangles;
    GLint start;
    bool lissage;

    GLint color[3]; // RVB
public:
    Cylindre(GLfloat lar_cyl, GLfloat r_cyl, GLint nb_fac,int r=255, int v=255, int b=255, bool lissage=true);
    void setColor(int r, int v, int b);
    void construire_Cylindre(QVector<GLfloat> * vertData);
};

#endif // CYLINDRE_H
