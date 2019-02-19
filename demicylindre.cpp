#include "demicylindre.h"

DemiCylindre::DemiCylindre(GLfloat lar_cyl, GLfloat ep_cyl, GLfloat r_cyl, GLint nb_fac,int r, int v, int b)
{
    this->lar_cyl = lar_cyl;    // profondeur du cylindre
    this->ep_cyl = ep_cyl;      // epaisseur du cylindre
    this->r1_cyl = r_cyl;       // rayon exterieur
    this->r2_cyl = r_cyl-ep_cyl;// rayon interieur
    this->nb_fac = nb_fac;      // nombre de facettes
    this->nb_triangles=(tf*nb_fac+tb);
    this->setColor(r,v,b);
    qDebug() << "youou" <<nb_triangles;
}

void DemiCylindre::setColor(int r, int v, int b){
    color[0] = r; // rouge
    color[1] = v; // vert
    color[2] = b; // bleu
}

void DemiCylindre::construire_demiCylindre(QVector<GLfloat> * vertData){
    qDebug() << "bonjour";
    GLfloat alpha = M_PI/nb_fac;
    int A=0,B=1,C=2,D=3,E=4,F=5,G=6,H=7,I=8;
    QVector3D last;
    for (int i=0;i<nb_fac;i++){
        GLfloat vs[] = {
            0,                          0,                          -lar_cyl/2,     //A
            r1_cyl*cos((i+0)*alpha),    r1_cyl*sin((i+0)*alpha),    -lar_cyl/2,     //B
            r1_cyl*cos((i+1)*alpha),    r1_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //C
            r1_cyl*cos((i+1)*alpha),    r1_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //D
            r1_cyl*cos((i+0)*alpha),    r1_cyl*sin((i+0)*alpha),    +lar_cyl/2,     //E*
            r2_cyl*cos((i+0)*alpha),    r2_cyl*sin((i+0)*alpha),    -lar_cyl/2,     //F
            r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //G
            r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //H
            r2_cyl*cos((i+0)*alpha),    r2_cyl*sin((i+0)*alpha),    +lar_cyl/2,     //I*
        };
        int ind_ver[] = { A,B,C, B,C,D, B,D,E, F,G,H, F,H,I, D,H,I, D,E,I };

        GLdouble colors[] = {
            color[0]/255., color[1]/255., color[2]/255.,    //couleur générale
            color[0]/255.+0.5, color[1]/255.+0.5, color[2]/255.+0.5,    //couleur en A
        };
        //                A,B,C, B,C,D, B,D,E, F,G,H, F,H,I, D,H,I, D,E,I
        int ind_col[] = { 1,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0 };

        QVector3D vAB(vs[B*3+0]-vs[A*3+0], vs[B*3+1]-vs[A*3+1], vs[B*3+2]-vs[A*3+2]);
        QVector3D vBC(vs[C*3+0]-vs[B*3+0], vs[C*3+1]-vs[B*3+1], vs[C*3+2]-vs[B*3+2]);
        QVector3D vDC(vs[C*3+0]-vs[D*3+0], vs[C*3+1]-vs[D*3+1], vs[C*3+2]-vs[D*3+2]);
        QVector3D vCE(vs[E*3+0]-vs[C*3+0], vs[E*3+1]-vs[C*3+1], vs[E*3+2]-vs[C*3+2]);
        QVector3D vDE(vs[E*3+0]-vs[D*3+0], vs[E*3+1]-vs[D*3+1], vs[E*3+2]-vs[D*3+2]);

        //ABC:AB,AC; ABC:AC,CB; ABC:AB,CB;
        QVector3D nACB = QVector3D::normal(vBC, vAB);
        QVector3D nBCD = QVector3D::normal(vDC, vBC);
        QVector3D nFHG = -nBCD;
        QVector3D nDIH = -nACB;

        if(i==0){
            last=nBCD;
        }

        QVector3D nBE  = (last+nBCD)/2;
        QVector3D nFI  = -nBE;

        last=nBCD;


        GLfloat normals[] = {
            nACB.x(), nACB.y(), nACB.z(),
            nBCD.x(), nBCD.y(), nBCD.z(),
            nFHG.x(), nFHG.y(), nFHG.z(),
            nDIH.x(), nDIH.y(), nDIH.z(),
            nBE.x() , nBE.y() , nBE.z() ,
            nFI.x() , nFI.y() , nFI.z() ,
        };
        //                A,B,C, B,C,D, B,D,E, F,G,H, F,H,I, D,H,I, D,E,I

      //int ind_nor[] = { 0,0,0, 1,1,1, 1,1,1, 2,2,2, 2,2,2, 3,3,3, 3,3,3,};
        int ind_nor[] = { 0,0,0, 4,1,1, 4,1,4, 5,2,2, 5,2,5, 3,3,3, 3,3,3,};

        for (int i = 0; i < 7*3; ++i) {
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
