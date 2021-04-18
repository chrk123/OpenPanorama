#include "PanoramaPane.hpp"

#include <qnamespace.h>
#include <qquickitem.h>

#include <QPainter>

PanoramaPane::PanoramaPane(QQuickItem* parent) : QQuickPaintedItem(parent) {
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod);
  setFlag(ItemHasContents);
  setImplicitSize(50, 50);
}

void PanoramaPane::paint(QPainter* painter) {
  QFont font = painter->font();
  font.setPixelSize(20);
  painter->setFont(font);
  painter->setPen(Qt::red);

  auto const num_items = m_Model->rowCount();

  for (int i = 0; i < num_items; i++) {
    auto const model_idx = m_Model->index(i, 0);
    auto const img = m_Model->data(model_idx, m_Model->roleNames().key("image"))
                         .value<QImage>();
    auto const uuid = m_Model->data(model_idx, m_Model->roleNames().key("uuid"))
                          .value<QUuid>();

    auto const& target = m_Locations.at(uuid);

    painter->drawImage(target, img, img.rect());

    if (uuid == m_SelectedImage) {
      painter->drawRect(QRect{target.x(), target.y(),
                              target.width() - painter->pen().width(),
                              target.height() - painter->pen().width()});
    }
  }
}

void PanoramaPane::OnModelChanged() {
  auto const num_items = m_Model->rowCount();

  int max_x = 50;
  int max_y = 50;

  for (int i = 0; i < num_items; i++) {
    auto const model_idx = m_Model->index(i, 0);

    auto const img = m_Model->data(model_idx, m_Model->roleNames().key("image"))
                         .value<QImage>();
    auto const uuid = m_Model->data(model_idx, m_Model->roleNames().key("uuid"))
                          .value<QUuid>();

    auto const& target =
        m_Locations.try_emplace(uuid, img.rect()).first->second;

    if (target.bottomRight().y() > max_y) {
      max_y = target.bottomRight().y();
    }

    if (target.bottomRight().x() > max_x) {
      max_x = target.bottomRight().x();
    }
  }
  setImplicitSize(max_x, max_y);
  update();
}

void PanoramaPane::SetModel(QAbstractItemModel* model) {
  m_Model = model;
  connect(m_Model, SIGNAL(rowsInserted(QModelIndex, int, int)),
          SLOT(OnModelChanged()));
  connect(m_Model, SIGNAL(modelReset()), SLOT(OnModelChanged()));
}

void PanoramaPane::mousePressEvent(QMouseEvent* event) {
  auto const& location = event->localPos().toPoint();
  auto const it = std::find_if(m_Locations.cbegin(), m_Locations.cend(),
                               [&location](auto const& key_val) {
                                 return key_val.second.contains(location);
                               });

  if (it != m_Locations.cend()) {
    m_SelectedImage = it->first;
    update();
  }
}
