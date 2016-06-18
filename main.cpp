#include "imageviewer.h"
#include <QApplication>
#include <QGuiApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    ImageViewer imageViewer;
    imageViewer.show();
    return app.exec();
}
