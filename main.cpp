/*GRUPO: Daniel, Marcos, Ralf */
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <GL/glut.h>
#include "imageloader.h"

GLfloat fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
int x_ini,y_ini,bot;
// Total de pontos intermediários na curva
int prec = 300;
int flag=1;

// Pontos de controle
GLfloat pontos[4][4][3] = {
	{{-5.0, -1.7, -15.0}, {0.3, -1.7, -15.0}, {0.7, -1.7, -15.0}, {5.0, -1.7, -15.0}},
	{{-8.0, -1.7, -7.0}, {-0.7, -0.5, -9.0}, {0.7, -0.5, -9.0}, {8.0, -1.7, -7.0}},
	{{-8.0, -1.7, -3.0}, {-0.7, -0.5, -5.0}, {0.7, -0.5, -5.0}, {8.0, -1.7, -3.0}},
	{{-5.0, -1.7, 0.0}, {0.3, -1.7, 0.0}, {0.7, -1.7, 0.0}, {5.0, -1.7, 0.0}}
};

GLUquadric *quad1, *quad2;
GLuint _textureTijolo, _texturePorta, _textureTelhado, _textureJanela, _textureCeu, _textureTerreno, _textureEstrada, _texturaRoad, _textureSol, _textureMontanha;

GLboolean proj = true; //T = projeção perspectiva. F = projeção ortográfica
GLfloat angulo;
GLfloat angle, angle2;
GLboolean iniciar = false;

//especifica os limites mínimos para as duas projeções
GLfloat lim_min_pers = 0.5, lim_min_orto = 0.5;

//especifica os limites máximos para as duas projeções
GLfloat lim_max_pers = 400, lim_max_orto = 400;

GLfloat largura, altura;

// Luz selecionada
int luz = 0;

// Posição de cada luz
GLfloat posLuz[1][4] = {
	{ 0, -30, -180, 1 }
	//{   0, 10, -5, 1 }
};

// Direção de cada luz
GLfloat dirLuz[1][3] = {
	{ 0,-1,0 }
};

// Cor difusa de cada luz
GLfloat luzDifusa[1][4] = {
	{ 1,0.85,0,0},
};

// Cor especular de cada luz
GLfloat luzEspecular[1][4] = {
	{ 0,-1,0,0 }
};

// Define um vértice
struct VERT{
	float x,y,z;	// posição no espaço
};

// Define uma face
struct FACE{
	int total;	// total de vértices
	int ind[4];	// índices para o vetor de vértices
};

// Define um objeto 3D
struct OBJ{
	VERT *vertices;		// aponta para os vértices
	FACE *faces;		// aponta para as faces
	int total_faces;	// total de faces no objeto
};

// Definição dos vértices
VERT vertices[] = {
    {48,2,0.5}, //0
    {-48,2,0.5}, //1
    {-48,-2,0.5}, //2
    {48,-2,0.5}, //3
    {48,-2,-0.5}, //4
    {48,2,-0.5}, //5
    {-48,2,-0.5}, //6
    {-48,-2,-0.5}  //7
};

// Definição das faces
FACE faces[] = {
	{ 4, { 0,1,2,3 }},
	{ 4, { 0,3,4,5 }},
	{ 4, { 0,5,6,1 }},
	{ 4, { 1,6,7,2 }},
	{ 4, { 7,4,3,2 }},
	{ 4, { 4,7,6,5 }}
};

// Finalmente, define o objeto pirâmide
OBJ muro = {
	vertices, faces, 5
};


// Definição dos vértices
VERT vertices2[] = {
    {23,2,0.5}, //0
    {-23,2,0.5}, //1
    {-23,-2,0.5}, //2
    {23,-2,0.5}, //3
    {23,-2,-0.5}, //4
    {23,2,-0.5}, //5
    {-23,2,-0.5}, //6
    {-23,-2,-0.5}  //7
};

