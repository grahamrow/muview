#include <deque>
#include <iostream>

#include <QtGui>
#include <QDir>
//#include <QxtSpanSlider>
#include <QKeySequence>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSignalMapper>
#include <QDebug>
#include <QTimer>
#include <QMap>

#include "glwidget.h"
#include "window.h"
#include "ui_window.h"
#include "OMFImport.h"
#include "OMFHeader.h"
#include "OMFContainer.h"
#include "qxtspanslider.h"

struct OMFImport;

Window::Window(int argc, char *argv[]) :
    QMainWindow(NULL),
    ui(new Ui::Window)
{
//	QWidget *widget = new QWidget;
//	setCentralWidget(widget);

    ui->setupUi(this);

	// Cache size
	cacheSize = 50;
	cachePos  = 0;

//	ui->viewport = new ui->viewport;
	prefs = new Preferences(this);
    about = new AboutDialog(this);

    initSlider(ui->xSlider);
    initSlider(ui->ySlider);
    initSlider(ui->zSlider);
    initSpanSlider(ui->xSpanSlider);
    initSpanSlider(ui->ySpanSlider);
    initSpanSlider(ui->zSpanSlider);

	// Rotation
    connect(ui->xSlider,  SIGNAL(valueChanged(int)),     ui->viewport, SLOT(setXRotation(int)));
    connect(ui->viewport, SIGNAL(xRotationChanged(int)), ui->xSlider, SLOT(setValue(int)));
    connect(ui->ySlider,  SIGNAL(valueChanged(int)),     ui->viewport, SLOT(setYRotation(int)));
    connect(ui->viewport, SIGNAL(yRotationChanged(int)), ui->ySlider, SLOT(setValue(int)));
    connect(ui->zSlider,  SIGNAL(valueChanged(int)),     ui->viewport, SLOT(setZRotation(int)));
    connect(ui->viewport, SIGNAL(zRotationChanged(int)), ui->zSlider, SLOT(setValue(int)));

	// Slicing
    connect(ui->xSpanSlider, SIGNAL(lowerValueChanged(int)), ui->viewport, SLOT(setXSliceLow(int)));
    connect(ui->xSpanSlider, SIGNAL(upperValueChanged(int)), ui->viewport, SLOT(setXSliceHigh(int)));
    connect(ui->ySpanSlider, SIGNAL(lowerValueChanged(int)), ui->viewport, SLOT(setYSliceLow(int)));
    connect(ui->ySpanSlider, SIGNAL(upperValueChanged(int)), ui->viewport, SLOT(setYSliceHigh(int)));
    connect(ui->zSpanSlider, SIGNAL(lowerValueChanged(int)), ui->viewport, SLOT(setZSliceLow(int)));
    connect(ui->zSpanSlider, SIGNAL(upperValueChanged(int)), ui->viewport, SLOT(setZSliceHigh(int)));

//	QHBoxLayout *mainLayout = new QHBoxLayout;

//	sliceGroupBox = new QGroupBox(tr("XYZ Slicing"));
//	rotGroupBox   = new QGroupBox(tr("Rotation"));
//	sliceGroupBox->setAlignment(Qt::AlignHCenter);
//	rotGroupBox->setAlignment(Qt::AlignHCenter);

//	QVBoxLayout *displayLayout = new QVBoxLayout;
//	QHBoxLayout *sliceLayout   = new QHBoxLayout;
//	QHBoxLayout *rotLayout     = new QHBoxLayout;

	// Center display and animation bar
//	animLabel  = new QLabel(tr("<i>Animation</i> timeline"));
//	animLabel->setAlignment(Qt::AlignCenter);
//	ui->animSlider = new QSlider(Qt::Horizontal);
    ui->animSlider->setRange(0, 10);
    ui->animSlider->setSingleStep(1);
    ui->animSlider->setPageStep(10);
    ui->animSlider->setTickInterval(2);
    ui->animSlider->setTickPosition(QSlider::TicksRight);
    ui->animSlider->setEnabled(false);
//	animLabel->setFixedHeight(animLabel->sizeHint().height());
//	displayLayout->addWidget(ui->viewport);
//	displayLayout->addWidget(animLabel);
//	displayLayout->addWidget(ui->animSlider);

	// Slicing
//	sliceLayout->addWidget(ui->xSpanSlider);
//	sliceLayout->addWidget(ui->ySpanSlider);
//	sliceLayout->addWidget(ui->zSpanSlider);
//	sliceGroupBox->setLayout(sliceLayout);
//	sliceGroupBox->setFixedWidth(120);

	// Rotation
//	rotLayout->addWidget(ui->xSlider);
//	rotLayout->addWidget(ui->ySlider);
//	rotLayout->addWidget(ui->zSlider);
//	rotGroupBox->setLayout(rotLayout);
//	rotGroupBox->setFixedWidth(120);

	// Overall Layout
//	mainLayout->addWidget(rotGroupBox);
//	mainLayout->addLayout(displayLayout);
//	mainLayout->addWidget(sliceGroupBox);
//	widget->setLayout(mainLayout);

	// Main Window Related
    createActions();
//	createMenus();

    ui->xSlider->setValue(15 * 16);
    ui->ySlider->setValue(345 * 16);
    ui->zSlider->setValue(0 * 16);
	setWindowTitle(tr("MuView 0.9"));

	// Data, don't connect until we are ready (probably still not ready here)...
    connect(ui->animSlider, SIGNAL(valueChanged(int)), this, SLOT(updateDisplayData(int)));
	
	// Load files from command line if supplied
	if (argc > 1) {
		QStringList rawList;
		for (int i=1; i<argc; i++) {
			rawList << argv[i];
		}

		if (rawList.contains(QString("-w"))) {
			if (rawList.indexOf("-w") < (rawList.length() - 1))  {
				watchDir(rawList[rawList.indexOf("-w")+1]);
			}
		} else {
			QStringList allLoadedFiles;
			foreach (QString item, rawList)
			{
				QFileInfo info(item);
				if (!info.exists()) {
					std::cout << "File " << item.toStdString() << " does not exist" << std::endl;
				} else {
								// Push our new content...
					if (info.isDir()) {
						QDir chosenDir(item);
						dirString = chosenDir.path()+"/";
						QStringList filters;
						filters << "*.omf" << "*.ovf";
						chosenDir.setNameFilters(filters);
						QStringList files = chosenDir.entryList();

						foreach (QString file, files)
						{
							filenames << (dirString+file);
							displayNames << (dirString+item);
							//omfCache.push_back(readOMF((dirString+file).toStdString(), tempHeader));
						}

					} else {
						// just a normal file
						filenames << (dirString+item);
						displayNames << (dirString+item);
						//omfCache.push_back(readOMF(item.toStdString(), tempHeader));
					}
				}
			}

			processFilenames();
			gotoFrontOfCache();
            adjustAnimSlider(false);  // Refresh the animation bar
		}
	}
}

