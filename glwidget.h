#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <boost/smart_ptr.hpp>
#include "OMFContainer.h"
#include "OMFHeader.h"

typedef boost::shared_ptr<OMFHeader> header_ptr;

void angleToRGB(float angle, GLfloat *color);
void HSLToRGB(float h, float s, float l, GLfloat *color);

class GLWidget : public QGLWidget
{
  Q_OBJECT

  public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void setXCom(float val);
  void setYCom(float val);
  void setZCom(float val);
  void setXLoc(float val);
  void setYLoc(float val);
  void setZLoc(float val);
  void setXSliceLow(int low);
  void setYSliceLow(int low);
  void setZSliceLow(int low);
  void setXSliceHigh(int high);
  void setYSliceHigh(int high);
  void setZSliceHigh(int high);
  void updateCOM();
  void updateExtent();
  void updateData(array_ptr data);
  void updateHeader(header_ptr header, array_ptr data);
//  void updateTopOverlay(QString newstring);
  void toggleDisplay(int cubes);
  void setBackgroundColor(QColor color);

signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);
  void COMChanged(float val);

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:
  int valuedim; // scalar or vector

  int xRot;
  int yRot;
  int zRot;
  float xLoc;
  float yLoc;
  float zLoc;
  float zoom;

  QPoint lastPos;
  QColor qtGreen;
  QColor qtPurple;
  QColor backgroundColor;

  GLuint cone, cube, vector;
  int drawCubes;
  // For storing the magnetization vectors
  // we only need float32 since double precision
  // is not necessary for rendering

  int   numSpins;
  float locations[1000][3];
  float spins[1000][3];

  // center of mass coordinates
  float xcom;
  float ycom;
  float zcom;

  // max extent
  float xmax, xmin;
  float ymax, ymin;
  float zmax, zmin;

  // Max Values
  float maxmag, minmag;

  // slice variables
  int xSliceLow, xSliceHigh;
  int ySliceLow, ySliceHigh;
  int zSliceLow, zSliceHigh;
  
  // pointer to relevant data
  array_ptr dataPtr;
  bool usePtr;
  bool displayOn;
//  bool topOverlayOn;

  // Overpainting
//  void drawInstructions(QPainter *painter);
//  QString topOverlayText;
};

#endif