// Definição das faces
FACE faces2[] = {
	{ 4, { 0,1,2,3 }},
	{ 4, { 0,3,4,5 }},
	{ 4, { 0,5,6,1 }},
	{ 4, { 1,6,7,2 }},
	{ 4, { 7,4,3,2 }},
	{ 4, { 4,7,6,5 }}
};
OBJ muro2 = {
	vertices2, faces2, 5
};
// Função responsável pela especificação dos parâmetros de iluminaç¿o
void DefineIluminacao (void)
{
	GLfloat luzAmbiente[4]={0.1,0.1,0.1,1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	GLint especMaterial = 90;

	// Define a refletância do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros das fontes de luz
	for(int i=0;i<1;++i)
	{
		glLightfv(GL_LIGHT0+i, GL_AMBIENT, luzAmbiente);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, luzDifusa[i] );
		glLightfv(GL_LIGHT0+i, GL_SPECULAR, luzEspecular[i] );
		glLightfv(GL_LIGHT0+i, GL_POSITION, posLuz[i] );
		glLightfv(GL_LIGHT0+i,GL_SPOT_DIRECTION,dirLuz[i]);
		glLightf(GL_LIGHT0+i,GL_SPOT_CUTOFF,90.0);
		glLightf(GL_LIGHT0+i,GL_SPOT_EXPONENT,0.0);
	}
}

