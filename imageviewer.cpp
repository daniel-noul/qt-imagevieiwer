#include "imageviewer.h"
#include <QtWidgets>

const unsigned int DISPLAY_IMAGE_WIDTH = 640;
const unsigned int DISPLAY_IMAGE_HEIGHT = 480;

const unsigned int DISPLAY_FULL_IMAGE_WIDTH = 1920;
const unsigned int DISPLAY_FULL_IMAGE_HEIGHT = 1080;

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent),
      m_pOriginImage(NULL),
      m_pImageLabel(new QLabel(this)),
      m_ButtonOpenImage("OpenImage", this),
      m_ButtonTest("Test", this),
      m_ButtonDefaultView("DefaultView", this),
      m_ButtonFullView("FullView", this)
{
    setDefaultView();
    m_pImageLabel->setBackgroundRole(QPalette::Base);

    QObject::connect(&m_ButtonOpenImage, SIGNAL(clicked()), this, SLOT(openImage()));
    QObject::connect(&m_ButtonTest, SIGNAL(clicked()), this, SLOT(test()));
    QObject::connect(&m_ButtonFullView, SIGNAL(clicked()), this, SLOT(showFullView()));
    QObject::connect(&m_ButtonDefaultView, SIGNAL(clicked()), this, SLOT(showDefaultView()));
}

ImageViewer::~ImageViewer()
{
    delete m_pImageLabel;
    if (m_pOriginImage != NULL)
        delete m_pOriginImage;
}

void ImageViewer::setDefaultView()
{
    m_bDefaultView = true;
    showNormal();
    setFixedSize(800, 600);
    m_pImageLabel->setGeometry(10, 10, DISPLAY_IMAGE_WIDTH, DISPLAY_IMAGE_HEIGHT);

    updateButtonPosition();
}

void ImageViewer::setFullView()
{
    m_bDefaultView = false;
    setWindowState(Qt::WindowFullScreen);
    setFixedSize(DISPLAY_FULL_IMAGE_WIDTH, DISPLAY_FULL_IMAGE_HEIGHT);
    m_pImageLabel->setGeometry(0, 0, DISPLAY_FULL_IMAGE_WIDTH, DISPLAY_FULL_IMAGE_HEIGHT);

    updateButtonPosition();
}

void ImageViewer::updateButtonPosition()
{
    int x = frameSize().width() - 120;
    m_ButtonOpenImage.setGeometry(x, 10, 120, 30);
    m_ButtonTest.setGeometry(x, 50, 120, 30);
    m_ButtonDefaultView.setGeometry(x, 90, 120, 30);
    m_ButtonFullView.setGeometry(x, 90, 120, 30);
    if (m_bDefaultView) {
        m_ButtonDefaultView.setVisible(false);
        m_ButtonFullView.setVisible(true);
    } else {
        m_ButtonDefaultView.setVisible(true);
        m_ButtonFullView.setVisible(false);
    }
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

void ImageViewer::showFullView()
{
    setFullView();
    if (m_pOriginImage) {
        QImage scaledImage = m_pOriginImage->scaled(DISPLAY_FULL_IMAGE_WIDTH, DISPLAY_FULL_IMAGE_HEIGHT, Qt::KeepAspectRatio);
        m_pImageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    }
}

void ImageViewer::showDefaultView()
{
    setDefaultView();
    if (m_pOriginImage) {
        QImage scaledImage = m_pOriginImage->scaled(DISPLAY_IMAGE_WIDTH, DISPLAY_IMAGE_HEIGHT, Qt::KeepAspectRatio);
        m_pImageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    }
}
