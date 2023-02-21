//g++ b.cpp -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lglut -lGLU -lm -o mandelbrot#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <vector> 
#include <tuple>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;    

void fctrec();
void display();
void display_lignes();
void display_points();
void display_fleches();
void init_display();
void exec();
void exec_mem();
void update();
void addpoint();
void addcouleur_point();
void addcouleur_fleche();
void addligne();
void addfleche();

// REGUL: défini régulier, REGUL_MEM: les prochains dépendent des anciens
enum Mode { ALEA, REGUL, REGUL_MEM, CST};
enum Draw { RIEN, UN, TOUT};

vector<pair<float,float>> points;
vector<tuple<int,int,int>> couleurs_points;
vector<vector<pair<float,float>>> lignes;
vector<tuple<float,float,float,float>> fleches;
vector<tuple<int,int,int>> couleurs_fleches;
vector<tuple<float,float,float,float>> exec_vec;

/* PARAMETRE DES CALCULS */
Mode mode0=ALEA;
Mode modec=CST;
Draw ligne=RIEN;
Draw fix=RIEN;
Draw fleche=TOUT;
float bord=10;
float suite=10;
float bordg_x=-bord;
float bordd_x=bord;
float bordb_y=-bord;
float bordh_y=bord;
float suiteg_x=-suite;
float suited_x=suite;
float suiteb_y=-suite;
float suiteh_y=suite;
int umax=500;
int kmax=80000;
int imax=1;
int lignei=0;
int fixi=35;
int flechei=0;
float xc_cst=5;
float yc_cst=-1;
float x0_cst=0;
float y0_cst=0;

/* PARAMETRES D'AFFICHAGE */
int kdisplay=kmax;
int lwin=1500;//1920;
int hwin=950;//1080;
float alphaR=100000;
float alphaG=10000;
float alphaB=500;
float alpha_exp=1/7.;
bool repere=1;
bool couleur_points=0;
bool couleur_fleches=1;
float taille_fleche=0.01*min(bordh_y-bordb_y,bordd_x-bordg_x);

float float_rand(float min,float max) {
    float scale=rand()/(float)RAND_MAX;
    return min+scale*(max-min);
}

pair<float,float> fctrec(float x,float y,float cx,float cy,int it) {
// mandelbrot : z(n+1)=z(n)²+c
// navire en feu : z(n+1)=abs(z)²+c=(abs(Re(z))+i*abs(Im(z)))²+c
// test : z(n+1)=z(n)²+c,z(n+2)=z(n+1)²+i*c,z(n+3)=z(n+2)²-c,z(n+4)=z(n+3)²-i*c
    float zx_new=x*x-y*y+cx;
    float zy_new=2*x*y+cy;
    /*float zx_new=x*x-y*y+cx;
    float zy_new=abs(x*y)+cy;*/
    /*switch (it) {
        case 0:
            zx_new=x*x-y*y+cx;
            zy_new=abs(x*y)+cy;
        case 1:
            zx_new=x*x-y*y+cy;
            zy_new=abs(x*y)-cx;
        case 2:
            zx_new=x*x-y*y-cx;
            zy_new=abs(x*y)-cy;
        case 3:
            zx_new=x*x-y*y-cy;
            zy_new=abs(x*y)+cx;
    }*/







    return make_pair(zx_new,zy_new);
}

int idisplay = 0;
void display() {
    usleep(10000);
    glClear(GL_COLOR_BUFFER_BIT);
    if (fix!=RIEN) display_points();
    if (ligne!=RIEN) display_lignes();
    if (fleche!=RIEN) display_fleches();
    couleurs_points.clear();
    couleurs_fleches.clear();
    lignes.clear();
    points.clear();
    fleches.clear();
    glFlush();
    //glutSwapBuffers();
    //glutPostRedisplay();
}

void glutdisplay() {
    idisplay++; cout << "GLUT idisplay=" << idisplay << endl;
    //display();
}

void execdisplay() {
    //glClear(GL_COLOR_BUFFER_BIT);
    idisplay++; cout << "EXEC idisplay=" << idisplay << endl;
    display();
}

void display_points() {
    int j=0;
    glColor3ub(0,0,0);
    glBegin(GL_POINTS);
    for (auto it=points.begin();it!=points.end();it++,j++) {
        if (couleur_points)
            glColor3ub(get<0>(couleurs_points[j]),get<1>(couleurs_points[j]),get<2>(couleurs_points[j]));
        glVertex2f((*it).first,(*it).second);
        //std::cout << "point : " << (*it).first << ' ' << (*it).second << endl;
    }
    glEnd();
}

