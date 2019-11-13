#include "demicylindre.h"

DemiCylindre::DemiCylindre(GLfloat lar_cyl, GLfloat r_cyl, GLfloat ep_cyl, GLint nb_fac,int r, int v, int b, bool lissage)
{
    this->lar_cyl = lar_cyl;    // profondeur du cylindre
    this->ep_cyl = ep_cyl;      // epaisseur du cylindre
    this->r1_cyl = r_cyl;       // rayon exterieur
    this->r2_cyl = r_cyl-ep_cyl;// rayon interieur
    this->nb_fac = nb_fac;      // nombre de facettes
    this->nb_triangles=(tf*nb_fac+tb);
    this->lissage = lissage;
    this->red=0;
    this->setColor(r,v,b);
}

void DemiCylindre::setColor(int r, int v, int b){
    color[0] = r; // rouge
    color[1] = v; // vert
    color[2] = b; // bleu
}

void DemiCylindre::construire_demiCylindre(QVector<GLfloat> * vertData){
    GLfloat alpha = M_PI/nb_fac;
    int A=0,B=1,C=2,D=3,E=4,F=5,G=6,H=7,I=8,sF=9,sG=10,Y=11,Z=12;
    QVector3D last;
    GLfloat space=0.001;
    for (int i=0;i<nb_fac;i++){
        GLfloat vs[] = {
            0,                          0,                          -lar_cyl/2,     //A
            r1_cyl*cos((i+0)*alpha),    r1_cyl*sin((i+0)*alpha),    -lar_cyl/2,     //B
            r1_cyl*cos((i+1)*alpha),    r1_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //C
            r1_cyl*cos((i+1)*alpha),    r1_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //D
            r1_cyl*cos((i+0)*alpha),    r1_cyl*sin((i+0)*alpha),    +lar_cyl/2,     //E
            r2_cyl*cos((i+0)*alpha),    r2_cyl*sin((i+0)*alpha),    -lar_cyl/2,     //F
            r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    -lar_cyl/2,     //G
            r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    +lar_cyl/2,     //H
            r2_cyl*cos((i+0)*alpha),    r2_cyl*sin((i+0)*alpha),    +lar_cyl/2,     //I
            r2_cyl*cos((i+0)*alpha),    r2_cyl*sin((i+0)*alpha),    -lar_cyl/2+space,     //F'
            r2_cyl*cos((i+1)*alpha),    r2_cyl*sin((i+1)*alpha),    -lar_cyl/2+space,     //G'
            r1_cyl*cos((i-1)*alpha),    r1_cyl*sin((i-1)*alpha),    -lar_cyl/2,     //Y
            r1_cyl*cos((i+2)*alpha),    r1_cyl*sin((i+2)*alpha),    -lar_cyl/2,     //Z
        };
        int ind_ver[] = { A,B,C, B,C,D, B,D,E, F,G,H, F,H,I, D,H,I, D,E,I, A,sF,sG};

        GLdouble colors[] = {
            color[0]/255., color[1]/255., color[2]/255.,                //couleur générale
            color[0]/255.-0.1, color[1]/255.-0.1, color[2]/255.-0.1,    //couleur en A
            color[0]/255., color[1]/255.*(1-red), color[2]/255.*(1-red),      //couleur en AFG
        };
        //                A,B,C, B,C,D, B,D,E, F,G,H, F,H,I, D,H,I, D,E,I, A,sF,sG
        int ind_col[] = { 1,0,0, 0,0,0, 0,0,0, 2,2,0, 2,0,0, 0,0,0, 0,0,0, 2,2,2};

        QVector3D vAB(vs[B*3+0]-vs[A*3+0], vs[B*3+1]-vs[A*3+1], vs[B*3+2]-vs[A*3+2]);
        QVector3D vBC(vs[C*3+0]-vs[B*3+0], vs[C*3+1]-vs[B*3+1], vs[C*3+2]-vs[B*3+2]);
        QVector3D vDC(vs[C*3+0]-vs[D*3+0], vs[C*3+1]-vs[D*3+1], vs[C*3+2]-vs[D*3+2]);

        QVector3D vBE(vs[E*3+0]-vs[B*3+0], vs[E*3+1]-vs[B*3+1], vs[E*3+2]-vs[B*3+2]);
        QVector3D vBY(vs[Y*3+0]-vs[B*3+0], vs[Y*3+1]-vs[B*3+1], vs[Y*3+2]-vs[B*3+2]);
        QVector3D vDZ(vs[Z*3+0]-vs[D*3+0], vs[Z*3+1]-vs[D*3+1], vs[Z*3+2]-vs[D*3+2]);

        //ABC:AB,AC; ABC:AC,CB; ABC:AB,CB;
        QVector3D nACB = QVector3D::normal(vBC, vAB);
        QVector3D nBCD = QVector3D::normal(vDC, vBC);
        QVector3D nFHG = -nBCD;
        QVector3D nDIH = -nACB;

        QVector3D nBEY = QVector3D::normal(vBE, vBY);
        QVector3D nDCZ = QVector3D::normal(vDC, vDZ);

        QVector3D nBE  = (nBEY+nBCD)/2;
        QVector3D nDC  = (nDCZ+nBCD)/2;
        QVector3D nFI  = -nBE;
        QVector3D nGH  = -nDC;

        GLfloat normals[] = {
            nACB.x(), nACB.y(), nACB.z(),   //n1
            nBCD.x(), nBCD.y(), nBCD.z(),   //n2
            nFHG.x(), nFHG.y(), nFHG.z(),   //n3
            nDIH.x(), nDIH.y(), nDIH.z(),   //n4
            nBE.x() , nBE.y() , nBE.z() ,   //n5
            nDC.x() , nDC.y() , nDC.z() ,   //n6
            nFI.x() , nFI.y() , nFI.z() ,   //n7
            nGH.x() , nGH.y() , nGH.z() ,   //n8
        };
        //                  A,B,C, B,C,D, B,D,E, F,G,H, F,H,I, D,H,I, D,E,I, A,sF,sG
        //int ind_nor[] = { 0,0,0, 1,1,1, 1,1,1, 2,2,2, 2,2,2, 3,3,3, 3,3,3, 3,3,3,};
        //int ind_nor[] = { 0,0,0, 4,1,1, 4,1,4, 5,2,2, 5,2,5, 3,3,3, 3,3,3, 3,3,3,};

        int n1=0,n2=1,n3=2,n4=3,n5=4,n6=5,n7=6,n8=7;
        if(!lissage){n5=n2;n6=n2;n7=n3;n8=n3;}
        //                A, B, C,  B, C, D,  B, D, E,  F, G, H,  F, H, I,  D, H, I,  D, E, I,  A, sF,sG
        int ind_nor[] = { n1,n1,n1, n5,n6,n6, n5,n6,n5, n7,n8,n8, n7,n8,n7, n4,n4,n4, n4,n4,n4, n4,n4,n4,};

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
    GLfloat vs[] = {
        r1_cyl,     0,    -lar_cyl/2,     //A
        r2_cyl,     0,    -lar_cyl/2,     //B
        r1_cyl,     0,    +lar_cyl/2,     //C
        r2_cyl,     0,    +lar_cyl/2,     //D
        -r1_cyl,    0,    -lar_cyl/2,     //E
        -r2_cyl,    0,    -lar_cyl/2,     //F
        -r1_cyl,    0,    +lar_cyl/2,     //G
        -r2_cyl,    0,    +lar_cyl/2,     //H
    };

    int ind_ver[] = { A,B,C, B,C,D, E,F,G, F,G,H, };

    GLdouble colors[] = {
        color[0]/255., color[1]/255., color[2]/255.,    //couleur générale
    };
    //                A,B,C, B,C,D, E,F,G, F,G,H
    int ind_col[] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0, };

    QVector3D vAB(vs[B*3+0]-vs[A*3+0], vs[B*3+1]-vs[A*3+1], vs[B*3+2]-vs[A*3+2]);
    QVector3D vAC(vs[C*3+0]-vs[A*3+0], vs[C*3+1]-vs[A*3+1], vs[C*3+2]-vs[A*3+2]);

    QVector3D nACB = QVector3D::normal(vAC, vAB);

    GLfloat normals[] = {
        nACB.x(), nACB.y(), nACB.z(),
    };
    //                A,B,C, B,C,D, E,F,G, F,G,H
    int ind_nor[] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0, };


    for (int i = 0; i < tb*3; ++i) {
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