// Desenha um muro
void DesenhaMuro(OBJ *objeto)
{
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
	for(int f=0; f < objeto->total_faces; f++)
	{

	    if(f==0){
            glBindTexture(GL_TEXTURE_2D, _textureEstrada);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
	    if(f==1)
        {
            glBindTexture(GL_TEXTURE_2D, _textureEstrada);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
	    if(f==2)
        {
            glBindTexture(GL_TEXTURE_2D, _textureEstrada);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
	    if(f==3)
        {
            glBindTexture(GL_TEXTURE_2D, _textureEstrada);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
	    if(f==4)
        {
            glBindTexture(GL_TEXTURE_2D, _textureEstrada);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
	    if(f==5)
        {
            glBindTexture(GL_TEXTURE_2D, _textureEstrada);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
		glBegin(GL_POLYGON);
		// Percorre todos os vértices da face
		for(int v=0; v < objeto->faces[f].total; v++)
			glVertex3f(objeto->vertices[objeto->faces[f].ind[v]].x,
				       objeto->vertices[objeto->faces[f].ind[v]].y,
				       objeto->vertices[objeto->faces[f].ind[v]].z);
		glEnd();
	}
	glPopMatrix();
}



void DesenhaTronco(float height,float Base){
    GLUquadricObj *qobj;
    qobj = gluNewQuadric();

    glPushMatrix();
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        gluCylinder(qobj, Base, Base - (0.2 * Base), height, 20, 20);
    glPopMatrix();
}

void DesenhaArvore(float height,float Base){

    glPushMatrix();
        DesenhaTronco(height, Base);
        glTranslatef(0.0f, height,0.0f);
        height -=height*0.2f;
        Base -=Base*0.5f;
        for(int a=0; a<3; a++)  //Gerar galhos
        {
            angle2 = 20+((rand()%50));
            if(angle2 >48)
            angle2 = -(20+((rand()%50)));
            if (height > 1)
            {
                glPushMatrix();
                if (flag)
                    glRotatef(angle2, 1.0f, 0.0f, 0.0f);
                else
                    glRotatef(angle2, 0.0f, 1.0f, 1.0f);
                flag = !flag;
                DesenhaArvore(height, Base); //chamada recursiva
                glPopMatrix();
            }
            else
            {
                glColor3f(1.0f, 1.0f / a, 0.0f);
                glutSolidSphere(0.25f, 10, 10);// para frutas.
            }
        }
        //Glut.glutSwapBuffers();
        glPopMatrix();
}

void DesenhaEstrada(){
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureEstrada);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.6);  glVertex3f(-0.7,0.25,-10);
            glTexCoord3f(1.0,1.0,0.6);  glVertex3f(0.7,0.25,-10);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(0.7,0.25,10);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-0.7,0.25,10);
        glEnd();
    glPopMatrix();
}

void DesenhaRodovia(){
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _texturaRoad);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(-140,0.25,-10);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(140,0.25,-10);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(140,0.25,10);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-140,0.25,10);
        glEnd();
    glPopMatrix();
}

void DesenhaCasa(){
// Lado da Frente da casa
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureTijolo);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glRotatef(angulo, 0.0, 1.0, 0.0);
        glBegin(GL_QUADS);  // Parede
            glTexCoord3f(0.0,2.0,0.1);  glVertex3f(-4,1,1);
            glTexCoord3f(4.0,2.0,0.1);  glVertex3f(4,1,1);
            glTexCoord3f(4.0,0.0,0.1);  glVertex3f(4,-1.5,1);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-4,-1.5,1);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, _textureTelhado);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Telhado
            glTexCoord3f(0.0,2.0,0); glVertex3f(-4.4,1.5,-0.5);
            glTexCoord3f(4.0,2.0,0);glVertex3f(4.4,1.5,-0.5);
            glTexCoord3f(4.0,0.0,1.25); glVertex3f(4.4,0.6,2.2);
            glTexCoord3f(0.0,0.0,1.25); glVertex3f(-4.4,0.6,2.2);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, _texturePorta);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Porta
            glTexCoord3f(0.0,1.0,1.0001); glVertex3f(-0.3,0.0,1.0001);
            glTexCoord3f(1.0,1.0,1.0001); glVertex3f(0.4,0.0,1.0001);
            glTexCoord3f(1.0,0.0,1.0001); glVertex3f(0.4,-1.5,1.0001);
            glTexCoord3f(0.0,0.0,1.0001); glVertex3f(-0.3,-1.5,1.0001);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, _textureJanela);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Janela Esquerda
            glTexCoord3f(0.0,1.0,1.0001); glVertex3f(-1.5,0.3,1.0001);
            glTexCoord3f(1.0,1.0,1.0001); glVertex3f(-0.75,0.3,1.0001);
            glTexCoord3f(1.0,0.0,1.0001); glVertex3f(-0.75,-0.2,1.0001);
            glTexCoord3f(0.0,0.0,1.0001); glVertex3f(-1.5,-0.2,1.0001);
        glEnd();

        glBegin(GL_QUADS);  // Janela Direita
            glTexCoord3f(0.0,1.0,1.0001); glVertex3f(1.5,0.3,1.0001);
            glTexCoord3f(1.0,1.0,1.0001); glVertex3f(0.75,0.3,1.0001);
            glTexCoord3f(1.0,0.0,1.0001); glVertex3f(0.75,-0.2,1.0001);
            glTexCoord3f(0.0,0.0,1.0001); glVertex3f(1.5,-0.2,1.0001);
        glEnd();
    glPopMatrix();

    // Lado de tras da casa
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _textureTijolo);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Parede
            glTexCoord3f(0.0,2.0,-1);  glVertex3f(-4,1,-2);
            glTexCoord3f(4.0,2.0,-1);  glVertex3f(4,1,-2);
            glTexCoord3f(4.0,0.0,-1);  glVertex3f(4,-1.5,-2);
            glTexCoord3f(0.0,0.0,-1);  glVertex3f(-4,-1.5,-2);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, _textureTelhado);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Telhado
            glTexCoord3f(0.0,2.0,0); glVertex3f(-4.4,1.5,-0.5);
            glTexCoord3f(4.0,2.0,0);glVertex3f(4.4,1.5,-0.5);
            glTexCoord3f(4.0,0.0,-1.25); glVertex3f(4.4,0.7,-3.0);
            glTexCoord3f(0.0,0.0,-1.25); glVertex3f(-4.4,0.7,-3.0);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, _textureJanela);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Janela Esquerda
            glTexCoord3f(0.0,1.0,-1.0001); glVertex3f(-1.5,0.2,-2.0001);
            glTexCoord3f(1.0,1.0,-1.0001); glVertex3f(-0.75,0.2,-2.0001);
            glTexCoord3f(1.0,0.0,-1.0001); glVertex3f(-0.75,-0.3,-2.0001);
            glTexCoord3f(0.0,0.0,-1.0001); glVertex3f(-1.5,-0.3,-2.0001);
        glEnd();

        glBegin(GL_QUADS);  // Janela Direita
            glTexCoord3f(0.0,1.0,1.0001); glVertex3f(1.5,0.2,-2.0001);
            glTexCoord3f(1.0,1.0,1.0001); glVertex3f(0.75,0.2,-2.0001);
            glTexCoord3f(1.0,0.0,1.0001); glVertex3f(0.75,-0.3,-2.0001);
            glTexCoord3f(0.0,0.0,1.0001); glVertex3f(1.5,-0.3,-2.0001);
        glEnd();
    glPopMatrix();

    // Lado direito da casa
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureTijolo);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);  // Parede
            glTexCoord3f(0.0,2.0,1); glVertex3f(4,1,1);
            glTexCoord3f(2.0,2.0,-1); glVertex3f(4,1,-2);
            glTexCoord3f(2.0,0.0,-1); glVertex3f(4,-1.5,-2);
            glTexCoord3f(0.0,0.0,1); glVertex3f(4,-1.5,1);
        glEnd();

        glBegin(GL_TRIANGLES);  // Parede de cima
            glTexCoord3f(0.0,1.0,0); glVertex3f(4,1.5,-0.5);
            glTexCoord3f(1.0,0.0,1); glVertex3f(4,1,1);
            glTexCoord3f(-1.0,0.0,-1); glVertex3f(4,1,-2);
        glEnd();
    glPopMatrix();

    // Lado Esquerdo da casa
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureTijolo);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBegin(GL_QUADS);  // Parede
            glTexCoord3f(0.0,2.0,1);    glVertex3f(-4,1,1);
            glTexCoord3f(2.0,2.0,-1);    glVertex3f(-4,1,-2);
            glTexCoord3f(2.0,0.0,-1);    glVertex3f(-4,-1.5,-2);
            glTexCoord3f(0.0,0.0,1);    glVertex3f(-4,-1.5,1);
        glEnd();

        glBegin(GL_TRIANGLES);  // Parede de cima
            glTexCoord3f(0.0,1.0,0);    glVertex3f(-4,1.5,-0.5);
            glTexCoord3f(1.0,0.0,1);    glVertex3f(-4,1,1);
            glTexCoord3f(-1.0,0.0,-1);  glVertex3f(-4,1,-2);
        glEnd();
    glPopMatrix();

}