void display_lignes() {
    int j=0;
    for (auto it=lignes.begin();it!=lignes.end();it++,j++) {
        glColor3ub(rand()%256,rand()%256,rand()%256) ;
        glBegin(GL_LINE_STRIP);
        int j2=0;
        for (auto it2=(*it).begin();it2!=(*it).end();it2++,j2++) {
            glVertex2f((*it2).first,(*it2).second) ;
            //std::cout << "ligne : " << (*it2).first << ' ' << (*it2).second << endl;
        }
        glEnd();
    }
}

void display_fleches() {
    int j=0;
    glBegin(GL_LINES);
    for (auto it=fleches.begin();it!=fleches.end();it++,j++) {
        if (couleur_fleches)
            glColor3ub(get<0>(couleurs_fleches[j]),get<1>(couleurs_fleches[j]),get<2>(couleurs_fleches[j]));
        float x_0=get<0>(*it);
        float y_0=get<1>(*it);
        float x_1=get<2>(*it);
        float y_1=get<3>(*it);
        float len=sqrt((x_1-x_0)*(x_1-x_0)+(y_1-y_0)*(y_1-y_0));
        float xu=(x_1-x_0)/len;
        float yu=(y_1-y_0)/len;
        float xv=-yu;
        float yv=xu;
        float x_2=x_0+xu*taille_fleche;
        float y_2=y_0+yu*taille_fleche;
        glVertex2f(x_0,y_0);
        glVertex2f(x_2,y_2);
        glVertex2f(x_2,y_2);
        glVertex2f(x_2+taille_fleche*(0.1*xv-0.2*xu),y_2+taille_fleche*(0.1*yv-0.2*yu));
        glVertex2f(x_2,y_2);
        glVertex2f(x_2+taille_fleche*(-0.1*xv-0.2*xu),y_2+taille_fleche*(-0.1*yv-0.2*yu));
    }
    glEnd();
}

void init_display() {
    srand(time(NULL));
    glMatrixMode(GL_MODELVIEW);
    glClearColor(255,255,255,10);
    glClear(GL_COLOR_BUFFER_BIT);
    int minwin=(float)min(hwin,lwin);
    float facteurl=(float)(lwin)/(minwin);
    float facteurh=(float)(hwin)/(minwin);
    bordg_x=facteurl*bordg_x;
    bordd_x=facteurl*bordd_x;
    bordb_y=facteurh*bordb_y;
    bordh_y=facteurh*bordh_y;
    gluOrtho2D(bordg_x,bordd_x,bordb_y,bordh_y);
    glColor3ub(0,0,0);
    if (repere) {
        glBegin(GL_LINES);
            glVertex2f(-1,0);
            glVertex2f(1,0);
            glVertex2f(0,-1);
            glVertex2f(0,1);
        glEnd();
    }
    glLineWidth(1.2);
    glutSwapBuffers();
}

void addpoint(float f1,float f2) {
    points.push_back(make_pair(f1,f2));
}

void addfleche(float f1,float f2,float f3,float f4) {
    fleches.push_back(make_tuple(f1,f2,f3,f4));
}

void addligne(float f1,float f2,bool courant) {
    if (courant) {
        lignes.back().push_back(make_pair(f1,f2));
    }
    else {
        vector<pair<float,float>> nligne;
        nligne.push_back(make_pair(f1,f2));
        lignes.push_back(nligne);
    }
}

void addcouleur_point(int c1,int c2,int c3){
    couleurs_points.push_back(make_tuple(c1,c2,c3));
}

void addcouleur_fleche(int c1,int c2,int c3){
    couleurs_fleches.push_back(make_tuple(c1,c2,c3));
}

void init_param(float& _x0,float& _y0,float& _xc,float& _yc,int k) {
    if (mode0 == ALEA) {
            _x0=float_rand(suiteg_x,suited_x);
            _y0=float_rand(suiteb_y,suiteh_y);
        }
        else if (mode0 == REGUL) {
            int kligne=(int)sqrt(kmax);
            _x0=suiteg_x+(suited_x-suiteg_x)*(k%kligne)/kligne;
            _y0=suiteh_y-(suiteh_y-suiteb_y)*(k/kligne)/kligne;
        }
        else {
            _x0=x0_cst;
            _y0=y0_cst;
        }
        if (modec == ALEA) {
            _xc=float_rand(suiteg_x,suited_x);
            _yc=float_rand(suiteb_y,suiteh_y);
        }
        /*else if (modec == REGUL) {
            xc=suiteg_x+2*framex*(k%kline)/kline;
            yc=suiteb_y-framey+2*framey*(k//kline)/kline;
        }*/
        else {
            _xc=xc_cst;
            _yc=yc_cst;
        }
}

