#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdint.h>

#define ROWS 9  // Number of rows of asteroids.

// Globals.
static intptr_t font = (intptr_t)GLUT_BITMAP_8_BY_13; // Font selection.
static int width, height; // Size of the OpenGL window.
static float angle = 0.0; // Angle of the spacecraft.
static float xVal = 0, zVal = 0; // Co-ordinates of the spacecraft.
static int isCollision = 0; // Is there collision between the spacecraft and an asteroid?
static unsigned int spacecraft; // Display lists base index.
static int frameCount = 0; // Number of frames

// animation
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

static float rotateAroundItself[ROWS] = {0, 2.0, 1.0, 4.0, 3.0, 8.0, 7.0, 5.0, 6.0};
static float rotateAroundSun[ROWS] = {0, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
static float moonRotate_aroundSun = 0.6;
static float moonRotate_aroundEarth = 1;
static float newMoonASun = 0;
static float newMoonAEarth = 0;

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
    char *c;

    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Asteroid class.
class Asteroid
{
public:
    Asteroid();
    Asteroid(float x, float y, float z, float r, unsigned char colorR,
             unsigned char colorG, unsigned char colorB, float rate, float s_rate);
    float getCenterX() { return centerX; }
    float getCenterY() { return centerY; }
    float getCenterZ() { return centerZ; }
    float getRadius() { return radius; }
    float getRotateRate() { return rotateRate; }
    float getS_rotateRate() { return s_rotateRate; }
    unsigned char getColorR() { return color[0]; }
    unsigned char getColorG() { return color[1]; }
    unsigned char getColorB() { return color[2]; }
    void draw();

public:
    float centerX, centerY, centerZ, radius, rotateRate, s_rotateRate;
    unsigned char color[3];
};

// Asteroid default constructor.
Asteroid::Asteroid()
{
    centerX = 0.0;
    centerY = 0.0;
    centerZ = 0.0;
    radius = 0.0; // Indicates no asteroid exists in the position.
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    rotateRate = 0.0;
    s_rotateRate = 0.0;
}

// Asteroid constructor.
Asteroid::Asteroid(float x, float y, float z, float r, unsigned char colorR,
                   unsigned char colorG, unsigned char colorB, float rate, float s_rate)
{
    centerX = x;
    centerY = y;
    centerZ = z;
    radius = r;
    color[0] = colorR;
    color[1] = colorG;
    color[2] = colorB;
    rotateRate = rate;
    s_rotateRate = s_rate;
}

// Function to draw asteroid.
void Asteroid::draw()
{
    if (radius > 0.0) // If asteroid exists.
    {
        glPushMatrix();
        glTranslatef(centerX, centerY, centerZ);

        // Rotate scene(around itself)
        glRotatef(Zangle, 0.0, 0.0, 1.0);
        glRotatef(rotateRate, 0.0, 1.0, 0.0);
        glRotatef(Xangle, 1.0, 0.0, 0.0);

        glColor3ubv(color);
        glutSolidSphere(radius, (int)radius * 20, (int)radius * 20);
        glPopMatrix();
    }
}

Asteroid arrayAsteroids[ROWS]; // Global array of asteroids.

// Routine to count the number of frames drawn every second.
void frameCounter(int value)
{
    if (value != 0) // No output the first time frameCounter() is called (from main()).
        std::cout << "FPS = " << frameCount << std::endl;
    frameCount = 0;
    glutTimerFunc(1000, frameCounter, 1);
}

void animate(int value)
{
    float newAngle;
    float new_S_angle;

    if (isAnimate)
    {
        for(int i = 0; i < 9; i++) {
            if(rotateAroundItself[i]) { // is a planet
                newAngle = (arrayAsteroids[i].getRotateRate()) + rotateAroundItself[i];
                if (newAngle > 360.0) newAngle -= 360.0;
                arrayAsteroids[i].rotateRate = newAngle;
            }

            if(rotateAroundSun[i]) {
                new_S_angle = (arrayAsteroids[i].getS_rotateRate()) + rotateAroundSun[i];
                if (new_S_angle > 360.0) new_S_angle -= 360.0;
                arrayAsteroids[i].s_rotateRate = new_S_angle;
            }
        }

        // moon
        newMoonASun += 0.6;
        if (newMoonASun > 360.0) newMoonASun -= 360.0;
        moonRotate_aroundSun = newMoonASun;

        newMoonAEarth += 1;
        if (newMoonAEarth > 360.0) newMoonAEarth -= 360.0;
        moonRotate_aroundEarth = newMoonAEarth;

        glutPostRedisplay();
        glutTimerFunc(animationPeriod, animate, 1);
    }
}

// lighting ////////////////////////////////////
void init() {
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // We'll use light source 0

    GLfloat sunDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, sunDiffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);
}

