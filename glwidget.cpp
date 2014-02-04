#include <math.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <QtGui>
#include <QtOpenGL>
#include <iostream>
#include "OMFContainer.h"
#include "analysis.h"
#include "glwidget.h"

#define PI (3.141592653589793)

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
  xRot = xLoc = 0;
  yRot = yLoc = 0;
  zRot = xLoc = 0;
  usePtr       = false;
  displayOn    = false;
  topOverlayOn = true;
  topOverlayText = tr("Open files using the \"File\" menu above");

  qtGreen  = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
  qtPurple = QColor::fromRgbF(1.0, 1.0, 1.0);

  setAutoFillBackground(false);
}

GLWidget::~GLWidget()
{
}

void GLWidget::updateData(array_ptr data)
{
  dataPtr    = data;
  displayOn  = true;
  
  // Update the display
  updateCOM();
  updateExtent();
  updateGL();
}

void GLWidget::updateHeader(header_ptr header, array_ptr data)
{
  valuedim = header->valuedim;
  if ((header->xmin == 0.0) && (header->xmax == 0.0) && \
      (header->ymin == 0.0) && (header->ymax == 0.0) && \
      (header->zmin == 0.0) && (header->zmax == 0.0)) {
    // Nothing set for the extents...
    if (valuedim == 3) {
      minmaxmag(data, minmag, maxmag);
    } else if (valuedim == 1) {
      minmax(data, minmag, maxmag);
    }
  }
}

QSize GLWidget::minimumSizeHint() const
{ return QSize(200, 200); }

QSize GLWidget::sizeHint() const 
{ return QSize(800, 400); }