void Window::initSpanSlider(QxtSpanSlider *slider)
{
//	QxtSpanSlider *spanSlider = new QxtSpanSlider(Qt::Vertical);
    slider->setRange(0 *16, 100 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);
    slider->setLowerValue(0*16);
    slider->setUpperValue(100*16);
}

void Window::initSlider(QSlider *slider)
{
//	QSlider *slider = new QSlider(Qt::Vertical);
	slider->setRange(0, 360 * 16);
	slider->setSingleStep(16);
	slider->setPageStep(15 * 16);
	slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
}

void Window::adjustAnimSlider(bool back)
{
	int numFiles = filenames.size();
	//qDebug() << QString("Updating Animation Slider to size") << numFiles;
	if (numFiles > 1) {
        ui->animSlider->setRange(0, numFiles-1);
        ui->animSlider->setSingleStep(1);
        ui->animSlider->setPageStep(10);
        ui->animSlider->setTickInterval(2);
        ui->animSlider->setTickPosition(QSlider::TicksRight);
        ui->animSlider->setEnabled(true);
		if (back) {
            ui->animSlider->setSliderPosition(numFiles-1);
		} else {
            ui->animSlider->setSliderPosition(0);
		}
	} else {
        ui->animSlider->setEnabled(false);
	}
	//qDebug() << QString("Updated Animation Slider to size") << numFiles;
}