void PosicionaObservador(void)
{
	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();
	// Posiciona e orienta o observador
	glTranslatef(-obsX,-obsY,-obsZ);
	glRotatef(rotX,1,0,0);
	glRotatef(rotY,0,1,0);
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao()
{
	// Especifica sistema de coordenadas de projeção
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();

	// Especifica a projeção perspectiva(angulo,aspecto,zMin,zMax)
	gluPerspective(angle,fAspect,0.1,1200);

	PosicionaObservador();
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    const float ar = (GLfloat) w / (GLfloat) h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Calcula a correção de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;
    largura = w;
    altura = h;

	EspecificaParametrosVisualizacao();
}

void Desenha(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    // Chama a função que especifica os parâmetros de iluminação
	DefineIluminacao();

	// Desabilita a iluminação para desenhar as esferas
	glDisable(GL_LIGHTING);
		// Desenha "esferas" nas posições das fontes de luz
		glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, _textureSol);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTranslatef(posLuz[0][0],posLuz[0][1],posLuz[0][2]);
            glColor3f(luzDifusa[0][0],luzDifusa[0][1],luzDifusa[0][2]);
            gluQuadricTexture(quad1,1);
            gluSphere(quad1,4,30,30);
		glPopMatrix();
	// Habilita iluminação novamente
	glEnable(GL_LIGHTING);

	// Altera a cor do desenho para branco
	glColor3f(1,1,1);

    /// Ceu
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureCeu);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /// Ceu frente
        glTranslatef(0,0,-6);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(-140,140,-140);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(140,140,-140);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(140,-10,-140);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-140,-10,-140);
        glEnd();
        /// Ceu cima
        glTranslatef(0,0,-6);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(-140,140,-140);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(140,140,-140);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(140,140,140);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-140,140,140);
        glEnd();
        /// Ceu dir.
        glTranslatef(0,0,-6);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(140,140,-140);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(140,140,160);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(140,-10,160);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(140,-10,-140);
        glEnd();
        /// Ceu esq.
        glTranslatef(0,0,-6);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(-140,140,-140);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(-140,140,160);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(-140,-10,160);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-140,-10,-140);
        glEnd();
        /// Ceu tras.
        glTranslatef(0,0,-6);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(-140,140,140);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(140,140,140);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(140,-10,140);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-140,-10,140);
        glEnd();
    glPopMatrix();

    /// Terreno
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureTerreno);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTranslatef(0,0,-6);
        //glRotatef(angulo, 0.0, 1.0, 0.0);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,70.0,1);  glVertex3f(-140,-1.5,140);
            glTexCoord3f(0.0,0.0,-1);  glVertex3f(-140,-1.5,-140);
            glTexCoord3f(70.0,0.0,-1);  glVertex3f(140,-1.5,-140);
            glTexCoord3f(70.0,70.0,1);  glVertex3f(140,-1.5,140);
        glEnd();
    glPopMatrix();
