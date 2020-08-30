#pragma once

#include <QImage>
#include <QObject>
#include <QUuid>

class PanoramaImage : public QObject {
  Q_OBJECT

  Q_PROPERTY(QImage image READ GetImage CONSTANT)
  Q_PROPERTY(QUuid uuid READ GetUuid CONSTANT)

 public:
  explicit PanoramaImage(QImage img);

  QImage GetImage() const;
  QUuid GetUuid() const;

 private:
  QImage m_Image;
  QUuid m_Uuid;
};