void Window::keyPressEvent(QKeyEvent *e)
{
	if (e->modifiers() == Qt::CTRL) {
	// Close on Ctrl-Q or Ctrl-W
		if (e->key() == Qt::Key_Q || e->key() == Qt::Key_W )
			close();
	} else if (e->modifiers() == Qt::SHIFT) {
		if (e->key() == Qt::Key_Q || e->key() == Qt::Key_W )
			close();
	} else {
		if (e->key() == Qt::Key_Escape)
			close();
		else
			QWidget::keyPressEvent(e);
	}
}

//void Window::createMenus()
//{
//	fileMenu = menuBar()->addMenu(tr("&File"));
//	fileMenu->addAction(ui->actionFiles);
//	fileMenu->addAction(ui->actionDir);
//	fileMenu->addAction(ui->actionFollow);
//	fileMenu->addSeparator();

//	settingsMenu = menuBar()->addMenu(tr("&Settings"));

//	helpMenu = menuBar()->addMenu(tr("&Help"));
//	helpMenu->addAction(actionAbout);
//	helpMenu->addSeparator();
//	//helpMenu->addAction(webAct);

//	settingsMenu->addAction(actionSettings);
//	settingsMenu->addSeparator();
//	settingsMenu->addAction(ui->actionCubes);
//	settingsMenu->addAction(ui->actionCones);
//	settingsMenu->addAction(ui->actionVectors);
//}

//void Window::about()
//{
//	//infoLabel->setText(tr("Invoked <b>Help|About</b>"));
//	QMessageBox::about(this, tr("About Muview"),
//											tr("<b>Muview</b> 0.9 \n<br>"
//												"Mumax visualization tool written in OpenGL and Qt<br>"
//												"<br>Created by Graham Rowlands 2014."));
//}

void Window::openSettings()
{
	prefs->exec();
}

void Window::openAbout()
{
    about->exec();
}

void Window::processFilenames() {
	// Looping over files
	for (int loadPos=0; loadPos<cacheSize && loadPos<filenames.size(); loadPos++) {
		header_ptr header = header_ptr(new OMFHeader());
		omfHeaderCache.push_back(header);
		omfCache.push_back(readOMF((filenames[loadPos]).toStdString(), *header));
	}
}

void Window::gotoFrontOfCache() {
    ui->viewport->updateHeader(omfHeaderCache.front(), omfCache.front());
    ui->viewport->updateTopOverlay(filenames.front());
    ui->viewport->updateData(omfCache.front());
	cachePos = 0;
    adjustAnimSlider(false); // Go to end of slider
}

void Window::gotoBackOfCache() {
    ui->viewport->updateHeader(omfHeaderCache.back(), omfCache.back());
    ui->viewport->updateTopOverlay(filenames.back());
    ui->viewport->updateData(omfCache.back());
	cachePos = filenames.size()-1;
    adjustAnimSlider(true); // Go to start of slider
}

void Window::clearHeaderCache() {
	while (!omfHeaderCache.empty()) {
		omfHeaderCache.pop_back();
	}
}

void Window::clearOMFCache() {
	while (!omfCache.empty()) {
		omfCache.pop_back();
	}
}

void Window::openFiles()
{
	QString fileName;
	fileName = QFileDialog::getOpenFileName(this,
		tr("Open File"), QDir::currentPath(),
		tr("OVF Files (*.omf *.ovf)"));
	
	if (fileName != "") 
	{
		filenames.clear();
		filenames.push_back(fileName);
		clearOMFCache();
		clearHeaderCache();
		processFilenames();
		gotoBackOfCache();
	}
}