///INICIO
///Conjuntos de CASAS da esquerda
    ///Desenha Muro Esquerda
    glPushMatrix();
        glTranslatef(-100, -1, 19.5);
        DesenhaMuro(&muro);
    glPopMatrix();
    /// Desenha Casa do centro
    glPushMatrix();
        glTranslatef(-50,0,0);
        DesenhaCasa();
    glPopMatrix();
    /// Desenha Estrada
    glPushMatrix();
        glTranslatef(-50,-1.7, 10);
        DesenhaEstrada();
    glPopMatrix();
    /// Desenha Casa da Esquerda
    glPushMatrix();
        glTranslatef(-60,0,10);
        glRotatef(90,0,50,0);
        DesenhaCasa();
    glPopMatrix();
    /// Desenha Estrada
    glPushMatrix();
        glTranslatef(-50,-1.7, 10);
        glRotatef(90,0,50,0);
        DesenhaEstrada();
    glPopMatrix();
    /// Desenha Casa da Direita
    glPushMatrix();
        glTranslatef(-40,0,10);
        glRotatef(90,0,-50,0);
        DesenhaCasa();
    glPopMatrix();
    ///Desenha Muro Direita
    glPushMatrix();
        glTranslatef(-25, -1, 19.5);
        DesenhaMuro(&muro2);
    glPopMatrix();

///Conjunto de Casas do Centro
    /// Desenha Casa do centro
    glPushMatrix();
        glTranslatef(0,0,0);
        DesenhaCasa();
    glPopMatrix();
    /// Desenha Estrada
    glPushMatrix();
        glTranslatef(0,-1.7, 10);
        DesenhaEstrada();
    glPopMatrix();
    /// Desenha Casa da Esquerda
    glPushMatrix();
        glTranslatef(-10,0,10);
        glRotatef(90,0,50,0);
        DesenhaCasa();
    glPopMatrix();
    /// Desenha Estrada
    glPushMatrix();
        glTranslatef(0,-1.7, 10);
        glRotatef(90,0,50,0);
        DesenhaEstrada();
    glPopMatrix();
    /// Desenha Casa da Direita
    glPushMatrix();
        glTranslatef(10,0,10);
        glRotatef(90,0,-50,0);
        DesenhaCasa();
    glPopMatrix();

    ///Desenha Muro Direita
    glPushMatrix();
        glTranslatef(25, -1, 19.5);
        DesenhaMuro(&muro2);
    glPopMatrix();

///Conjuntos de CASAS da direita
    /// Desenha Casa do centro
    glPushMatrix();
        glTranslatef(50,0,0);
        DesenhaCasa();
    glPopMatrix();
    /// Desenha Estrada
    glPushMatrix();
        glTranslatef(50,-1.7, 10);
        DesenhaEstrada();
    glPopMatrix();
    /// Desenha Casa da Esquerda
    glPushMatrix();
        glTranslatef(60,0,10);
        glRotatef(90,0,-50,0);
        DesenhaCasa();
    glPopMatrix();
    /// Desenha Estrada
    glPushMatrix();
        glTranslatef(50,-1.7, 10);
        glRotatef(90,0,50,0);
        DesenhaEstrada();
    glPopMatrix();
    /// Desenha Casa da Direita
    glPushMatrix();
        glTranslatef(40,0,10);
        glRotatef(90,0,50,0);
        DesenhaCasa();
    glPopMatrix();

    ///Desenha Muro Esquerda
    glPushMatrix();
        glTranslatef(100, -1, 19.5);
        DesenhaMuro(&muro);
    glPopMatrix();
