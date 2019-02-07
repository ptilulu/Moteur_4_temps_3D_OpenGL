#ifndef CYLINDRE_H
#define CYLINDRE_H

#include <QVector>
#include <GL/glu.h>

class Cylindre
{
public:
    GLfloat ep_cyl; // epaisseur AB
    GLfloat r_cyl; //rayon AC
    GLint nb_fac; // nombre de facettes
    GLint color[3]; // RVB
    boolean flag_fill;
    GLfloat *vertices;
    GLfloat *colors;

public:
    Cylindre(GLdouble ep_cyl, GLdouble r_cyl, GLint nb_fac,int r=255, int v=255, int b=255);
    void setColor(int r, int v, int b);
    void construire_cylindre();
    void construire_cylindre(QVector<GLfloat> vertices, QVector<GLfloat> colors);
};

#endif // CYLINDRE_H
