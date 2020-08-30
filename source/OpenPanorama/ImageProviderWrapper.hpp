#pragma once

#include <QQuickImageProvider>
#include <memory>

#include "PanoramaImageModel.hpp"

class ImageProviderWrapper : public QQuickImageProvider {
 public:
  explicit ImageProviderWrapper(std::shared_ptr<PanoramaImageModel> model);
  QImage requestImage(const QString &id, QSize *size,
                      const QSize &requestedSize);

 private:
  std::shared_ptr<PanoramaImageModel> m_ImageModel;
};