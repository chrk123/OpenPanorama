#pragma once

#include <qurl.h>

#include <QAbstractItemModel>
#include <filesystem>
#include <memory>
#include <vector>

#include "PanoramaImage.hpp"

class PanoramaImageModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  Q_INVOKABLE void AddImageFromFile(QUrl filename);
  Q_INVOKABLE void Reset();
  void translateImage(QUuid id, QPoint const& translation);
  std::pair<QRegion, QRegion> GetOverlap(QUuid) const;

  QUuid GetImageAtLocation(QPoint const&) const;

  QHash<int, QByteArray> roleNames() const override;

  int columnCount(QModelIndex const& parent = QModelIndex{}) const override;
  int rowCount(QModelIndex const& parent = QModelIndex{}) const override;

  QModelIndex index(int row, int column,
                    QModelIndex const& parent = QModelIndex{}) const override;
  QModelIndex parent(QModelIndex const& index) const override;
  QVariant data(QModelIndex const& index,
                int role = Qt::DisplayRole) const override;

 private:
  std::vector<std::unique_ptr<PanoramaImage>> m_Images;
};