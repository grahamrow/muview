#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <boost/smart_ptr.hpp>

// Other parts of the interface
#include "preferences.h"
#include "aboutdialog.h"

// For reading OMF files
#include "OMFContainer.h"
#include "OMFHeader.h"
#include <deque>

typedef boost::shared_ptr<OMFHeader> header_ptr;

// Forward Definitions
class QSlider;
//class GLWidget;
class QxtSpanSlider;
class QSignalMapper;
//class QGroupBox;

// Main Window Stuff
//class QAction;
class QActionGroup;
//class QLabel;
//class QMenu;

// Other
class QFileSystemWatcher;

namespace Ui {
    class Window;
}

class Window : public QMainWindow
{
  Q_OBJECT

public:
  explicit Window(int argc, char *argv[]);
  ~Window();

protected:
  void keyPressEvent(QKeyEvent *event);
  //void contextMenuEvent(QContextMenuEvent *event);

private slots:
  void openFiles();
  void openDir();
  void watchDir(const QString& str);
  void toggleDisplay();
  void updateWatchedFiles(const QString& str);
  void openSettings();
  void openAbout();
  void updateDisplayData(int index);
  void updatePrefs();

private:
  // Main Window Stuff
  Ui::Window *ui;
//  void createActions();
//  void createMenus();
  void adjustAnimSlider(bool back);

//  QMenu *fileMenu;
//  QMenu *settingsMenu;
//  QMenu *helpMenu;
  
//  QAction *openFilesAct;
//  QAction *openDirAct;
//  QAction *attachToMumax;
//  QAction *settingsAct;
//  QAction *aboutAct;
//  QAction *watchDirAct;

  QActionGroup *displayType;
//  QAction *conesAct;
//  QAction *cubesAct;
//  QAction *vectorsAct;
  
//  QAction *scaleByMagnitude;
  //QAction *webAct;

  // Preferences window;
  Preferences *prefs;
  AboutDialog *about;

  // Convenience Functions for Sliders
  void initSlider(QSlider *slider);
  void initSpanSlider(QxtSpanSlider *slider);

//  QGroupBox *sliceGroupBox;
//  QGroupBox *rotGroupBox;

//  GLWidget *glWidget;

//  QSlider *xSlider;
//  QSlider *ySlider;
//  QSlider *zSlider;

//  QSlider *animSlider;
//  QLabel *animLabel;

//  QxtSpanSlider *xSpanSlider;
//  QxtSpanSlider *ySpanSlider;
//  QxtSpanSlider *zSpanSlider;
  
  // ============================================================
  // Storage and caching
  // A finite number of files, as governed by cacheSize, will
  // reside in memory at a given time, otherwise we will choke
  // the system on large output directories.
  // ============================================================

  int cacheSize; // Maxmimum cache size
  int cachePos;  // Current location w.r.t list of all filenames

  void clearOMFCache();
  void clearHeaderCache();
  void gotoBackOfCache();
  void gotoFrontOfCache();
  void processFilenames();

  std::deque<array_ptr>  omfCache;
  std::deque<header_ptr> omfHeaderCache;
  QStringList filenames;
  QStringList displayNames;

  QString dirString;

  // Watch dirs
  QSignalMapper* signalMapper;
  QFileSystemWatcher *watcher;
  QMap<QString, QDateTime> watchedFiles;
  bool noFollowUpdate;
};

#endif
