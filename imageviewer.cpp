#include "imageviewer.h"
#include <QtWidgets>

const unsigned int BUTTON_WIDTH = 120;

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent),
      m_pOriginImage(NULL),
      m_pImageLabel(new QLabel(this)),
      m_ButtonOpenImage("OpenImage", this),
      m_ButtonTest("Test", this),
      m_ButtonDefaultView("DefaultView", this),
      m_ButtonFullView("FullView", this),
      m_ButtonExit("Exit", this)
{
    showDefaultView();
    m_pImageLabel->setBackgroundRole(QPalette::Base);

    QObject::connect(&m_ButtonOpenImage, SIGNAL(clicked()), this, SLOT(openImage()));
    QObject::connect(&m_ButtonTest, SIGNAL(clicked()), this, SLOT(test()));
    QObject::connect(&m_ButtonFullView, SIGNAL(clicked()), this, SLOT(showFullView()));
    QObject::connect(&m_ButtonDefaultView, SIGNAL(clicked()), this, SLOT(showDefaultView()));
    QObject::connect(&m_ButtonExit, SIGNAL(clicked()), this, SLOT(exit()));
}

ImageViewer::~ImageViewer()
{
    delete m_pImageLabel;
    if (m_pOriginImage != NULL)
        delete m_pOriginImage;
}

void ImageViewer::updateButtonPosition()
{
    int x = frameSize().width() - BUTTON_WIDTH;
    m_ButtonOpenImage.setGeometry(x, 10, BUTTON_WIDTH, 30);
    m_ButtonTest.setGeometry(x, 50, BUTTON_WIDTH, 30);
    m_ButtonDefaultView.setGeometry(x, 90, BUTTON_WIDTH, 30);
    m_ButtonFullView.setGeometry(x, 90, BUTTON_WIDTH, 30);
    m_ButtonExit.setGeometry(x, 130, BUTTON_WIDTH, 30);

    m_ButtonDefaultView.setVisible(!m_bDefaultView);
    m_ButtonFullView.setVisible(m_bDefaultView);
}

static void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;
    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes =
            acceptMode == QFileDialog::AcceptOpen ?
                QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::openImage()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadImageFile(dialog.selectedFiles().first())) {}
}

bool ImageViewer::loadImageFile(const QString& fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);

    if (m_pOriginImage != NULL)
        delete m_pOriginImage;
    m_pOriginImage = new QImage;
    bool ret = reader.read(m_pOriginImage);
    if (ret == false) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(fileName),
                                                              reader.errorString()));
        return false;
    }

    if (m_bDefaultView) {
        showDefaultView();
    } else {
        showFullView();
    }
    setWindowFilePath(fileName);

    return true;
}

void ImageViewer::test()
{
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                             tr("This is Test"));
}

void ImageViewer::exit()
{
    QApplication::quit();
}

void ImageViewer::showFullView()
{
    m_bDefaultView = false;
    setWindowState(Qt::WindowFullScreen);

    QRect rect = QApplication::desktop()->screenGeometry();
    unsigned int w = rect.width();
    unsigned int h = rect.height();
    setFixedSize(w, h);
    m_pImageLabel->setGeometry(0, 0, w, h);
    updateButtonPosition();

    if (m_pOriginImage) {
        QImage scaledImage = m_pOriginImage->scaled(w, h, Qt::KeepAspectRatio);
        m_pImageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    }
}

void ImageViewer::showDefaultView()
{
    m_bDefaultView = true;
    showNormal();

    QRect rect = QApplication::desktop()->screenGeometry();
    unsigned int w = rect.width() / 2;
    unsigned int h = rect.height() / 2;
    setFixedSize(w, h);
    m_pImageLabel->setGeometry(0, 0, w - BUTTON_WIDTH, h);
    updateButtonPosition();

    if (m_pOriginImage) {
        QImage scaledImage = m_pOriginImage->scaled(w - BUTTON_WIDTH, h, Qt::KeepAspectRatio);
        m_pImageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    }
}
