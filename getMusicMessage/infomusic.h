#ifndef INFOMUSIC_H
#define INFOMUSIC_H

#include <QObject>
#include <QQuickImageProvider>
#include <QtCore>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}
class InfoMusic;
// 提供　与　ｐｒｏｖｉｄｅｒ使用　将数据解析开来
class InfoMusic : public QObject
{
    Q_OBJECT

public:
    struct Information
    {
        QString musicName;
        QString author;
        QString album;
        QString duration;
        QString filePath;
    };
    using InfoMusicType = Information;
    //  读取信息资源的时候　将　音乐的信息转出到数据库中使用
    explicit InfoMusic(QString filePath, QObject *parent = nullptr);
    ~InfoMusic() = default;
    QPixmap getResouce() const;
    QString CoveImage = "image://CodeImg/image.png"; // 最初的文件路径
signals:
    void urlChanged(QUrl);
    //void iamgeChanged(QUrl &);
    // 成功时将触发此信号
    void getInformation(const InfoMusicType data);
    //获得封面
    void getCoverImage(QPixmap resouce);
    // void errorResource(const QString &);
public slots:
    Q_INVOKABLE void setUrl(QString filePath);

    void getMessage(); //  emit getInformation when success

private:
    QPixmap resouce; // 封面信息
    QString url;
    inline std::string getResourceFromQrc(const QString &qrcFile)
    {
        QUrl dir{qrcFile};
        QDir assist;
        return assist.absoluteFilePath(dir.fileName()).toStdString();
    }
    inline std::string getResourceFromFileSystem(const QString &url)
    {
        return url.toString().toStdString();
    }
};

#endif // INFOMUSIC_H