static void qNormalizeAngle(int &angle)
{
  while (angle < 0)
    angle += 360 * 16;
  while (angle > 360 * 16)
    angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
  qNormalizeAngle(angle);
  if (angle != xRot) {
    xRot = angle;
    emit xRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setYRotation(int angle)
{
  qNormalizeAngle(angle);
  if (angle != yRot) {
    yRot = angle;
    emit yRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setZRotation(int angle)
{
  qNormalizeAngle(angle);
  if (angle != zRot) {
    zRot = angle;
    emit zRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setXCom(float val)
{
  if (xcom != val) {
    xcom = val;
    emit COMChanged(val);
    updateGL();
  }
}

void GLWidget::setYCom(float val)
{
  if (ycom != val) {
    ycom = val;
    emit COMChanged(val);
    updateGL();
  }
}

void GLWidget::setZCom(float val)
{
  if (zcom != val) {
    zcom = val;
    emit COMChanged(val);
    updateGL();
  }
}

void GLWidget::setXLoc(float val)
{
  if (xLoc != val) {
    xLoc = val;
    emit COMChanged(val);
    updateGL();
  }
}

void GLWidget::setYLoc(float val)
{
  if (yLoc != val) {
    yLoc = val;
    emit COMChanged(val);
    updateGL();
  }
}

void GLWidget::setZLoc(float val)
{
  if (zLoc != val) {
    zLoc = val;
    emit COMChanged(val);
    updateGL();
  }
}

void GLWidget::setXSliceLow(int low)
{
  if (xSliceLow != low) {
    xSliceLow = low;
    updateGL();
  }
}

void GLWidget::setXSliceHigh(int high)
{
  if (xSliceLow != high) {
    xSliceHigh = high;
    updateGL();
  }
}

void GLWidget::setYSliceLow(int low)
{
  if (ySliceLow != low) {
    ySliceLow = low;
    updateGL();
  }
}

void GLWidget::setYSliceHigh(int high)
{
  if (ySliceLow != high) {
    ySliceHigh = high;
    updateGL();
  }
}

void GLWidget::setZSliceLow(int low)
{
  if (zSliceLow != low) {
    zSliceLow = low;
    updateGL();
  }
}

void GLWidget::setZSliceHigh(int high)
{
  if (zSliceLow != high) {
    zSliceHigh = high;
    updateGL();
  }
}

void GLWidget::updateCOM()
{
  const long unsigned int *size = dataPtr->shape();
  xcom = (float)size[0]*0.5;
  ycom = (float)size[1]*0.5;
  zcom = (float)size[2]*0.5;
}

void GLWidget::updateExtent() 
{
  const long unsigned int *size = dataPtr->shape();
  xmax = size[0];
  ymax = size[1];
  zmax = size[2];
  xmin = 0.0;
  ymin = 0.0;
  zmin = 0.0;
}

void GLWidget::initializeGL()
{
  // GLUT wants argc and argv... qt obscures these in the class
  // so let us circumenvent this problem...
  int argc = 1;
  const char* argv[] = {"Sloppy","glut"};
  glutInit(&argc, (char**)argv);

  qglClearColor(qtPurple.dark());

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_MULTISAMPLE);

  static GLfloat global_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  // Lights
  static GLfloat lightPosition1[4] = { 4.0,  1.0, 10.0, 0.0 };
  static GLfloat lightPosition2[4] = { -4.0, -1.0, 10.0, 0.0 };
  //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
  //glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);

  // Display List for vector
  vector = glGenLists(1);
  GLUquadric *cylinder = gluNewQuadric();
  GLUquadric *bottom = gluNewQuadric();
  gluQuadricOrientation(bottom, GLU_INSIDE); // Flip the disk
  // Draw a vector pointing along the z axis
  glNewList(vector, GL_COMPILE);
  glPushMatrix();
  gluDisk( bottom, 0.0f, 0.2f, 10, 2 );
  gluCylinder( cylinder, 0.2f, 0.2f, 1.0f, 10, 1 );
  glTranslatef(0.0f, 0.0f, 1.0f);
    glutSolidCone(0.35f, 0.95f, 10, 1); // The tip
    glPopMatrix();
    glEndList();

  // Display List for cone
    cone = glGenLists(1);
  // Draw a cone pointing along the z axis
    glNewList(cone, GL_COMPILE);
    glPushMatrix();
    glutSolidCone(0.35f, 0.95f, 10, 1);
    glPopMatrix();
    glEndList();

  // Display List for cube
    cube = glGenLists(1);
  // Draw a cone pointing along the z axis
    glNewList(cube, GL_COMPILE);
    glPushMatrix();
    glutSolidCube(1.0f);
    glPopMatrix();
    glEndList();

  // Set the slice initial conditions
    xSliceLow=ySliceLow=zSliceLow=0;
    xSliceHigh=ySliceHigh=zSliceHigh=16*100;

  // Initial view
    zoom=0.1;
  }

  void GLWidget::toggleDisplay(int cubes) 
  {
    drawCubes = cubes;
    updateGL();
  }

  void GLWidget::paintGL()
  {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    qglClearColor(qtPurple.dark());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
  //gluLookAt(eyex, eyey, eyez, atx, aty, atz, upx, upy, upz);
    glTranslatef(xLoc, yLoc, -15.0 + zoom);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

    if (displayOn) {

      const long unsigned int *size = dataPtr->shape();
      int xnodes = size[0];
      int ynodes = size[1];
      int znodes = size[2];
      float theta, phi, mag;

      for(int i=0; i<xnodes; i++)
      {
       for(int j=0; j<ynodes; j++)
       {
         for(int k=0; k<znodes; k++)
         {
          if (valuedim == 1) {
            mag = (*dataPtr)[i][j][k][0];
          } else {
          mag = sqrt( (*dataPtr)[i][j][k][0] * (*dataPtr)[i][j][k][0] +
           (*dataPtr)[i][j][k][1] * (*dataPtr)[i][j][k][1] +
           (*dataPtr)[i][j][k][2] * (*dataPtr)[i][j][k][2]);
          }

          if ( ((valuedim == 1 ) || ((valuedim == 3) && mag != 0.0)) &&
            i >= (xmax-xmin)*(float)xSliceLow/1600.0 &&
            i <= (xmax-xmin)*(float)xSliceHigh/1600.0 &&
            j >= (ymax-ymin)*(float)ySliceLow/1600.0 &&
            j <= (ymax-ymin)*(float)ySliceHigh/1600.0 &&
            k >= (zmax-zmin)*(float)zSliceLow/1600.0 &&
            k <= (zmax-zmin)*(float)zSliceHigh/1600.0) 
          {

            theta = acos(  (*dataPtr)[i][j][k][2]/mag);
            phi   = atan2( (*dataPtr)[i][j][k][1]/mag,  (*dataPtr)[i][j][k][0]/mag);

            glPushMatrix();

            glTranslatef((float)i-xcom,(float)j-ycom, (float)k-zcom);

            GLfloat color[3];

            float x = (*dataPtr)[i][j][k][0]/mag;
            float y = (*dataPtr)[i][j][k][1]/mag;
            float z = (*dataPtr)[i][j][k][2]/mag;
            float s = sqrt(x*x + y*y + z*z);
            float l = 0.5*z + 0.5;
            float h = atan2(y, x);
            if (valuedim == 1) {
              // std::cout << mag << "\t" << maxmag << std::endl;
              if (maxmag!=minmag) {
                phi = 2.0f*PI*fabs(mag-minmag)/fabs(maxmag-minmag);
              } else {
                phi = 0.0f;
              }
              // std::cout << "Phi:\t" << phi << std::endl;
              angleToRGB(phi, color);
            } else {
              HSLToRGB(h, s, l, color);
            }

            glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
            glMaterialfv(GL_FRONT, GL_AMBIENT, color);
            glColor3fv(color);

            if (drawCubes==0 || valuedim==1) {
              glCallList(cube);
            } else if (drawCubes==1) {		    
              glRotatef(180.0*(phi+0.5*PI)/PI, 0.0, 0.0, 1.0);
              glRotatef(180.0*theta/PI,  1.0, 0.0, 0.0);
              glCallList(cone);
            } else {
              glRotatef(180.0*(phi+0.5*PI)/PI, 0.0, 0.0, 1.0);
              glRotatef(180.0*theta/PI,  1.0, 0.0, 0.0);
              glCallList(vector);
            }
            glPopMatrix();
          }
        }
      }
    }
  } 

  if (topOverlayOn) {
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawInstructions(&painter);
    painter.end();
  }
}

void GLWidget::resizeGL(int width, int height)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (float)width / (float)height, 0.1, 500.0);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, width, height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
  lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
    setXRotation(xRot + 8 * dy);
    setYRotation(yRot + 8 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(xRot + 8 * dy);
    setZRotation(zRot + 8 * dx);
  } else if (event->buttons() & Qt::MiddleButton) {
    setXLoc(xLoc + 0.2*dx);
    setYLoc(yLoc - 0.2*dy);
  }
  lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
  if(event->orientation() == Qt::Vertical)
  {
   zoom += (float)(event->delta()) / 50;
   updateGL();
 }
}

void GLWidget::drawInstructions(QPainter *painter)
{
  QString text = topOverlayText;
  QFont newFont("Helvetica", 9);
  painter->setFont(newFont);
  QFontMetrics metrics = QFontMetrics(font());
  int border = qMax(4, metrics.leading());

  QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.135),
    Qt::AlignCenter | Qt::TextWordWrap, text);

  painter->setRenderHint(QPainter::TextAntialiasing);
  painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
    QColor(0, 0, 0, 127));
  painter->setPen(Qt::white);
  painter->drawText((width() - rect.width())/2, border,
    rect.width(), rect.height(),
    Qt::AlignLeft | Qt::TextWordWrap, text);
}



