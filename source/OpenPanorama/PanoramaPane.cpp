#include "PanoramaPane.hpp"

#include <qnamespace.h>
#include <qquickitem.h>

#include <QPainter>
#include <cmath>

PanoramaPane::PanoramaPane(QQuickItem* parent)
    : QQuickPaintedItem(parent), m_Cols{2}, m_SelectedImage{-1} {
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
}

void PanoramaPane::paint(QPainter* painter) {
  QSizeF itemSize = size();

  QFont font = painter->font();
  font.setPixelSize(20);
  painter->setFont(font);
  painter->setPen(Qt::red);

  auto const num_items = m_Model->rowCount();

  int rows = std::ceil(num_items / static_cast<float>(m_Cols));

  int dx = itemSize.width() / m_Cols;
  int dy = itemSize.height() / (rows == 0 ? 1 : rows);

  // TODO: Use ImageProvider

  for (int i = 0; i < num_items; i++) {
    int col = i % m_Cols;
    int row = i / m_Cols;
    QRectF const target(col * dx, row * dy, dx, dy);
    auto const img =
        m_Model->data(m_Model->index(i, 0), Qt::UserRole).value<QImage>();
    painter->drawImage(target, img, img.rect());

    if (i == m_SelectedImage) {
      painter->drawRect(QRectF{target.x(), target.y(),
                               target.width() - painter->pen().widthF(),
                               target.height() - painter->pen().widthF()});
    }
  }
}

void PanoramaPane::SetModel(QAbstractItemModel* model) {
  m_Model = model;
  connect(m_Model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(update()));
  connect(m_Model, SIGNAL(modelReset()), SLOT(update()));
}

void PanoramaPane::mousePressEvent(QMouseEvent* event) {
  auto const num_of_images = m_Model->rowCount();

  // if we do not have any images, we can't select it
  if (num_of_images == 0) return;

  int rows = std::ceil(num_of_images / static_cast<float>(m_Cols));

  int dx = size().width() / m_Cols;
  int dy = size().height() / rows;

  auto const& position = event->localPos();

  // position on the grid
  int idx_y = position.y() / dy;
  int idx_x = position.x() / dx;
  int idx = idx_y * m_Cols + idx_x;

  if (idx < num_of_images) {
    m_SelectedImage = idx;
    update();
  }
}
