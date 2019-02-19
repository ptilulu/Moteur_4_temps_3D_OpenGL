#include "cylindre.h"

Cylindre::Cylindre(GLfloat lar_cyl, GLfloat r_cyl, GLint nb_fac,int r, int v, int b)
{
    this->lar_cyl = lar_cyl;    // profondeur du cylindre
    this->r_cyl = r_cyl;       // rayon
    this->nb_fac = nb_fac;      // nombre de facettes
    this->nb_triangles=(tf*nb_fac);
    this->setColor(r,v,b);
}

void Cylindre::setColor(int r, int v, int b){
    color[0] = r; // rouge
    color[1] = v; // vert
    color[2] = b; // bleu
}

void Cylindre::construire_Cylindre(QVector<GLfloat> * vertData){
    GLfloat alpha = 2*M_PI/nb_fac;
    int A=0,B=1,C=2,D=3,E=4,F=5;
    QVector3D last;
    for (int i=0;i<nb_fac;i++){
        GLfloat vs[] = {
            0,                         0,                         -lar_cyl/2,     //A
            r_cyl*cos((i+0)*alpha),    r_cyl*sin((i+0)*alpha),    -lar_cyl/2,     //B
            r_cyl*cos((i+1)*alpha),    r_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //C
            r_cyl*cos((i+1)*alpha),    r_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //D
            r_cyl*cos((i+0)*alpha),    r_cyl*sin((i+0)*alpha),    +lar_cyl/2,     //E
            0,                         0,                         +lar_cyl/2,     //F
        };
        int ind_ver[] = { A,B,C, B,C,D, B,D,E, D,E,F };

        GLdouble colors[] = {
            color[0]/255., color[1]/255., color[2]/255.,    //couleur générale
            color[0]/255.+0.5, color[1]/255.+0.5, color[2]/255.+0.5,    //couleur en A et F
        };
        //                A,B,C, B,C,D, B,D,E, D,E,F
        int ind_col[] = { 1,0,0, 0,0,0, 0,0,0, 0,0,1, };

        QVector3D vAB(vs[B*3+0]-vs[A*3+0], vs[B*3+1]-vs[A*3+1], vs[B*3+2]-vs[A*3+2]);
        QVector3D vBC(vs[C*3+0]-vs[B*3+0], vs[C*3+1]-vs[B*3+1], vs[C*3+2]-vs[B*3+2]);
        QVector3D vDC(vs[C*3+0]-vs[D*3+0], vs[C*3+1]-vs[D*3+1], vs[C*3+2]-vs[D*3+2]);

        //ABC:AB,AC; ABC:AC,CB; ABC:AB,CB;
        QVector3D nACB = QVector3D::normal(vBC, vAB);
        QVector3D nBCD = QVector3D::normal(vDC, vBC);
        QVector3D nDEF = -nACB;

        if(i==0){
            last=nBCD;
        }

        QVector3D nBE  = (last+nBCD)/2;

        last=nBCD;

        GLfloat normals[] = {
            nACB.x(), nACB.y(), nACB.z(),
            nBCD.x(), nBCD.y(), nBCD.z(),
            nDEF.x(), nDEF.y(), nDEF.z(),
            nBE.x() , nBE.y() , nBE.z() ,
        };
        //                A,B,C, B,C,D, B,D,E, D,E,F

      //int ind_nor[] = { 0,0,0, 1,1,1, 1,1,1, 2,2,2, };
      int ind_nor[] = { 0,0,0, 3,1,1, 3,1,3, 2,2,2, };

        for (int i = 0; i < 4*3; ++i) {
            // coordonnées sommets
            for (int j = 0; j < 3; j++)
                vertData->append(vs[ind_ver[i]*3+j]);
            // couleurs sommets
            for (int j = 0; j < 3; j++)
                vertData->append(colors[ind_col[i]*3+j]);
            // normales sommets
            for (int j = 0; j < 3; j++)
                vertData->append(normals[ind_nor[i]*3+j]);
        }
    }
}