// Initialization routine.
void setup(void)
{
    spacecraft = glGenLists(1);
    glNewList(spacecraft, GL_COMPILE);
    glPushMatrix();
    glRotatef(180.0, 0.0, 1.0, 0.0); // To make the spacecraft point down the $z$-axis initially.
    glColor3f(1.0, 1.0, 1.0);
    glutWireCone(5.0, 10.0, 10, 10);
    glPopMatrix();
    glEndList();

    // Initialize global arrayAsteroids.
    arrayAsteroids[0] = Asteroid(0.0, 0.0, -40.0, 15.0,
                                 228.0, 159.0, 39,
                                 0.0, 0.0);
    arrayAsteroids[1] = Asteroid(20, 0.0, -50.0, 1,
                                 110.0, 110.0, 110.0,
                                 0.0, 0.0);
    arrayAsteroids[2] = Asteroid(40, 0.0, -60.0, 3.0,
                                 177, 79, 27,
                                 0.0, 0.0);
    arrayAsteroids[3] = Asteroid(60, 0.0, -70.0, 4.0,
                                 34.0, 60.0, 122,
                                 0.0, 0.0);
    arrayAsteroids[4] = Asteroid(80, 0.0, -80.0, 2.0,
                                 95.0, 44.0, 34.0,
                                 0.0, 0.0);
    arrayAsteroids[5] = Asteroid(100, 0.0, -90.0, 8.0,
                                 157.0, 124.0, 100,
                                 0.0, 0.0);
    arrayAsteroids[6] = Asteroid(120, 0.0, -100.0, 7.0,
                                 181.0, 162.0, 131.0,
                                 0.0, 0.0);
    arrayAsteroids[7] = Asteroid(140, 0.0, -110.0, 6.0,
                                 85.0, 120.0, 128.0,
                                 0.0, 0.0);
    arrayAsteroids[8] = Asteroid(160, 0.0, -120.0, 5.0,
                                 42.0, 74.0, 131.0,
                                 0.0, 0.0);


    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glutTimerFunc(0, frameCounter, 0); // Initial call of frameCounter().
}

// Function to check if two spheres centered at (x1,y1,z1) and (x2,y2,z2) with
// radius r1 and r2 intersect.
int checkSpheresIntersection(float x1, float y1, float z1, float r1,
                             float x2, float y2, float z2, float r2)
{
    return ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2) <= (r1 + r2)*(r1 + r2));
}

// Function to check if the spacecraft collides with an asteroid when the center of the base
// of the craft is at (x, 0, z) and it is aligned at an angle a to to the -z direction.
// Collision detection is approximate as instead of the spacecraft we use a bounding sphere.
int asteroidCraftCollision(float x, float z, float a)
{
    int i;

    // Check for collision with each asteroid.
    for (i = 0; i<ROWS; i++)
        if (arrayAsteroids[i].getRadius() > 0) // If asteroid exists.
            if (checkSpheresIntersection(x - 5 * sin((M_PI / 180.0) * a), 0.0,
                                         z - 5 * cos((M_PI / 180.0) * a), 7.072,
                                         arrayAsteroids[i].getCenterX(), arrayAsteroids[i].getCenterY(),
                                         arrayAsteroids[i].getCenterZ(), arrayAsteroids[i].getRadius()))
                return 1;
    return 0;
}