void Window::updateWatchedFiles(const QString& str) {
	// Look at all of the files in the directory
	// and add those which are not in the list of
	// original filenames

	// filenames contains the list of loaded files
	// watchedFiles is a map of files to load and their modification timestamps

	// When the timestamps in wathcedFiles stop changing we actually
	// push the relevant files into the OMF cache.

	QDir chosenDir(str);
	QString dirString = chosenDir.path()+"/";
	QStringList filters;
	filters << "*.omf" << "*.ovf";
	chosenDir.setNameFilters(filters);
	QStringList dirFiles = chosenDir.entryList();

	OMFHeader tempHeader = OMFHeader();

	// compare to existing list of files
	bool changed = false;
	foreach(QString dirFile, dirFiles)
	{
		//qDebug() << QString("Changed File!") << dirFile;
		if (!filenames.contains(dirString + dirFile)) {
			// Haven't been loaded
			QString fullPath = dirString + dirFile;
			QFileInfo info(fullPath);
			//qDebug() << QString("Not in filenames: ") << (dirFile);

			if (!watchedFiles.contains(fullPath)) {
				// Not on the watch list yet
				watchedFiles.insert(fullPath, info.lastModified());
				//qDebug() << QString("Inserted: ") << (dirFile);
			} else {
				// on the watch list
				if (info.lastModified() == watchedFiles[fullPath]) {
					// File size has stabalized
					//qDebug() << QString("Stable, pushed") << dirFile;
					filenames.append(fullPath);
					displayNames.append(dirFile);
					//omfCache.push_back(readOMF(fullPath.toStdString(), tempHeader));
					changed = true;
				} else {
					// File still changing
					//qDebug() << QSt        //qDebug() << QString("Pushing Back") << filenames[loadPos];ring("Unstable") << dirFile;
					watchedFiles[fullPath] = info.lastModified();
				}
			}
		}
	}


	if (changed) {
		clearOMFCache();
		clearHeaderCache();
		processFilenames();
		gotoBackOfCache();
	}

}

void Window::updateDisplayData(int index)
{
	// Check to see if we've cached this data already.
	// Add and remove elements from the front and back
	// of the deque until we've caught up... if we're
	// too far out of range just scratch everything and
	// reload.

	if ( abs(index-cachePos) >= cacheSize ) {
			// Out of the realm of caching
			// Clear the cache of pre-existing elements
			//qDebug() << QString("Clearing the cache, too far out of range") << index << cachePos;
		clearOMFCache();
		clearHeaderCache();
		cachePos = index;
		for (int loadPos=index; loadPos<(index+cacheSize) && loadPos<filenames.size(); loadPos++) {
			header_ptr header = header_ptr(new OMFHeader());
			omfHeaderCache.push_back(header);
			omfCache.push_back(readOMF((filenames[loadPos]).toStdString(), *header));
		}
		cachePos = index;
	} else if ( index < cachePos ) {
			// Moving backwards, regroup for fast scrubbing!
			//qDebug() << QString("Moving backwards") << index << cachePos;
		for (int loadPos=cachePos-1; loadPos >= index && loadPos<filenames.size(); loadPos--) {
			if (omfCache.size()==uint(cacheSize)) {
				omfCache.pop_back();
				omfHeaderCache.pop_back();
			} 
			header_ptr header = header_ptr(new OMFHeader());
			omfHeaderCache.push_front(header);
			omfCache.push_front(readOMF((filenames[loadPos]).toStdString(), *header));
			
		}
		cachePos = index;
	}

	// We are within the current cache
	if (index < filenames.size()) {
		//qDebug() << QString("In Cache Range") << index << cachePos;
		// Update the top overlay
        ui->viewport->updateTopOverlay(displayNames[index]);
		// Update the Display
		//qDebug() << QString("Current cache size") << omfCache.size();
        ui->viewport->updateHeader(omfHeaderCache.at(index-cachePos), omfCache.at(index-cachePos));
        ui->viewport->updateData(omfCache.at(index-cachePos));
	} else {
			//qDebug() << QString("Out of Cache Range!!!!") << index << cachePos;
        ui->viewport->updateTopOverlay(QString("Don't scroll so erratically..."));
	}
}

void Window::openDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	
	if (dir != "") 
	{
		QDir chosenDir(dir);
		dirString = chosenDir.path()+"/";
		QStringList filters;
		filters << "*.omf" << "*.ovf";
		chosenDir.setNameFilters(filters);
		QStringList dirFiles = chosenDir.entryList();
		filenames.clear();
		foreach (QString file, dirFiles) {
			filenames.push_back(dirString + file);
		}

		// persistent storage of filenames for top overlay
		displayNames = dirFiles;

		clearOMFCache();
		clearHeaderCache();
		processFilenames();
		gotoBackOfCache();

	}
}

