#include "imageviewer.h"
#include <QtWidgets>

const unsigned int DISPLAY_IMAGE_WIDTH = 640;
const unsigned int DISPLAY_IMAGE_HEIGHT = 480;

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent),
      m_pOriginImage(NULL),
      m_pImageLabel(new QLabel(this)),
      m_ButtonOpenImage("OpenImage", this),
      m_ButtonTest("Test", this)
{
    setFixedSize(800, 600);
    m_pImageLabel->setGeometry(10, 10, DISPLAY_IMAGE_WIDTH, DISPLAY_IMAGE_HEIGHT);
    m_pImageLabel->setBackgroundRole(QPalette::Base);

    m_ButtonOpenImage.setGeometry(660, 10, 120, 30);
    QObject::connect(&m_ButtonOpenImage, SIGNAL(clicked()), this, SLOT(openImage()));

    m_ButtonTest.setGeometry(660, 50, 120, 30);
    QObject::connect(&m_ButtonTest, SIGNAL(clicked()), this, SLOT(test()));
}

ImageViewer::~ImageViewer()
{
    delete m_pImageLabel;
    if (m_pOriginImage != NULL)
        delete m_pOriginImage;
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

    QImage scaledImage = m_pOriginImage->scaled(DISPLAY_IMAGE_WIDTH, DISPLAY_IMAGE_HEIGHT, Qt::KeepAspectRatio);
    m_pImageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    setWindowFilePath(fileName);

    return true;
}

void ImageViewer::test()
{
    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                             tr("This is Test"));
}

