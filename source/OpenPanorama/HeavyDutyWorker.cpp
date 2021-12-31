#include "HeavyDutyWorker.h"

#include <QImage>
#include <QRect>

#include "../CV/SIFTDescriptorStrategy.h"

HeavyDutyWorker::HeavyDutyWorker()
    : m_DescriptorStrategy(std::make_unique<SIFTDescriptorStrategy>()) {}

void HeavyDutyWorker::DetectFeatures(QImage img1, QImage img2, QRect domain) {
  emit featureDetectionInProgressChanged(true);
  emit featuresReady(m_DescriptorStrategy->GetDescriptors(img1, img2, domain));
  emit featureDetectionInProgressChanged(false);
}