//void drawCircle(float X, float Y, float Z, float radius, int numSegments) {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glColor3f(1.0f, 1.0f, 1.0f);  // Set color to white
//    glRotatef(15, 0.0, 0.0, 1.0);
//    glBegin(GL_LINE_LOOP);
//    for (int i = 0; i < numSegments; i++) {
//        float theta = 2.0f * M_PI * float(i) / float(numSegments);  // Angle for each segment
//        float x = radius * cos(theta);
//        float y = radius * sin(theta);
//        glVertex3f(x + X, y + Y, Z);
//    }
//    glEnd();
//
//    glFlush();
//}

// Drawing routine.
void drawScene(void)
{
    frameCount++; // Increment number of frames every redraw.

    int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Begin viewport #2 (solar system).
    glViewport(570, 20, width / 4.0, height / 4); //demo
    glLoadIdentity();

    // Write text in isolated (i.e., before gluLookAt) translate block.
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos3f(-28.0, 25.0, -30.0);
    if (isCollision) writeBitmapString((void*)font, "Cannot - will crash!");
    glPopMatrix();

    // Fixed camera.
    gluLookAt(0.0, 100.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Set up the sun's light properties
    GLfloat sunPosition[] = { arrayAsteroids[0].getCenterX(), arrayAsteroids[0].getCenterY(), arrayAsteroids[0].getCenterZ(), 1.0f };
    float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };
    float lightDifAndSpec0[] = { 0.5, 0.5, 0.5, 1.0 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, sunPosition);
    arrayAsteroids[0].draw();  // sunnnnnnnnnnn
    glPopMatrix();

    glEnable(GL_LIGHTING);

    // Draw all the asteroids in arrayAsteroids.
    for (i = 1; i < ROWS; i++) {

        float cR = arrayAsteroids[i].getColorR()/255.0;
        float cG = arrayAsteroids[i].getColorG()/255.0;
        float cB = arrayAsteroids[i].getColorB()/255.0;
        // Set the material properties for the asteroid
        GLfloat asteroidAmbient[] = { cR, cG, cB, 1.0f };  // Set the ambient color
        GLfloat asteroidShininess = 128.0f; // Set the shininess

        glMaterialfv(GL_FRONT, GL_AMBIENT, asteroidAmbient);
        glMaterialf(GL_FRONT, GL_SHININESS, asteroidShininess);

        glPushMatrix();
        // Rotate scene(around sun)
        glTranslatef(0.0, 0.0, -40);
        glRotatef(arrayAsteroids[i].s_rotateRate, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, 40);
        arrayAsteroids[i].draw();
        glPopMatrix();
    }

    // moon
    glPushMatrix();
    // around sun
    glTranslatef(0, 0, -40);
    glRotatef(moonRotate_aroundSun, 0.0, 1.0, 0.0);
    glTranslatef(0, 0, 40);
    // around earth
    glTranslatef(60, 0, -70);
    glRotatef(moonRotate_aroundEarth, 0.0, 1.0, 0.0);
    glTranslatef(-60, 0, 70);
    // draw
    glPushMatrix();
    glTranslatef(70, 0, -70);
    glColor3f(1, 1, 1);
    glutSolidSphere(1, (int)1 * 20, (int)1 * 20);
    glPopMatrix();

    glPopMatrix();

    // Draw spacecraft.
    glPushMatrix();
    glTranslatef(xVal, 0.0, zVal);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glCallList(spacecraft);
    glPopMatrix();
    // End viewport #2 (solar system).

    // Begin viewport #1 (Space craft view).
    glViewport(0, 0, width, height);
    glLoadIdentity();

    // Write text in isolated (i.e., before gluLookAt) translate block.
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos3f(-28.0, 25.0, -30.0);
    if (isCollision) writeBitmapString((void*)font, "Cannot - will crash!");
    glPopMatrix();

    // Locate the camera at the tip of the cone and pointing in the direction of the cone.
    gluLookAt(xVal - 10 * sin((M_PI / 180.0) * angle),
              0.0,
              zVal - 10 * cos((M_PI / 180.0) * angle),
              xVal - 11 * sin((M_PI / 180.0) * angle),
              0.0,
              zVal - 11 * cos((M_PI / 180.0) * angle),
              0.0,
              1.0,
              0.0);

//    glPushMatrix();
//    drawCircle(-60.0, 0.0, -70, 40, 100);
//    glPopMatrix();
//

// lighting

    // Set up the sun's light properties
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, sunPosition);
    arrayAsteroids[0].draw();  // sunnnnnnnnnnn
    glPopMatrix();

    glEnable(GL_LIGHTING);

    // Draw all the asteroids in arrayAsteroids.
    for (i = 1; i < ROWS; i++) {

        float cR = arrayAsteroids[i].getColorR()/255.0;
        float cG = arrayAsteroids[i].getColorG()/255.0;
        float cB = arrayAsteroids[i].getColorB()/255.0;
        // Set the material properties for the asteroid
        GLfloat asteroidAmbient[] = { cR, cG, cB, 1.0f };  // Set the ambient color
        GLfloat asteroidShininess = 128.0f; // Set the shininess

        glMaterialfv(GL_FRONT, GL_AMBIENT, asteroidAmbient);
        glMaterialf(GL_FRONT, GL_SHININESS, asteroidShininess);

        glPushMatrix();
        // Rotate scene(around sun)
        glTranslatef(0.0, 0.0, -40);
        glRotatef(arrayAsteroids[i].s_rotateRate, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, 40);
        arrayAsteroids[i].draw();
        glPopMatrix();
    }

    //moon
    glPushMatrix();
    // around sun
    glTranslatef(0, 0, -40);
    glRotatef(moonRotate_aroundSun, 0.0, 1.0, 0.0);
    glTranslatef(0, 0, 40);
    // around earth
    glTranslatef(60, 0, -70);
    glRotatef(moonRotate_aroundEarth, 0.0, 1.0, 0.0);
    glTranslatef(-60, 0, 70);
    // draw
    glPushMatrix();
    glTranslatef(70, 0, -70);
    glColor3f(1, 1, 1);
    glutSolidSphere(1, (int)1 * 20, (int)1 * 20);
    glPopMatrix();

    glPopMatrix();
    // End viewport #1 (Space craft view).

    glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 250.0);
    glMatrixMode(GL_MODELVIEW);

    // Pass the size of the OpenGL window.
    width = w;
    height = h;
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;
        case ' ':
            if (isAnimate) isAnimate = 0;
            else
            {
                isAnimate = 1;
                animate(1);
            }
            break;
        default:
            break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    float tempxVal = xVal, tempzVal = zVal, tempAngle = angle;

    // Compute next position.
    if (key == GLUT_KEY_LEFT) tempAngle = angle + 5.0;
    if (key == GLUT_KEY_RIGHT) tempAngle = angle - 5.0;
    if (key == GLUT_KEY_UP)
    {
        tempxVal = xVal - sin(angle * M_PI / 180.0);
        tempzVal = zVal - cos(angle * M_PI / 180.0);
    }
    if (key == GLUT_KEY_DOWN)
    {
        tempxVal = xVal + sin(angle * M_PI / 180.0);
        tempzVal = zVal + cos(angle * M_PI / 180.0);
    }

    // Angle correction.
    if (tempAngle > 360.0) tempAngle -= 360.0;
    if (tempAngle < 0.0) tempAngle += 360.0;

    // Move spacecraft to next position only if there will not be collision with an asteroid.
    if (!asteroidCraftCollision(tempxVal, tempzVal, tempAngle))
    {
        isCollision = 0;
        xVal = tempxVal;
        zVal = tempzVal;
        angle = tempAngle;
    }
    else isCollision = 1;

    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    std::cout << "Interaction:" << std::endl;
    std::cout << "press the space key to start animation, and enjoy :)"
                 "Press the left/right arrow keys to turn the craft." << std::endl
              << "Press the up/down arrow keys to move the craft." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("spaceTravel.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    init();
    setup();

    glutMainLoop();
}