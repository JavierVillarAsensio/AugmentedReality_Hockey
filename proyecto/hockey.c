#include <GL/glut.h>    
#include <AR/gsub.h>    
#include <AR/video.h>   
#include <AR/param.h>   
#include <AR/ar.h>
#include "orej.h"
#include <string.h>
#include <math.h>

#define DISK_PATT "data/patts/identic.patt"
#define HAND_PATT "data/patts/simple.patt"
#define GOAL_PATT "data/patts/kanji.patt"

#define WHITE "data/colors/blanco.ppm"
#define RED "data/colors/rojo.ppm"
#define BLUE "data/colors/azul.ppm"

#define DISK "data/models/disco.orj"
#define HAND "data/models/agarrador.orj"
#define GOAL "data/models/goal.orj"

// ==== Definicion de estructuras ===================================
struct TObject{
  int id;                      // Identificador del patron
  int visible;                 // Es visible el objeto?
  double width;                // Ancho del patron
  double center[2];            // Centro del patron  
  double patt_trans[3][4];     // Matriz asociada al patron
  void (* drawme)(void);       // Puntero a funcion drawme
};

struct TObject *objects = NULL;
int nobjects = 0;

OrjObjeto goal;
OrjObjeto disk;
OrjObjeto hand;

double speed_direction[2] = {0.000001, 0};
GLfloat disk_displacement[2] = {0, 0};
int hand_id = -1;
int goal_id = -1;
int disk_id = -1;
double patts_width = 0;
double speed = 3.0;
double disk_mat[3][4];
double prev_dist = 0;

void print_error (char *error) {  printf("%s\n", error); exit(0); }

double calculate_angle(double obj[3][4]) {
  //angle = arcos((u+v)/(mod_dist*mod_speed))
  double dist_vec[2];

  dist_vec[0] = disk_mat[0][3] - obj[0][3]; 
  dist_vec[1] = disk_mat[1][3] - obj[1][3];

  //printf("distvec: %f  %f\n", dist_vec[0], dist_vec[1]);
  //printf("speeddir: %f  %f\n", speed_direction[0], speed_direction[1]);

  double u = dist_vec[0] * speed_direction[0];
  double v = dist_vec[1] * speed_direction[1];

  //printf("u v: %f  %f\n", u, v);

  double mod_dist = sqrt(pow(dist_vec[0], 2) + pow(dist_vec[1], 2));
  double mod_speed = sqrt(pow(speed_direction[0], 2) + pow(speed_direction[1], 2));

  //printf("mods dist speed: %f  %f\n", mod_dist, mod_speed);

  float arcos = (float)(u + v)/(mod_dist * mod_speed);
  //printf("arcos %f\n", arcos);
  double angle = acosf(arcos); 
  printf("angle: %f\n\n", angle);

  return angle;
}

void change_speed_direction(double new_angle){
  speed_direction[0] = cos(new_angle) * speed;
  speed_direction[1] = sin(new_angle) * speed;
}

void disk_movement() {
  double inv[3][4];
  double mul[3][4];
  double dist;
  int nesw;
  double new_angle;
  arUtilMatInv(disk_mat, inv);
  printf("inv:\n%f  %f  %f  %f\n%f  %f  %f  %f\n%f  %f  %f  %f\n", inv[0][0], inv[0][1], inv[0][2], inv[0][3], inv[1][0], inv[1][1], inv[1][2], inv[1][3], inv[2][0], inv[2][1], inv[2][2], inv[2][3]);

  for(int i = 0; i < nobjects; i++){
    if(objects[i].id == hand_id){
      //printf("miro %d\n", objects[i].id);
      arUtilMatMul(inv, objects[i].patt_trans, mul);
      dist = sqrt(pow(mul[0][3], 2) + pow(mul[1][3], 2) + pow(mul[2][3], 2));
      printf("dist prev: %f  %f\n", dist, prev_dist);
      if (((abs)(prev_dist - dist)) > 100){
        prev_dist = dist;
        if(dist <= 220.0){
          new_angle = calculate_angle(objects[i].patt_trans);
          change_speed_direction(new_angle);
      }
        }
        
    }
  }
}

