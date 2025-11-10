#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>

// Globals for rotation and animation
static float Xangle = 20.0, Yangle = 0.0, Zangle = 0.0;
static float doorAngle = 0.0; // Door opening angle
static bool doorOpening = false;
static bool doorClosing = false;
static int animationPeriod = 16; // ~60 FPS
static float cameraDistance = -80.0;

// Lighting toggle
static bool lightingEnabled = true;

// Draw a textured box (cabinet body)
void drawBox(float width, float height, float depth)
{
    glBegin(GL_QUADS);

    // Front face
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-width / 2, -height / 2, depth / 2);
    glVertex3f(width / 2, -height / 2, depth / 2);
    glVertex3f(width / 2, height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, depth / 2);

    // Back face
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-width / 2, -height / 2, -depth / 2);
    glVertex3f(-width / 2, height / 2, -depth / 2);
    glVertex3f(width / 2, height / 2, -depth / 2);
    glVertex3f(width / 2, -height / 2, -depth / 2);

    // Top face
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-width / 2, height / 2, -depth / 2);
    glVertex3f(-width / 2, height / 2, depth / 2);
    glVertex3f(width / 2, height / 2, depth / 2);
    glVertex3f(width / 2, height / 2, -depth / 2);

    // Bottom face
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, -height / 2, depth / 2);
    glVertex3f(-width / 2, -height / 2, depth / 2);

    // Right face
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, height / 2, -depth / 2);
    glVertex3f(width / 2, height / 2, depth / 2);
    glVertex3f(width / 2, -height / 2, depth / 2);

    // Left face
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-width / 2, -height / 2, -depth / 2);
    glVertex3f(-width / 2, -height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, -depth / 2);

    glEnd();
}

// Draw fire alarm button
void drawAlarmButton(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    // Button frame (black)
    glColor3f(0.1, 0.1, 0.1);
    glPushMatrix();
    glScalef(3.0, 3.0, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();

    // Red button
    glColor3f(0.9, 0.1, 0.1);
    glTranslatef(0.0, 0.0, 0.5);
    glScalef(2.0, 2.0, 0.8);
    glutSolidCube(1.0);

    glPopMatrix();
}

// Draw small box (fire hose connector)
void drawConnector(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    // Connector box
    glColor3f(0.8, 0.1, 0.1);
    glPushMatrix();
    glScalef(3.5, 3.5, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

    // Small detail
    glColor3f(0.9, 0.9, 0.9);
    glTranslatef(0.0, 0.0, 0.6);
    glutSolidSphere(0.8, 10, 10);

    glPopMatrix();
}

// Draw speaker holes pattern
void drawSpeakerHoles(float x, float y, float z)
{
    glColor3f(0.2, 0.2, 0.2);
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            glPushMatrix();
            glTranslatef(x + i * 1.2, y + j * 1.2, z);
            glutSolidSphere(0.3, 8, 8);
            glPopMatrix();
        }
    }
}

// Draw text "HYDRANT" using simple blocks
void drawHydrantText(float x, float y, float z)
{
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.5, 0.5, 0.5);

    // Just draw a simple representation
    glRasterPos3f(-15.0, 0.0, 0.0);
    const char* text = "HYDRANT";
    for (const char* c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopMatrix();
}

// Draw the complete hydrant cabinet
void drawHydrantCabinet()
{
    // Main cabinet body (red)
    glColor3f(0.7, 0.15, 0.15);

    // Lower compartment (larger)
    glPushMatrix();
    glTranslatef(0.0, -10.0, 0.0);
    drawBox(30.0, 40.0, 10.0);
    glPopMatrix();

    // Upper control panel
    glPushMatrix();
    glTranslatef(0.0, 15.0, 0.0);
    drawBox(30.0, 10.0, 10.0);
    glPopMatrix();

    // Draw components on upper panel
    drawAlarmButton(-8.0, 15.0, 5.5);
    drawConnector(4.0, 15.0, 5.5);
    drawSpeakerHoles(12.0, 15.0, 5.3);

    // Door with animation
    glPushMatrix();
    glTranslatef(-15.0, -10.0, 5.0); // Hinge position
    glRotatef(doorAngle, 0.0, 2.0, 0.0); // Rotate around hinge
    glTranslatef(15.0, 0.0, 0.0); // Move back

    // Door panel (slightly darker red)
    glColor3f(0.65, 0.12, 0.12);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.3);
    drawBox(30.0, 40.0, 1.0);
    glPopMatrix();

    // Door handle
    glColor3f(0.8, 0.8, 0.8);
    glPushMatrix();
    glTranslatef(12.0, -5.0, 0.8);
    glRotatef(90, 0.0, 1.0, 0.0);
    glutSolidTorus(0.5, 1.5, 10, 15);
    glPopMatrix();

    glPopMatrix();

    // Draw "HYDRANT" text on lower door
    glPushMatrix();
    glTranslatef(-15.0, -10.0, 5.0);
    glRotatef(doorAngle, 0.0, 1.0, 0.0);
    glTranslatef(15.0, 0.0, 0.0);
    drawHydrantText(0.0, -5.0, 1.5);
    glPopMatrix();

    // Side pipes
    glColor3f(0.7, 0.15, 0.15);
    glPushMatrix();
    glTranslatef(15.0, 0.0, 0.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 2.0, 2.0, 15.0, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

// Drawing routine
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera positioning
    glTranslatef(0.0, 0.0, cameraDistance);

    // Scene rotation
    glRotatef(Xangle, 1.0, 0.0, 0.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Zangle, 0.0, 0.0, 1.0);

    // Draw ground plane for reference
    glDisable(GL_LIGHTING);
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glVertex3f(-50.0, -30.0, -50.0);
    glVertex3f(50.0, -30.0, -50.0);
    glVertex3f(50.0, -30.0, 50.0);
    glVertex3f(-50.0, -30.0, 50.0);
    glEnd();

    if (lightingEnabled)
        glEnable(GL_LIGHTING);

    // Draw the hydrant cabinet
    drawHydrantCabinet();

    glutSwapBuffers();
}

