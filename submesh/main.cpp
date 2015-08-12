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

bool showFaces = true;
std::vector<std::string> paths = {"/Users/rohansawhney/Desktop/developer/C++/submesh/kitten.obj"};

std::vector<Mesh> submeshes;
bool success = true;

void printInstructions()
{
    std::cerr << "space: toggle between meshes\n"
              << "q: toggle between drawing polygons and edges\n"
              << "↑/↓: move in/out\n"
              << "w/s: move up/down\n"
              << "a/d: move left/right\n"
              << "escape: exit program\n"
              << std::endl;
}

void loadMeshes()
{
    Mesh mesh;
    success = mesh.read(paths[0]);
    if (success) {
        submeshes = mesh.generateSubmeshes();
    }
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
}

void drawEdges()
{
    for (int i = 0; i < (int)submeshes.size(); i++) {
        if (i == submeshIndex) {
            glColor4f(0.0, 1.0, 0.0, 0.6);
        } else {
            glColor4f(1.0, 1.0, 1.0, 0.6);
        }
        
        glBegin(GL_LINES);
        for (EdgeCIter e = submeshes[i].edges.begin(); e != submeshes[i].edges.end(); e ++) {
            Eigen::Vector3d a = e->he->vertex->position;
            Eigen::Vector3d b = e->he->flip->vertex->position;
            
            glVertex3d(a.x(), a.y(), a.z());
            glVertex3d(b.x(), b.y(), b.z());
        }
        
        glEnd();
    }
}

void drawFaces()
{
    for (int i = 0; i < (int)submeshes.size(); i++) {
        if (i == submeshIndex) {
            glColor4f(0.0, 1.0, 0.0, 0.6);
        } else {
            glColor4f(0.0, 0.0, 1.0, 0.6);
        }
        
        for (FaceCIter f = submeshes[i].faces.begin(); f != submeshes[i].faces.end(); f++) {
        
            if (f->isBoundary()) continue;
        
            glBegin(GL_LINE_LOOP);
            HalfEdgeCIter he = f->he;
            do {
                glVertex3d(he->vertex->position.x(),
                           he->vertex->position.y(),
                           he->vertex->position.z());
            
                he = he->next;
            
            } while (he != f->he);
        
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
        if (showFaces) {
            drawFaces();
            
        } else {
            drawEdges();
        }
    }
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x0, int y0)
{
    switch (key) {
        case 27 :
            exit(0);
        case ' ':
            loadMeshes();
            break;
        case 'q':
            showFaces = !showFaces;
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
        case GLUT_KEY_UP:
            z += 0.03;
            break;
        case GLUT_KEY_DOWN:
            z -= 0.03;
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