// ==== addObject (Anade objeto a la lista de objetos) ==============
void addObject(char *p, double w, double c[2], void (*drawme)(void)) 
{
  int pattid;

  if((pattid=arLoadPatt(p)) < 0)
    print_error ("Error en carga de patron\n");
  else{
    if(strcmp(p, HAND_PATT) == 0)
      hand_id = pattid;
    else if(strcmp(p, DISK_PATT) == 0)
      disk_id = pattid;
    else
      goal_id = pattid;
  }

  printf("%d\n", pattid);

  nobjects++;
  objects = (struct TObject *) 
    realloc(objects, sizeof(struct TObject)*nobjects);

  objects[nobjects-1].id = pattid;
  objects[nobjects-1].width = w;
  objects[nobjects-1].center[0] = c[0];
  objects[nobjects-1].center[1] = c[1];
  objects[nobjects-1].drawme = drawme;   
  
}

// ==== draw****** (Dibujado especifico de cada objeto) =============
void drawHand(void) {
  GLfloat material[]     = {1.0, 0.0, 0.0, 1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  desplegarObjeto(&hand, ESOLIDO);
}

void drawGoal(void){
  GLfloat material[]     = {0.0, 0.0, 0.0, 1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  desplegarObjeto(&goal, ESOLIDO);
}

void drawDisk(void) {
  disk_movement();
  disk_displacement[0] = disk_displacement[0] + (GLfloat)(speed * speed_direction[0]);
  disk_displacement[1] = disk_displacement[1] + (GLfloat)(speed * speed_direction[1]);
  //printf("x: %f, y: %f\n\n", speed_direction[0], speed_direction[1]);
  glTranslatef(disk_displacement[0], disk_displacement[1], 0.0);
  desplegarObjeto(&disk, ESOLIDO);
}

// ======== cleanup =================================================
static void cleanup(void) {   // Libera recursos al salir ...
  arVideoCapStop();  arVideoClose();  argCleanup();  free(objects);  
  exit(0);
}

// ======== keyboard ================================================
static void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x1B: case 'Q': case 'q':
    cleanup(); break;
  case 'C': case 'c':
    disk_displacement[0] = 0;
    disk_displacement[1] = 0;
    speed_direction[0] = 0.000001;
    speed_direction[1] = 0;
    }
}

// ======== draw ====================================================
void draw( void ) {
  double  gl_para[16];   // Esta matriz 4x4 es la usada por OpenGL
  GLfloat light_position[]  = {100.0,-200.0,200.0,0.0};
  int i;
  
  argDrawMode3D();              // Cambiamos el contexto a 3D
  argDraw3dCamera(0, 0);        // Y la vista de la camara a 3D
  glClear(GL_DEPTH_BUFFER_BIT); // Limpiamos buffer de profundidad
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  for (i=0; i<nobjects; i++) {
    if (objects[i].visible) {   // Si el objeto es visible
    printf("pinto %d\n", objects[i].id);
      if (objects[i].id == disk_id){
        disk_mat[0][0] = objects[i].patt_trans[0][0];
        disk_mat[0][1] = objects[i].patt_trans[0][1];
        disk_mat[0][2] = objects[i].patt_trans[0][2];
        disk_mat[0][3] = objects[i].patt_trans[0][3];

        disk_mat[1][0] = objects[i].patt_trans[1][0];
        disk_mat[1][1] = objects[i].patt_trans[1][1];
        disk_mat[1][2] = objects[i].patt_trans[1][2];
        disk_mat[1][3] = objects[i].patt_trans[1][3];

        disk_mat[2][0] = objects[i].patt_trans[2][0];
        disk_mat[2][1] = objects[i].patt_trans[2][1];
        disk_mat[2][2] = objects[i].patt_trans[2][2];
        disk_mat[2][3] = objects[i].patt_trans[2][3];
        printf("matriz copiada\n");
      }

      argConvGlpara(objects[i].patt_trans, gl_para);   
      glMatrixMode(GL_MODELVIEW);           
      glLoadMatrixd(gl_para);   // Cargamos su matriz de transf.            

      glEnable(GL_LIGHTING);  glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);
      objects[i].drawme();      // Llamamos a su función de dibujar
    }
  }
  glDisable(GL_DEPTH_TEST);
}