void init_param_mem() {
    if (mode0 == ALEA)
        for (int k=0;k<kmax;k++)
            exec_vec.push_back(make_tuple(float_rand(suiteg_x,suited_x),float_rand(suiteb_y,suiteh_y),0,0));
    else if (mode0==REGUL) {
        int kligne=(int)sqrt(kmax);
        float resx=suiteg_x;
        float resy=suiteh_y;
        for (int kl=0;kl<kligne;kl++) {
            for (int k=0;k<kligne;k++) {
                exec_vec.push_back(make_tuple(resx,resy,0,0));
                resx=resx+(suited_x-suiteg_x)/kligne;
            }
            resx=suiteg_x;
            resy=resy-(suiteh_y-suiteb_y)/kligne;
        }
    }
    else {
        exec_vec.push_back(make_tuple(x0_cst,y0_cst,0,0));
    }
    if (modec == ALEA)
        for (auto it=exec_vec.begin();it!=exec_vec.end();it++)
            *it=make_tuple(get<0>(*it),get<1>(*it),float_rand(suiteg_x,suited_x),float_rand(suiteg_x,suited_x));
    else
        for (auto it=exec_vec.begin();it!=exec_vec.end();it++)
            *it=make_tuple(get<0>(*it),get<1>(*it),xc_cst,yc_cst);
}

void update() {
    yc_cst+=0.2;
}

void exec_mem() {
    int u=0;
    while (u<umax) {
        int i=0;
        init_param_mem();
        while (i<imax) {
            //cout << "i:"<<i<<endl;
            int k=0;
            vector<tuple<float,float,float,float>> tvec;
            while (k<exec_vec.size()) {
                //cout << " k:"<< k;
                float x,y,x0,y0,xc,yc;
                x0=get<0>(exec_vec[k]);
                y0=get<1>(exec_vec[k]);
                xc=get<2>(exec_vec[k]);
                yc=get<3>(exec_vec[k]);
                pair<float,float> z=fctrec(x0,y0,xc,yc,0);
                x=z.first;
                y=z.second;
                if (x<5*bordd_x && x>5*bordg_x && y<5*bordh_y && y>5*bordb_y && !(i>0 && x==x0 && y==y0)) {
                    tvec.push_back(make_tuple(x,y,xc,yc));
                }
                if ((i==flechei && fleche==UN) || fleche==TOUT) {
                    addfleche(x0,y0,x,y);
                    //cout << "fleche: " << x0 << " " << y0 << endl;
                    if (couleur_fleches) {
                        float len=sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
                        float c=255-255.*len/max(bordd_x-bordd_x,bordh_y-bordb_y);
                        addcouleur_fleche(0,c,c);
                    }
                }
                k++;
            }
            exec_vec=tvec;
            i++;
            execdisplay();
        }
        update();
    }
}

void exec() {
    int u=0;
    while (u<umax) {
        int k=0;
        float x0,y0,xc,yc;
        while (k<kmax) {
            init_param(x0,y0,xc,yc,k);
            float x=x0;
            float y=y0;
            float tx=x;
            float ty=y;
            int i=0;
            double d=0;
            bool bt=0;
            while (i<imax && x<5*bordd_x && x>5*bordg_x && y<5*bordh_y && y>5*bordb_y && !(i>0 && tx==x && ty==y)) {
                tx=x;
                ty=y;
                pair<float,float> z=fctrec(tx,ty,xc,yc,0);
                x=z.first;
                y=z.second;
                if (fix!=RIEN && i>0 && fixi<=i && couleur_points) {
                    d=d+((x-tx)*(x-tx)+(y-ty)*(y-ty));
                }
                if ((i==fixi && fix==UN) || fix == TOUT) {
                    if (x*x+y*y<2) {
                        double res=pow(d,alpha_exp)/fixi;
                        if (couleur_points)
                            addcouleur_point((int)(255./(alphaR*res+1)),(int)(255./(alphaG*res+1)),(int)(255./(alphaB*res+1)));
                        addpoint(x0,y0);
                    }
                }
                if ((i==lignei && ligne==UN) || ligne == TOUT) {
                    if (bt==0) {
                        addligne(tx,ty,0);
                        bt=1;
                    }
                    addligne(x,y,1);
                }
                else bt=0;
                if ((i==flechei && fleche==UN) || fleche==TOUT) {
                    addfleche(tx,ty,x,y);
                    if (couleur_fleches) {
                        float len=sqrt((x-tx)*(x-tx)+(y-ty)*(y-ty));
                        float c=255-255.*len/max(bordd_x-bordd_x,bordh_y-bordb_y);
                        addcouleur_fleche(0,c,c);
                    }
                }
                i++;
            }
            k++;
            if (k%kdisplay==0) 
                execdisplay();
        }
        u++;
        update();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    //glutSetOption(GLUT_MULTISAMPLE, 4);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(lwin,hwin);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Mandelbrot");
    glutDisplayFunc(glutdisplay);
    //glutFullScreen();
    init_display();
    usleep(1000000);
    exec();
    usleep(2000000);
    glutMainLoop();
    return 0;
}