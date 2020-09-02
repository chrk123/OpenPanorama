#include "PanoramaPane.hpp"

#include <qnamespace.h>
#include <qquickitem.h>

#include <QPainter>
#include <cmath>

PanoramaPane::PanoramaPane(QQuickItem* parent) : QQuickPaintedItem(parent) {}

void PanoramaPane::paint(QPainter* painter) {
  QSizeF itemSize = size();

  QFont font = painter->font();
  font.setPixelSize(20);
  painter->setFont(font);
  painter->setPen(Qt::red);

  auto num_items = m_Model->rowCount();

  int cols = 2;
  int rows = std::ceil(num_items / (float)cols);

  int dx = itemSize.width() / cols;
  int dy = itemSize.height() / rows;

  // TODO: Use ImageProvider

  for (int i = 0; i < num_items; i++) {
    int col = i % cols;
    int row = i / cols;
    QRectF const target(col * dx, row * dy, dx, dy);
    auto const img =
        m_Model->data(m_Model->index(i, 0), Qt::UserRole).value<QImage>();
    painter->drawImage(target, img, img.rect());
  }
}

void PanoramaPane::SetModel(QAbstractItemModel* model) {
  m_Model = model;
  connect(m_Model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(update()));
  connect(m_Model, SIGNAL(modelReset()), SLOT(update()));
}