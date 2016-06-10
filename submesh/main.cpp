#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Mesh.h"

int gridX = 600;
int gridY = 600;
int gridZ = 600;

const double fovy = 50.;
const double clipNear = .01;
const double clipFar = 1000.;
double x = 0;
double z = -2.5;
double y = 0;
int submeshIndex = 0;

std::vector<std::string> paths = {"/Users/rohansawhney/Desktop/developer/C++/submesh/kitten.obj",
                                  "/Users/rohansawhney/Desktop/developer/C++/submesh/house.obj"};
std::vector<Mesh> submeshes;
int m = 0;
bool success = true;

void printInstructions()
{
    std::cerr << "' ': toggle between meshes\n"
              << "↑/↓: move in/out\n"
              << "w/s: move up/down\n"
              << "a/d: move left/right\n"
              << "→/←: toggle between submeshes\n"
              << "escape: exit program\n"
              << std::endl;
}

void clearSubmeshes()
{
    for (int i = 0; i < (int)submeshes.size(); i++) {
        for (int j = 0; j < (int)submeshes[i].facePointers.size(); j++) {
            delete submeshes[i].facePointers[j];
        }
    }
}

void loadMeshes()
{
    Mesh mesh;
    success = mesh.read(paths[m], 0);
    if (success) {
        submeshes = mesh.generateSubmeshesFaceApproach();
    }
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
}

void drawFaces()
{
    for (int i = 0; i < (int)submeshes.size(); i++) {
        if (i == submeshIndex) {
            glColor4f(0.0, 1.0, 0.0, 0.6);
        } else {
            glColor4f(0.0, 0.0, 1.0, 0.6);
        }
        
        for (int j = 0; j < submeshes[i].facePointers.size(); j++) {
            glBegin(GL_LINE_LOOP);
                
            int a = submeshes[i].facePointers[j]->a;
            int b = submeshes[i].facePointers[j]->b;
            int c = submeshes[i].facePointers[j]->c;
                
            glVertex3d(submeshes[i].vertices[a].position.x(),
                        submeshes[i].vertices[a].position.y(),
                        submeshes[i].vertices[a].position.z());
            glVertex3d(submeshes[i].vertices[b].position.x(),
                        submeshes[i].vertices[b].position.y(),
                        submeshes[i].vertices[b].position.z());
            glVertex3d(submeshes[i].vertices[c].position.x(),
                        submeshes[i].vertices[c].position.y(),
                        submeshes[i].vertices[c].position.z());
            
            glEnd();
        }
        
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    double aspect = (double)viewport[2] / (double)viewport[3];
    gluPerspective(fovy, aspect, clipNear, clipFar);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0, 0, z, x, y, 0, 0, 1, 0);
    
    if (success) {
        drawFaces();
    }
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x0, int y0)
{
    switch (key) {
        case 27:
            clearSubmeshes();
            exit(0);
        case ' ':
            m ++;
            if (m == 2) m = 0;
            
            submeshIndex = 0;
            clearSubmeshes();
            loadMeshes();
            break;
        case 'a':
            x -= 0.03;
            break;
        case 'd':
            x += 0.03;
            break;
        case 'w':
            y += 0.03;
            break;
        case 's':
            y -= 0.03;
            break;        
    }
    
    glutPostRedisplay();
}

void special(int i, int x0, int y0)
{
    switch (i) {
        case GLUT_KEY_UP:
            z += 0.03;
            break;
        case GLUT_KEY_DOWN:
            z -= 0.03;
            break;
        case GLUT_KEY_LEFT:
            submeshIndex --;
            if (submeshIndex == -1) {
                submeshIndex = (int)submeshes.size()-1;
            }
            break;
        case GLUT_KEY_RIGHT:
            submeshIndex ++;
            if (submeshIndex == (int)submeshes.size()) {
                submeshIndex = 0;
            }
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv) {

    loadMeshes();
    
    printInstructions();
    glutInitWindowSize(gridX, gridY);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInit(&argc, argv);
    glutCreateWindow("Submeshes");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    
    return 0;
}
