# General libraries and includes\
# libqxt works well when installed from source
# but is quite finnicky in, e.g., ubuntu package manager

QT      += opengl
LIBS    += -lglut -Llibomf -lomf -lGLU 

INCLUDEPATH += libomf

# Files and Targets
HEADERS = glwidget.h window.h \
    preferences.h qxtspanslider.h qxtspanslider_p.h analysis.h \
    aboutdialog.h
SOURCES = glwidget.cpp main.cpp window.cpp \ 
    preferences.cpp qxtspanslider.cpp analysis.cpp \
    aboutdialog.cpp
FORMS += \
    preferences.ui \
    window.ui \
    aboutdialog.ui