// Animation timer
void animate(int value)
{
    // Door animation
    if (doorOpening && doorAngle < 120.0)
    {
        doorAngle += 2.0;
        if (doorAngle >= 120.0)
        {
            doorAngle = 120.0;
            doorOpening = false;
        }
    }
    else if (doorClosing && doorAngle > 0.0)
    {
        doorAngle -= 2.0;
        if (doorAngle <= 0.0)
        {
            doorAngle = 0.0;
            doorClosing = false;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(animationPeriod, animate, 1);
}


// Setup lighting
void setupLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // Light 0 - Key light (white, from upper right)
    GLfloat light0_pos[] = { 30.0, 50.0, 30.0, 1.0 };
    GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light0_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Light 1 - Fill light (softer, from left)
    GLfloat light1_pos[] = { -30.0, 20.0, 20.0, 1.0 };
    GLfloat light1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat light1_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);

    // Material properties
    GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

// Initialization routine
void setup(void)
{
    glClearColor(0.9, 0.9, 0.95, 0.0); // Light blue-gray background
    glEnable(GL_DEPTH_TEST);

    setupLighting();

    glutTimerFunc(5, animate, 1);
}

// OpenGL window reshape routine
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 3D Perspective projection
    gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);

    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC
        exit(0);
        break;
    case 'x':
        Xangle += 5.0;
        if (Xangle > 360.0) Xangle -= 360.0;
        break;
    case 'X':
        Xangle -= 5.0;
        if (Xangle < 0.0) Xangle += 360.0;
        break;
    case 'y':
        Yangle += 5.0;
        if (Yangle > 360.0) Yangle -= 360.0;
        break;
    case 'Y':
        Yangle -= 5.0;
        if (Yangle < 0.0) Yangle += 360.0;
        break;
    case 'z':
        Zangle += 5.0;
        if (Zangle > 360.0) Zangle -= 360.0;
        break;
    case 'Z':
        Zangle -= 5.0;
        if (Zangle < 0.0) Zangle += 360.0;
        break;
    case 'o': // Open door
    case 'O':
        doorOpening = true;
        doorClosing = false;
        break;
    case 'c': // Close door
    case 'C':
        doorClosing = true;
        doorOpening = false;
        break;
    case 'l': // Toggle lighting
    case 'L':
        lightingEnabled = !lightingEnabled;
        if (lightingEnabled)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        break;
    case '+': // Zoom in
        cameraDistance += 5.0;
        break;
    case '-': // Zoom out
        cameraDistance -= 5.0;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// Special key input (arrow keys)
void specialKeyInput(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        Xangle -= 5.0;
        break;
    case GLUT_KEY_DOWN:
        Xangle += 5.0;
        break;
    case GLUT_KEY_LEFT:
        Yangle -= 5.0;
        break;
    case GLUT_KEY_RIGHT:
        Yangle += 5.0;
        break;
    }
    glutPostRedisplay();
}

// Print instructions
void printInstructions()
{
    std::cout << "\n===== 3D Fire Hydrant Cabinet =====" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  x/X: Rotate around X-axis" << std::endl;
    std::cout << "  y/Y: Rotate around Y-axis" << std::endl;
    std::cout << "  z/Z: Rotate around Z-axis" << std::endl;
    std::cout << "  Arrow Keys: Rotate view" << std::endl;
    std::cout << "  o/O: Open door" << std::endl;
    std::cout << "  c/C: Close door" << std::endl;
    std::cout << "  l/L: Toggle lighting" << std::endl;
    std::cout << "  +/-: Zoom in/out" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
    std::cout << "===================================\n" << std::endl;
}

// Main routine
int main(int argc, char** argv)
{
    printInstructions();

    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Fire Hydrant Cabinet - OpenGL");

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();

    return 0;
}