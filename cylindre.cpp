#include "cylindre.h"

Cylindre::Cylindre(GLfloat lar_cyl, GLfloat r_cyl, GLint nb_fac,int r, int v, int b, bool lissage)
{
    this->lar_cyl = lar_cyl;    // profondeur du cylindre
    this->r_cyl = r_cyl;       // rayon
    this->nb_fac = nb_fac;      // nombre de facettes
    this->nb_triangles=(tf*nb_fac);
    this->lissage = lissage;
    this->setColor(r,v,b);
}

void Cylindre::setColor(int r, int v, int b){
    color[0] = r; // rouge
    color[1] = v; // vert
    color[2] = b; // bleu
}

void Cylindre::construire_Cylindre(QVector<GLfloat> * vertData){
    GLfloat alpha = 2*M_PI/nb_fac;
    int A=0,B=1,C=2,D=3,E=4,F=5,Y=6,Z=7;
    QVector3D last;
    for (int i=0;i<nb_fac;i++){
        GLfloat vs[] = {
            0,                         0,                         -lar_cyl/2,     //A
            r_cyl*cos((i+0)*alpha),    r_cyl*sin((i+0)*alpha),    -lar_cyl/2,     //B
            r_cyl*cos((i+1)*alpha),    r_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //C
            r_cyl*cos((i+1)*alpha),    r_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //D
            r_cyl*cos((i+0)*alpha),    r_cyl*sin((i+0)*alpha),    +lar_cyl/2,     //E
            0,                         0,                         +lar_cyl/2,     //F
            r_cyl*cos((i-1)*alpha),    r_cyl*sin((i-1)*alpha),    -lar_cyl/2,     //Y
            r_cyl*cos((i+2)*alpha),    r_cyl*sin((i+2)*alpha),    -lar_cyl/2,     //Z
        };
        int ind_ver[] = { A,B,C, B,C,D, B,D,E, D,E,F };

        GLdouble colors[] = {
            color[0]/255., color[1]/255., color[2]/255.,                //couleur générale
            color[0]/255.-0.1, color[1]/255.-0.1, color[2]/255.-0.1,    //couleur en A et F
            1.0,0.0,0.0
        };
        //                A,B,C, B,C,D, B,D,E, D,E,F
        int ind_col[] = { 1,0,0, 0,0,0, 0,0,0, 0,0,1, };

        QVector3D vAB(vs[B*3+0]-vs[A*3+0], vs[B*3+1]-vs[A*3+1], vs[B*3+2]-vs[A*3+2]);
        QVector3D vBC(vs[C*3+0]-vs[B*3+0], vs[C*3+1]-vs[B*3+1], vs[C*3+2]-vs[B*3+2]);
        QVector3D vDC(vs[C*3+0]-vs[D*3+0], vs[C*3+1]-vs[D*3+1], vs[C*3+2]-vs[D*3+2]);

        QVector3D vBE(vs[E*3+0]-vs[B*3+0], vs[E*3+1]-vs[B*3+1], vs[E*3+2]-vs[B*3+2]);
        QVector3D vBY(vs[Y*3+0]-vs[B*3+0], vs[Y*3+1]-vs[B*3+1], vs[Y*3+2]-vs[B*3+2]);
        QVector3D vDZ(vs[Z*3+0]-vs[D*3+0], vs[Z*3+1]-vs[D*3+1], vs[Z*3+2]-vs[D*3+2]);


        //ABC:AB,AC; ABC:AC,CB; ABC:AB,CB;
        QVector3D nACB = QVector3D::normal(vBC, vAB);
        QVector3D nBCD = QVector3D::normal(vDC, vBC);
        QVector3D nDEF = -nACB;

        QVector3D nBEY = QVector3D::normal(vBE, vBY);
        QVector3D nDCZ = QVector3D::normal(vDC, vDZ);

        QVector3D nBE  = (nBEY+nBCD)/2;
        QVector3D nDC  = (nDCZ+nBCD)/2;

        GLfloat normals[] = {
            nACB.x(), nACB.y(), nACB.z(),   //n1
            nBCD.x(), nBCD.y(), nBCD.z(),   //n2
            nDEF.x(), nDEF.y(), nDEF.z(),   //n3
            nBE.x() , nBE.y() , nBE.z() ,   //n4
            nDC.x() , nDC.y() , nDC.z() ,   //n5
        };
        //                  A,B,C, B,C,D, B,D,E, D,E,F
        //int ind_nor[] = { 0,0,0, 1,1,1, 1,1,1, 2,2,2, };
        //int ind_nor[] = { 0,0,0, 3,4,4, 3,4,3, 2,2,2, };

        int n1=0,n2=1,n3=2,n4=3,n5=4;
        if(!lissage){n4=n2;n5=n2;}
        //                A, B, C,  B, C, D,  B, D, E,  D, E, F
        int ind_nor[] = { n1,n1,n1, n4,n5,n5, n4,n5,n4, n3,n3,n3, };

        for (int i = 0; i < tf*3; ++i) {
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