///FIM

    /// Rodovia
    glPushMatrix();
        glTranslatef(0,-1.7, 30);
        DesenhaRodovia();
    glPopMatrix();

    ///Desenha 2 Arvores

    glPushMatrix();
        /// Habilita a definição da cor do material a partir da cor corrente
        srand(GLUT_ELAPSED_TIME);
        glTranslatef(-25.0, -1.4, 5.0);
        DesenhaArvore(4.0f,0.3f);
    glPopMatrix();

    glPushMatrix();
        /// Habilita a definição da cor do material a partir da cor corrente
        srand(GLUT_ELAPSED_TIME);
        glTranslatef(25.0, -1.4, 5.0);
        DesenhaArvore(4.0f,0.3f);
    glPopMatrix();



    ///Montanha Frente
    glPushMatrix();
          glBindTexture(GL_TEXTURE_2D, _textureMontanha);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTranslatef(0,0.0,78.0);
          glScalef(140.0,20.0,22.0);
          gluQuadricTexture(quad1,1);
          gluSphere(quad1,1.2,12,12);
    glPopMatrix();

    ///Montanha Esquerda
    glPushMatrix();
          glBindTexture(GL_TEXTURE_2D, _textureMontanha);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTranslatef(-120,5.0,-90.0);
          glScalef(20.0,15.0,90.0);
          gluQuadricTexture(quad1,1);
          gluSphere(quad1,1.2,12,12);

    glPopMatrix();

    ///Montanha Direita
    glPushMatrix();
          glBindTexture(GL_TEXTURE_2D, _textureMontanha);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTranslatef(120,5.0,-90.0);
          glScalef(30.0,15.0,90.0);
          gluQuadricTexture(quad1,1);
          gluSphere(quad1,1.2,12,12);
    glPopMatrix();

    ///Montanha traseira
    glPushMatrix();
          glBindTexture(GL_TEXTURE_2D, _textureMontanha);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTranslatef(0,5.0,-80.0);
          glScalef(120.0,8.0,50.0);
          gluQuadricTexture(quad1,1);
          gluSphere(quad1,1.2,12,12);
    glPopMatrix();


    glutSwapBuffers();
}

void Anima(int value)
{
	if(posLuz[luz][1] <= 0 && posLuz[luz][2] <= -180){
        posLuz[luz][1] += 1;
        posLuz[luz][2] += 1;

	}
	else if(posLuz[luz][1] == 0 && posLuz[luz][2] <= -150){
        posLuz[luz][2] += 1;
	}
	else if(posLuz[luz][1] <= 90 && posLuz[luz][2] <= -60){
        posLuz[luz][1] += 1;
        posLuz[luz][2] += 1;

	}
	else if(posLuz[luz][1] < 120 && posLuz[luz][2] <= -30){
        posLuz[luz][1] += 1;//Y do Sol
        posLuz[luz][2] += 1;//Z do Sol
        //printf("%f",posLuz[luz][2]);
	}
	else if(posLuz[luz][1] == 120 && posLuz[luz][2] <= 0){
        posLuz[luz][2] += 1;//Z do Sol
        //printf("%f",posLuz[luz][2]);
	}
	else if(posLuz[luz][1] > 120 && posLuz[luz][2] <= 30){
        posLuz[luz][1] -= 1;//Y do Sol
        posLuz[luz][2] += 1;//Z do Sol
	}
	else if(posLuz[luz][1] >= 90 && posLuz[luz][2] <= 60){
        posLuz[luz][1] -= 1;//Y do Sol
        posLuz[luz][2] += 1;//Z do Sol
	}
	else if(posLuz[luz][1] >= 0 && posLuz[luz][2] <= 150){
        posLuz[luz][1] -= 1;//Y do Sol
        posLuz[luz][2] += 1;//Z do Sol
	}
	else if(posLuz[luz][1] == 0 && posLuz[luz][2] <= 180){
        posLuz[luz][2] += 1;//Z do Sol
	}
	else if(posLuz[luz][1] >= -30){
        posLuz[luz][1] -= 1;//Y do Sol
	}

	glutPostRedisplay();
	if(iniciar == true){
        glutTimerFunc(60, Anima, 1);
	}
}

void Teclado (unsigned char key, int x, int y)
{


    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}

void TeclasEspeciais(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_F1:
           glutTimerFunc(1000, Anima, 1);
           iniciar = !iniciar;
           break;
        case GLUT_KEY_UP:
            rotX = 90;
            rotY = 90;
            obsX = 0;
            obsY = 0;
            obsZ = 130;
            break;
        case GLUT_KEY_DOWN:
            rotX = 20;
            rotY = 0;
            obsX = 0;
            obsY = 5;
            obsZ = 50;
            break;
        case GLUT_KEY_LEFT:
            rotX = 0;
            rotY = 90;
            obsX = -50;
            obsY = 50;
            obsZ = 80;
            break;
        case GLUT_KEY_RIGHT:
            rotX = 0;
            rotY = 90;
            obsX = 30;
            obsY = 50;
            obsZ = 80;
            break;
	}
	EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}

