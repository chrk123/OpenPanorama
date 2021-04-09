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
