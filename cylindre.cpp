#include "cylindre.h"
#include <GL/glu.h>
#include <QDebug>
#include <math.h>


Cylindre::Cylindre(GLdouble ep_cyl, GLdouble r_cyl, GLint nb_fac,int r, int v, int b)
{
    this->ep_cyl = ep_cyl; // epaisseur AB
    this->r_cyl = r_cyl;   //rayon AC
    this->nb_fac = nb_fac; // nombre de facettes
    this->setColor(r,v,b);
    this->vertices=new GLfloat[this->nb_fac*36];
    this->colors=new GLfloat[this->nb_fac*36];
}

void Cylindre::setColor(int r, int v, int b){
    color[0] = r; // rouge
    color[1] = v; // vert
    color[2] = b; // bleu
}

void Cylindre::construire_cylindre(){
    GLfloat alpha = 2 * M_PI/nb_fac;
    for (int i=0; i<nb_fac;++i) {
        int d=0;
        int dc=0;
        for (int j=1;j>=-1;j=j-2) {  //pos face avant j=1 face arrière j=-1
            vertices[i*36+d]=0;                     d++;    vertices[i*36+d]=0;                     d++;    vertices[i*36+d]=j*ep_cyl/2;d++;
            vertices[i*36+d]=r_cyl*cos(i*alpha);    d++;    vertices[i*36+d]=r_cyl*sin(i*alpha);    d++;    vertices[i*36+d]=j*ep_cyl/2;d++;
            vertices[i*36+d]=r_cyl*cos((i+1)*alpha);d++;    vertices[i*36+d]=r_cyl*sin((i+1)*alpha);d++;    vertices[i*36+d]=j*ep_cyl/2;d++;
        }

        for (int j=0;j < 6;++j) {   //pos facettes
            if(j==0 || j==1 || j==5){
                vertices[i*36+d]=r_cyl*cos(i*alpha);    d++;    vertices[i*36+d]=r_cyl*sin(i*alpha);    d++;   vertices[i*36+d]=((-0.5f+(j%2))*2)*ep_cyl/2;d++;
            }
            else{
                vertices[i*36+d]=r_cyl*cos((i+1)*alpha);d++;    vertices[i*36+d]=r_cyl*sin((i+1)*alpha);d++;   vertices[i*36+d]=((-0.5f+(j%2))*2)*ep_cyl/2;d++;
            }
        }
            for (int k=0;k < 3;++k) {   //color centre cylindre
                colors[i*36+k]=color[k]/255.0f;dc++;
            }
        for (int j=1;j < 6;++j) {    //color faces 1-2
            for (int k=0;k < 3;++k) {
                colors[i*36+j*3+k]=(color[k]/255.0f)*0.8f;dc++;
            }
        }for (int j=0;j < 6;++j) {    //color facettes 1-2
            for (int k=0;k < 3;++k) {
                colors[i*36+j*3+k+18]=(color[k]/255.0f)*0.6f;dc++;
            }
        }for (int j=0;j < 3;++j) {    //color facette2
            for (int k=0;k < 3;++k) {
                colors[i*36+j*3+k+27]=(color[k]/255.0f)*0.65f;dc++;
            }
        }
    }
}

//je n'arrive pas a faire l'affichage avec les QVector mais les calculs sont là
void Cylindre::construire_cylindre(QVector<GLfloat> vertices, QVector<GLfloat> colors){
    GLfloat alpha = 2 * M_PI/nb_fac;
    for (int i=0; i<nb_fac;++i) {
        for (int j=1;j>=-1;j=j-2) {  //pos face avant j=1 face arrière j=-1
            vertices.append(0);                         vertices.append(0);                         vertices.append(j*ep_cyl/2);
            vertices.append(r_cyl*cos(i*alpha));        vertices.append(r_cyl*sin(i*alpha));        vertices.append(j*ep_cyl/2);
            vertices.append(r_cyl*cos((i+1)*alpha));    vertices.append(r_cyl*sin((i+1)*alpha));    vertices.append(j*ep_cyl/2);
        }

        for (int j=0;j < 6;++j) {   //pos facettes
            if(j==0 || j==1 || j==5){
                vertices.append(r_cyl*cos(i*alpha));    vertices.append(r_cyl*sin(i*alpha));       vertices.append(((-0.5f+(j%2))*2)*ep_cyl/2);
            }
            else{
                vertices.append(r_cyl*cos((i+1)*alpha));   vertices.append(r_cyl*sin((i+1)*alpha));   vertices.append(((-0.5f+(j%2))*2)*ep_cyl/2);
            }
        }

        for (int j=0;j < 6;++j) {    //color faces
            for (int k=0;k < 3;++k) {
                colors.append(color[k]/255.0f);
            }
        }for (int j=0;j < 6;++j) {    //color facettes
            for (int k=0;k < 3;++k) {
                colors.append((color[k]/255.0f)*0.8f);
            }
        }
    }
}