// ======== init ====================================================
static void init( void ) {
  ARParam  wparam, cparam;   // Parametros intrinsecos de la camara
  int xsize, ysize;          // Tamano del video de camara (pixels)
  double c[2] = {0.0, 0.0};  // Centro de patron (por defecto)
  
  // Abrimos dispositivo de video
  if(arVideoOpen("-dev=/dev/video2") < 0) exit(0);  
  if(arVideoInqSize(&xsize, &ysize) < 0) exit(0);

  // Cargamos los parametros intrinsecos de la camara
  if(arParamLoad("data/camera_para.dat", 1, &wparam) < 0)   
    print_error ("Error en carga de parametros de camara\n");
  
  arParamChangeSize(&wparam, xsize, ysize, &cparam);
  arInitCparam(&cparam);   // Inicializamos la camara con "cparam"

  // Inicializamos la lista de objetos
  printf("hand: ");
  addObject(HAND_PATT, 87.0, c, drawHand); 
  printf("disk: ");
  addObject(DISK_PATT, 87.0, c, drawDisk);
  printf("goal: ");
  addObject(GOAL_PATT, 52.0, c, drawGoal);
  
  patts_width = objects[0].width;

  argInit(&cparam, 1.0, 0, 0, 0, 0);   // Abrimos la ventana 
}

// ======== mainLoop ================================================
static void mainLoop(void) {
  ARUint8 *dataPtr;
  ARMarkerInfo *marker_info;
  int marker_num, i, j, k;

  // Capturamos un frame de la camara de video
  if((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
    // Si devuelve NULL es porque no hay un nuevo frame listo
    arUtilSleep(2);  return;  // Dormimos el hilo 2ms y salimos
  }

  argDrawMode2D();
  argDispImage(dataPtr, 0,0);    // Dibujamos lo que ve la camara 

  // Detectamos la marca en el frame capturado (return -1 si error)
  if(arDetectMarker(dataPtr, 100, &marker_info, &marker_num) < 0) {
    cleanup(); exit(0);   // Si devolvio -1, salimos del programa!
  }

  arVideoCapNext();      // Frame pintado y analizado... A por otro!

  // Vemos donde detecta el patron con mayor fiabilidad
  for (i=0; i<nobjects; i++) {
    for(j = 0, k = -1; j < marker_num; j++) {
      if(objects[i].id == marker_info[j].id) {
	if (k == -1) k = j;
	else if(marker_info[k].cf < marker_info[j].cf) k = j;
      }
    }
    
    if(k != -1) {   // Si ha detectado el patron en algun sitio...
      objects[i].visible = 1;
      arGetTransMat(&marker_info[k], objects[i].center, 
		    objects[i].width, objects[i].patt_trans);
    } else { objects[i].visible = 0; }  // El objeto no es visible
  }
 
  draw();           // Dibujamos los objetos de la escena
  argSwapBuffers(); // Cambiamos el buffer con lo que tenga dibujado
}

// ======== Main ====================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);    // Creamos la ventana OpenGL con Glut
  init();                   // Llamada a nuestra funcion de inicio
  
  cargarObjeto(&disk, DISK, RED, NOANIM, 25);
  cargarObjeto(&goal, GOAL, BLUE, NOANIM, 15);
  cargarObjeto(&hand, HAND, WHITE, NOANIM, 50);

  arVideoCapStart();        // Creamos un hilo para captura de video
  argMainLoop( NULL, keyboard, mainLoop );    // Asociamos callbacks
  return (0);
}