void GLWidget::updateTopOverlay(QString newstring)
{
  if (newstring != "") {
    topOverlayOn = true;
    topOverlayText = newstring;
  } else {
    topOverlayOn = false;
  }
}

void angleToRGB(float angle, GLfloat *color)
{
  // starting from Hue = angle (in radians)
  // goes from zero to pi
  float piOverThree = PI/3.0;
  float h = fabs((angle)/piOverThree);
  float x = (1.0-fabs(fmodf(h,2.0)-1.0));

  if (h <= 1.0) {
    color[0] = 1.0;
    color[1] = x;
    color[2] = 0.0;
  } else if (h <= 2.0) {
    color[0] = x;
    color[1] = 1.0;
    color[2] = 0.0;
  } else if (h <= 3.0) {
    color[0] = 0.0;
    color[1] = 1.0;
    color[2] = x;
  } else if (h <= 4.0) {
    color[0] = 0.0;
    color[1] = x;
    color[2] = 1.0;
  } else if (h <= 5.0) {
    color[0] = x;
    color[1] = 0.0;
    color[2] = 1.0;
  } else if (h <= 6.0) {
    color[0] = 1.0;
    color[1] = 0.0;
    color[2] = x;
  } else {
    // std::cout << "H is:\t" << h << std::endl;
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;
  }

}

/// Taken form http://www.geekymonkey.com/Programming/CSharp/RGB2HSL_HSL2RGB.htm
void HSLToRGB(float h, float sl, float l, GLfloat *color){
 h = (h + PI)/(2 * PI);
 double v;
 double r,g,b;
 
            r = l;   // default to gray
            g = l;
            b = l;
            v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
            if (v > 0)
            {
              double m;
              double sv;
              int sextant;
              double fract, vsf, mid1, mid2;

              m = l + l - v;
              sv = (v - m ) / v;
              h *= 6.0;
              sextant = (int)h;
              fract = h - sextant;
              vsf = v * sv * fract;
              mid1 = m + vsf;
              mid2 = v - vsf;
              switch (sextant)
              {
                case 0:
                r = v;
                g = mid1;
                b = m;
                break;
                case 1:
                r = mid2;
                g = v;
                b = m;
                break;
                case 2:
                r = m;
                g = v;
                b = mid1;
                break;
                case 3:
                r = m;
                g = mid2;
                b = v;
                break;
                case 4:
                r = mid1;
                g = m;
                b = v;
                break;
                case 5:
                r = v;
                g = m;
                b = mid2;
                break;
              }
            }

            color[0] = r;
            color[1] = g;
            color[2] = b;

          }



