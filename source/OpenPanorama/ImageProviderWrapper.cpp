#include "ImageProviderWrapper.hpp"

#include <qnamespace.h>
#include <qquickimageprovider.h>

ImageProviderWrapper::ImageProviderWrapper(
    std::shared_ptr<PanoramaImageModel> model)
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_ImageModel(std::move(model)) {}

QImage ImageProviderWrapper::requestImage(QString const& id, QSize* size,
                                          QSize const&) {
  QUuid const uuid(QByteArray::fromPercentEncoding(id.toLatin1()));
  auto const indices =
      m_ImageModel->match(m_ImageModel->index(0, 0), Qt::UserRole + 1,
                          QVariant::fromValue(uuid), 1, Qt::MatchRecursive);

  assert(indices.size() <= 1 && "images must be unique");
  if (indices.size() != 0) {
    auto const& index = indices.first();
    return qvariant_cast<QImage>(m_ImageModel->data(index, Qt::UserRole));
  }
  return QImage();
}
