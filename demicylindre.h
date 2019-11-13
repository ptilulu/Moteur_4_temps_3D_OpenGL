#ifndef DEMICYLINDRE_H
#define DEMICYLINDRE_H

#include <QDebug>
#include <GL/glu.h>
#include <math.h>
#include <QVector3D>

class DemiCylindre
{
public:
    GLfloat lar_cyl; // epaisseur du coté
    GLfloat ep_cyl; // profondeur
    GLfloat r1_cyl; // rayon exterieur
    GLfloat r2_cyl; // rayon interieur
    GLint nb_fac;   // nombre de facettes
    GLint tf=(1+2+2+2+1); //triangle d'une face
    GLint tb=(2+2);     //taille des bordures
    GLint nb_triangles;
    GLint start;
    bool lissage;

    GLfloat red;

    GLint color[3]; // RVB
public:
    DemiCylindre(GLfloat lar_cyl, GLfloat r_cyl, GLfloat ep_cyl, GLint nb_fac,int r=255, int v=255, int b=255, bool lissage=true);
    void setColor(int r, int v, int b);
    void construire_demiCylindre(QVector<GLfloat> * vertData);
};

#endif // DEMICYLINDRE_H
