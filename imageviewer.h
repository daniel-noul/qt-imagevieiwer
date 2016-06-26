#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QPushButton>

class QImage;
class QLabel;

class ImageViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

signals:

public slots:
    void openImage();
    void test();
    void showDefaultView();
    void showFullView();
    void exit();

private:
    QImage* m_pOriginImage;
    QLabel* m_pImageLabel;
    QPushButton m_ButtonOpenImage;
    QPushButton m_ButtonTest;
    QPushButton m_ButtonDefaultView;
    QPushButton m_ButtonFullView;
    QPushButton m_ButtonExit;
    bool m_bDefaultView;

    bool loadImageFile(const QString& fileName);
    void setImage(const QImage& newImage);

    void setDefaultView();
    void setFullView();
    void updateButtonPosition();
};

#endif // IMAGEVIEWER_H