void Window::watchDir(const QString& str)
{
	QString dir;
		// Don't show a dialog if we get this message from the command line
	if (str == "") {
		dir = QFileDialog::getExistingDirectory(this, tr("Watch Directory"),
			"/home",
			QFileDialog::ShowDirsOnly
			| QFileDialog::DontResolveSymlinks);
	} else {
		dir = str;
	}

	filenames.clear();
	displayNames.clear();
	clearOMFCache();

	cachePos = 0; // reset position to beginning

	if (dir != "")
	{
		// Added the dir to the watch list
		watcher = new QFileSystemWatcher();
		//waiter = new QFileSystemWatcher();
		watcher->addPath(dir);

		// Now read all of the current files
		QDir chosenDir(dir);
		QString dirString = chosenDir.path()+"/";
		QStringList filters;
		filters << "*.omf" << "*.ovf";
		chosenDir.setNameFilters(filters);
		QStringList dirFiles = chosenDir.entryList();

		// persistent storage of filenames for top overlay
		displayNames = dirFiles;

		foreach (QString file, dirFiles) {
			filenames.push_back(dirString + file);
		}

		if (filenames.length()>0) {

			// Only cache the last file
			header_ptr header = header_ptr(new OMFHeader());
			cachePos = filenames.size()-1;
			omfHeaderCache.push_back(header);
			omfCache.push_back(readOMF(filenames.last().toStdString(), *header));

			// Update the Display with the first element
            ui->viewport->updateHeader(omfHeaderCache.back(), omfCache.back());
            ui->viewport->updateData(omfCache.back());
			
			// Update the top overlay
            ui->viewport->updateTopOverlay(displayNames.back());

			// Refresh the animation bar
            adjustAnimSlider(true);
		}
		// Now the callbacks
		QObject::connect(watcher, SIGNAL(directoryChanged(QString)),
			this, SLOT(updateWatchedFiles(QString)));
	}

}

void Window::toggleDisplay() {
    if (ui->actionCubes->isChecked()) {
        ui->viewport->toggleDisplay(0);
    } else if (ui->actionCones->isChecked()) {
        ui->viewport->toggleDisplay(1);
	} else {
        ui->viewport->toggleDisplay(2);
	}
}


void Window::createActions()
{
//    actionAbout = new QAction(tr("&About Muview"), this);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAbout()));

//    actionSettings = new QAction(tr("&Muview Preferences"), this);
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(openSettings()));

//    ui->actionCubes   = new QAction(tr("&Display Cubes"), this);
//    ui->actionCones   = new QAction(tr("&Display Cones"), this);
//    ui->actionVectors = new QAction(tr("&Display Vectors"), this);
//    ui->actionCubes->setCheckable(true);
    ui->actionCubes->setChecked(true);
//    ui->actionCones->setCheckable(true);
//    ui->actionVectors->setCheckable(true);
    connect(ui->actionCubes,   SIGNAL(triggered()), this, SLOT(toggleDisplay()));
    connect(ui->actionCones,   SIGNAL(triggered()), this, SLOT(toggleDisplay()));
    connect(ui->actionVectors, SIGNAL(triggered()), this, SLOT(toggleDisplay()));
    displayType = new QActionGroup(this);
    displayType->addAction(ui->actionCubes);
    displayType->addAction(ui->actionCones);
    displayType->addAction(ui->actionVectors);

//    ui->actionFiles  = new QAction(tr("&Open File(s)"), this);
//	ui->actionFiles->setShortcuts(QKeySequence::Open);
    connect(ui->actionFiles, SIGNAL(triggered()), this, SLOT(openFiles()));

//    ui->actionDir  = new QAction(tr("&Open Dir"), this);
//	ui->actionDir->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_D) );
    connect(ui->actionDir, SIGNAL(triggered()), this, SLOT(openDir()));

    signalMapper = new QSignalMapper(this);
//    ui->actionFollow  = new QAction(tr("&Follow Dir"), this);
//	ui->actionFollow->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_F) );
    connect(ui->actionFollow, SIGNAL(triggered()), signalMapper, SLOT(map()));

    signalMapper->setMapping (ui->actionFollow, "") ;
	connect (signalMapper, SIGNAL(mapped(QString)), this, SLOT(watchDir(QString))) ;
}

Window::~Window()
{
    delete ui;
}
