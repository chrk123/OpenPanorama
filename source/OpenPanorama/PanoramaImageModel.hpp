#pragma once

#include <QAbstractItemModel>
#include <filesystem>
#include <memory>
#include <vector>

#include "PanoramaImage.hpp"

class PanoramaImageModel : public QAbstractItemModel {
 public:
  QHash<int, QByteArray> roleNames() const;

  int columnCount(QModelIndex const& parent) const;
  int rowCount(QModelIndex const& parent) const;

  QModelIndex index(int row, int column,
                    QModelIndex const& parent = QModelIndex()) const;
  QModelIndex parent(QModelIndex const& index) const;
  QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

  void AddDummyImage();

 private:
  std::vector<std::unique_ptr<PanoramaImage>> m_Images;
};