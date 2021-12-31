#ifndef OPENPANORAMA_SIFTDESCRIPTORSTRATEGY_H
#define OPENPANORAMA_SIFTDESCRIPTORSTRATEGY_H

#include "../OpenPanorama/DescriptorStrategy.h"

class SIFTDescriptorStrategy : public DescriptorStrategy {
 public:
  std::vector<std::pair<QPoint, QPoint>> GetDescriptors(
      QImage const& img1, QImage const& img2, QRect const& domain) override;
};

#endif  // OPENPANORAMA_SIFTDESCRIPTORSTRATEGY_H
