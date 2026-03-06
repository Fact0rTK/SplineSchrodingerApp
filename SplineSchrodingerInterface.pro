QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog_result.cpp \
    dialog_savefile.cpp \
    dialog_settings.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_TextBoxes.cpp \
    mainwindows_BaseFunctions.cpp \
    mainwindows_Potential.cpp \
    qcustomplot.cpp \
    splinetwobody.cpp \
    splinetwobody_eigen.cpp

HEADERS += \
    complex_type.hpp \
    complex_type.hpp \
    dialog_result.h \
    dialog_savefile.h \
    dialog_settings.h \
    exprtk.hpp \
    exprtk.hpp \
    exprtk_complex_adaptor.hpp \
    exprtk_complex_adaptor.hpp \
    mainwindow.h \
    qcustomplot.h \
    splinetwobody.h \
    splinetwobody_eigen.h

FORMS += \
    dialog_result.ui \
    dialog_savefile.ui \
    dialog_settings.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#QMAKE_CXXFLAGS += -Wa,-mbig-obj

#QMAKE_LFLAGS += -Wl, -rpath "'$$ORIGIN'"

RESOURCES += \
    resource.qrc
