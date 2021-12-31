#ifndef OPENPANORAMA_HEAVYDUTYWORKER_H
#define OPENPANORAMA_HEAVYDUTYWORKER_H

#include <QObject>
#include <memory>

#include "DescriptorStrategy.h"

class HeavyDutyWorker : public QObject {
  Q_OBJECT

 public:
  explicit HeavyDutyWorker();

 public slots:
  void DetectFeatures(QImage, QImage, QRect);

 signals:
  void featuresReady(std::vector<std::pair<QPoint, QPoint>>);
  void featureDetectionInProgressChanged(bool in_progress);

 private:
  std::unique_ptr<DescriptorStrategy> m_DescriptorStrategy;
};

#endif  // OPENPANORAMA_HEAVYDUTYWORKER_H
