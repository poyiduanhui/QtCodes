#include "imageprovider.h"

ImageProvider::ImageProvider(QString filePath,
                             QObject *parent,
                             QQmlImageProviderBase::ImageType type,
                             QQmlImageProviderBase::Flags flags)
    : QQuickImageProvider(type, flags), QObject(parent)
{
    connect(this, &ImageProvider::urlChanged, this, &ImageProvider::getMessage);
    this->url = std::move(filePath);
}

void ImageProvider::getMessage()
{
    isSuccess = false;
    std::string imagePath;
    if (url.startsWith("qrc") || url.startsWith(":")) {
        imagePath = this->getResourceFromQrc(url);
    } else {
        if (QUrl{url}.isValid()) {
            imagePath = this->getResourceFromFileSystem(url);
        } else {
            qDebug() << "非法路径";
            return;
        }
    }
    AVFormatContext *fmt_ctx = nullptr;
    AVDictionaryEntry *tag = nullptr;
    int ret = 0;
    qDebug() << "当前的资源为" << imagePath.c_str();
    if ((ret = avformat_open_input(&fmt_ctx, imagePath.c_str(), nullptr, nullptr))) {
        qDebug() << "Fail to open file";
        return;
    } else {
        avformat_find_stream_info(fmt_ctx, nullptr);
        // 读取为秒 //将此数据传到 数据库中去
        QMap<QString, QString> maps;
        //读取metadata中所有的tag
        while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            maps.insert(tag->key, tag->value);
        }
        if (fmt_ctx->iformat->read_header(fmt_ctx) < 0) {
            printf("No header format");
            return;
        }

        for (size_t i = 0; i < fmt_ctx->nb_streams; i++) {
            if (fmt_ctx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                AVPacket pkt = fmt_ctx->streams[i]->attached_pic;
                //使用QImage读取完整图片数据（注意，图片数据是为解析的文件数据，需要用QImage::fromdata来解析读取）
                QImage imge = QImage::fromData(reinterpret_cast<uchar *>(pkt.data), pkt.size);
                //                qDebug() << "是否为空" << img.isNull(); // 不为空 ... 我擦
                if (!imge.isNull()) {
                    this->setImg(QPixmap::fromImage(std::move(imge))); // 获得图像
                    qDebug() << "读取图片成功" /*<< !this->img.isNull()*/;
                }
                break;
            }
        }
        auto duration = static_cast<int>(fmt_ctx->duration / AV_TIME_BASE);
        qDebug() << "读取的时长为" << duration;
        QTime time{0, 0, duration};

        emit getInformation(Information{maps.value("title"),
                                        maps.value("artist"),
                                        maps.value("album"),
                                        time.toString("mm:ss"),
                                        QString::fromStdString(imagePath)});
    }
}
