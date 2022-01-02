#include "PanoramaImageModel.hpp"

#include <qabstractitemmodel.h>
#include <qnamespace.h>
#include <qvariant.h>

#include <cassert>
#include <memory>

#include "PanoramaImage.hpp"

QHash<int, QByteArray> PanoramaImageModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[Qt::UserRole] = "image";
  roles[Qt::UserRole + 1] = "uuid";
  roles[Qt::UserRole + 2] = "location";
  return roles;
}

int PanoramaImageModel::columnCount(QModelIndex const&) const { return 0; }
int PanoramaImageModel::rowCount(QModelIndex const&) const {
  return static_cast<int>(m_Images.size());
}
QModelIndex PanoramaImageModel::parent(QModelIndex const& index) const {
  return QModelIndex();
}
QModelIndex PanoramaImageModel::index(int row, int column,
                                      QModelIndex const&) const {
  return QAbstractItemModel::createIndex(row, column);
}

QVariant PanoramaImageModel::data(QModelIndex const& index, int role) const {
  if (!index.isValid()) return QVariant();

  auto row = static_cast<size_t>(index.row());
  if (row < 0 || row >= m_Images.size()) return QVariant();

  auto* image = m_Images[row].get();

  if (role == Qt::UserRole) {
    return QVariant::fromValue(image->GetImage());
  } else if (role == Qt::UserRole + 1) {
    return QVariant::fromValue(image->GetUuid());
  } else if (role == Qt::UserRole + 2) {
    return QVariant::fromValue(image->GetLocation());
  }

  assert(false && "unsupported role given");
  return QVariant();
}

void PanoramaImageModel::AddImageFromFile(QUrl filename) {
  int idx = rowCount(QModelIndex());
  beginInsertRows(QModelIndex(), idx, idx);

  QImage img(filename.toLocalFile());

  if (!img.isNull())
    m_Images.push_back(std::make_unique<PanoramaImage>(std::move(img)));

  endInsertRows();
}

void PanoramaImageModel::Reset() {
  beginResetModel();
  m_Images.clear();
  endResetModel();
}
void PanoramaImageModel::translateImage(QUuid id, const QPoint& t) {
  auto img = std::find_if(m_Images.begin(), m_Images.end(),
                          [&id](auto const& img) { return img->GetUuid() == id; });

  if (img != m_Images.end()) {
    (*img)->Translate(t);

    auto const current_location = (*img)->GetLocation();
    if (current_location.bottomRight().y() < 0) {
      (*img)->Translate({0, -current_location.bottomRight().y()});
    }

    if (current_location.bottomRight().x() < 0) {
      (*img)->Translate({-current_location.bottomRight().x(), 0});
    }

    auto const q_idx =
        index(static_cast<int>(std::distance(m_Images.begin(), img)), 0);
    emit dataChanged(q_idx, q_idx, {Qt::UserRole + 2});
  }
}
QUuid PanoramaImageModel::GetImageAtLocation(const QPoint& location) const {
  auto const it = std::find_if(
      m_Images.cbegin(), m_Images.cend(), [&location](auto const& img_ptr) {
        return img_ptr->GetLocation().contains(location);
      });
  if (it != m_Images.cend()) return it->get()->GetUuid();

  return QUuid();
}

std::pair<QRegion, QRegion> PanoramaImageModel::GetOverlap(QUuid id) const {
  auto img = std::find_if(m_Images.cbegin(), m_Images.cend(),
                          [&id](auto const& img) { return img->GetUuid() == id; });
  if (img == m_Images.cend()) return {};

  auto const image_rect = img->get()->GetLocation();

  auto intersection_part = std::accumulate(
      m_Images.cbegin(), m_Images.cend(), QRegion{},
      [&id, &image_rect](auto acc, auto const& ptr) {
        if (ptr->GetUuid() == id) return acc;

        return acc.united(image_rect.intersected(ptr->GetLocation()));
      });

  return {QRegion{image_rect}.subtracted(intersection_part),
          std::move(intersection_part)};
}

