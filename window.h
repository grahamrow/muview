#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QDateTime>

// Other parts of the interface
#include "preferences.h"

// For reading OMF files
#include "OMFContainer.h"
#include <deque>

// General widget stuff 
class QSlider;
class GLWidget;
class QxtSpanSlider;
class QGroupBox;

// Main Window Stuff
class QAction;
class QActionGroup;
class QLabel;
class QMenu;

// Other
class QFileSystemWatcher;
//class QString;
// class QDateTime;

class Window : public QMainWindow
{
  Q_OBJECT

  public:
  Window(int argc, char *argv[]);

protected:
  void keyPressEvent(QKeyEvent *event);
  //void contextMenuEvent(QContextMenuEvent *event);

private slots:
  void openFiles();
  void openDir();
  void watchDir(const QString& str);
  void toggleDisplay();
  void updateWatchedFiles(const QString& str);
  void settings();
  void about();
  void updateDisplayData(int index);
  
private:
  // Main Window Stuff
  void createActions();
  void createMenus();
  void adjustAnimSlider(bool back);

  QMenu *fileMenu;
  QMenu *settingsMenu;
  QMenu *helpMenu;
  
  QAction *openFilesAct;
  QAction *openDirAct;
  QAction *attachToMumax;
  QAction *settingsAct;
  QAction *aboutAct;
  QAction *watchDirAct;

  QActionGroup *displayType;
  QAction *conesAct;
  QAction *cubesAct;
  QAction *vectorsAct;
  
  QAction *scaleByMagnitude;
  //QAction *webAct;

  // Preferences window;
  Preferences *prefs;

  // Other Stuff
  QSlider *createSlider();
  QxtSpanSlider *createSpanSlider();

  QGroupBox *sliceGroupBox;
  QGroupBox *rotGroupBox;

  GLWidget *glWidget;

  QSlider *xSlider;
  QSlider *ySlider;
  QSlider *zSlider;

  QSlider *animSlider;
  QLabel *animLabel;

  QxtSpanSlider *xSpanSlider;
  QxtSpanSlider *ySpanSlider;
  QxtSpanSlider *zSpanSlider;
  
  // Storage and caching
  // A finite number of files, as governed by cacheSize, will
  // reside in memory at a given time, otherwise we will choke
  // the system on large output directories.
  int cacheSize; // Maxmimum cache size
  int cachePos;  // Current location w.r.t list of all filenames
  std::deque<array_ptr> omfCache;
  // std::deque<int>       cacheFileDim; // The dimensionality of the data. Mostly 1 (scalar) or 3 (vector)
  QStringList filenames;
  QStringList displayNames;

  QString dirString;

  // Watch dirs
  QFileSystemWatcher *watcher;
  QMap<QString, QDateTime> watchedFiles;
  bool noFollowUpdate;
};

#endif
