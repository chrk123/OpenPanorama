#ifndef OPENPANORAMA_DESCRIPTORSTRATEGY_H
#define OPENPANORAMA_DESCRIPTORSTRATEGY_H

#include <utility>
#include <vector>

#include <QImage>
#include <QRect>
#include <QPoint>

class DescriptorStrategy {
 public:
  using point_correspondence_t = std::pair<QPoint, QPoint>;
  virtual std::vector<point_correspondence_t> GetDescriptors(
      QImage const& img1, QImage const& img2, QRect const& domain) = 0;
  virtual ~DescriptorStrategy() = default;
};

#endif  // OPENPANORAMA_DESCRIPTORSTRATEGY_H