// Função callback para eventos de botões do mouse
void GerenciaMouse(int button, int state, int x, int y)
{
	if(state==GLUT_DOWN)
	{
		// Salva os parâmetros atuais
		x_ini = x;
		y_ini = y;
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		bot = button;
	}
	else bot = -1;
}

// Função callback para eventos de movimento do mouse
#define SENS_ROT	10.0
#define SENS_OBS	15.0
#define SENS_TRANSL	30.0
void GerenciaMovim(int x, int y)
{
	// Botão esquerdo ?
	if(bot==GLUT_LEFT_BUTTON)
	{
		// Calcula diferenças
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		// E modifica ângulos
		rotY = rotY_ini - deltax/SENS_ROT;
		rotX = rotX_ini - deltay/SENS_ROT;
	}
	// Botão direito ?
	else if(bot==GLUT_RIGHT_BUTTON)
	{
		// Calcula diferença
		int deltaz = y_ini - y;
		// E modifica distância do observador
		obsZ = obsZ_ini + deltaz/SENS_OBS;
	}
	// Botão do meio ?
	else if(bot==GLUT_MIDDLE_BUTTON)
	{
		// Calcula diferenças
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		// E modifica posições
		obsX = obsX_ini + deltax/SENS_TRANSL;
		obsY = obsY_ini - deltay/SENS_TRANSL;
	}
	PosicionaObservador();
	glutPostRedisplay();
}

GLuint loadTexture(Image* image)
{
	GLuint textureId;
	glGenTextures(1, &textureId); //Abrir espaço para a textura
	glBindTexture(GL_TEXTURE_2D, textureId); //Diz ao OpenGL qual textura editar
	//Mapeie a imagem para a textura
	glTexImage2D(GL_TEXTURE_2D,                //Sempre GL_TEXTURE_2D
				 0,                            //0 por enquanto
				 GL_RGB,                       //Formatar usos do OpenGL para imagem
				 image->width, image->height,  //Altura e Largura
				 0,                            //A borda da imagem
				 GL_RGB, //GL_RGB, porque os pixels são armazenados no formato RGB
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, porque os pixels são armazenados
				                   //como números não assinados
				 image->pixels);               //Os dados de pixel reais
	return textureId; //Retorna o id da textura
}

void Inicializa()
{

    // Define significado dos pontos de controle
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &pontos[0][0][0]);
	// Ativa geração de coordenadas
	glEnable(GL_MAP2_VERTEX_3);

    // Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita as fontes de luz
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);

	// Habilita o modelo de colorização de Gouraud
	glShadeModel(GL_SMOOTH);

    angulo = 0.0f;
    angle=60;

    // Inicializa as variáveis usadas para alterar a posição do
	// observador virtual
	rotX = 20;
	rotY = 0;
	obsX = 0;
	obsY = 5;
	obsZ = 50;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);


	Image* image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/tijolo.bmp");
	_textureTijolo = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/porta.bmp");
	_texturePorta = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/green_slime.bmp");
	_textureTerreno = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/pedras1.bmp");
	_textureEstrada = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/road.bmp");
	_texturaRoad = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/telhado.bmp");
	_textureTelhado = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/janela.bmp");
	_textureJanela = loadTexture(image);

	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/sky.bmp");
	_textureCeu = loadTexture(image);

    quad1 = gluNewQuadric();
	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/montanha.bmp");
	_textureMontanha = loadTexture(image);

    quad2 = gluNewQuadric();
	image = loadBMP("C:/Users/danie/Desktop/Projeto-3d/texturas/sol.bmp");
	_textureSol = loadTexture(image);



	delete image;
}

int main(int argc, char **argv)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInit(&argc, argv);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1366,768);
	glutCreateWindow("Casa/Terreno");
	glEnable(GL_DEPTH_TEST);

	glutReshapeFunc(AlteraTamanhoJanela);
	// Registra a função callback para eventos de botões do mouse
	glutMouseFunc(GerenciaMouse);

	// Registra a função callback para eventos de movimento do mouse
	glutMotionFunc(GerenciaMovim);
	glutKeyboardFunc(Teclado);
	glutSpecialFunc(TeclasEspeciais);
	glutDisplayFunc(Desenha);

	//glutFullScreen();
	Inicializa();

	glutMainLoop();

	return 0;
}
