#pragma once

#include <QQuickImageProvider>
#include <memory>

#include "PanoramaImageModel.hpp"

class ImageProviderWrapper : public QQuickImageProvider {
 public:
  explicit ImageProviderWrapper(std::shared_ptr<PanoramaImageModel> model);
  QImage requestImage(QString const& id, QSize* size,
                      QSize const& requestedSize);

 private:
  std::shared_ptr<PanoramaImageModel> m_ImageModel;
};