#include "PanoramaPane.hpp"

#include <qnamespace.h>
#include <qquickitem.h>

#include <QPainter>

PanoramaPane::PanoramaPane(QQuickItem* parent) : QQuickPaintedItem(parent) {
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod);
  setFlag(ItemHasContents);
  setImplicitSize(50, 50);

  qRegisterMetaType<DescriptorStrategy::point_correspondence_t>();
  qRegisterMetaType<std::vector<DescriptorStrategy::point_correspondence_t>>();

  m_Worker.moveToThread(&m_HeavyDuties);

  connect(this, &PanoramaPane::StartFeatureDetection, &m_Worker,
          &HeavyDutyWorker::DetectFeatures);
  connect(&m_Worker, &HeavyDutyWorker::featureDetectionInProgressChanged, this,
          &PanoramaPane::UpdateFeatureDetectionInProgress);
  connect(&m_Worker, &HeavyDutyWorker::featuresReady, this,
          &PanoramaPane::UpdateCorrespondences);

  m_HeavyDuties.start();
}

void PanoramaPane::paint(QPainter* painter) {
  QFont font = painter->font();
  font.setPixelSize(100);
  painter->setFont(font);

  QPen pen(Qt::red, 20, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
  painter->setPen(pen);
  painter->setTransform(m_Transformation);

  auto const num_items = m_Model->rowCount();

  for (int i = 0; i < num_items; i++) {
    auto const model_idx = m_Model->index(i, 0);
    auto const img = m_Model->data(model_idx, m_Model->roleNames().key("image"))
                         .value<QImage>();
    auto const uuid = m_Model->data(model_idx, m_Model->roleNames().key("uuid"))
                          .value<QUuid>();

    auto const& target = m_Locations.at(uuid);

    auto const [non_overlapping_region, overlapping_region] =
        IntersectionPartition(uuid);

    painter->setClipping(true);
    painter->setClipRegion(non_overlapping_region);
    painter->drawImage(target, img, img.rect());

    painter->setClipRegion(overlapping_region);
    painter->setOpacity(0.5);
    painter->drawImage(target, img, img.rect());

    painter->setOpacity(1);
    painter->setClipping(false);
  }

  try {
    auto const& selected = m_Locations.at(m_SelectedImage);
    painter->drawRect(QRect{selected.x(), selected.y(),
                            selected.width() - painter->pen().width() / 2,
                            selected.height() - painter->pen().width() / 2});

    for (auto const [p1, p2] : m_Correspondences) {
      painter->drawPoint(p1 + QPoint{selected.x(), selected.y()});
    }

  } catch (std::out_of_range const&) {
  }

  if (m_FeatureDetectionInProgress) {
    QRectF center{width() / 3, height() / 3, 2 * width() / 3, 2 * height() / 3};

    painter->drawText(center, QStringLiteral("feature detection in progress"));
  }
}

void PanoramaPane::UpdateImplicitSize() {
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
  setImplicitSize(max_x * GetCurrentScaling().x(),
                  max_y * GetCurrentScaling().y());
}

void PanoramaPane::OnModelChanged() {
  UpdateImplicitSize();
  update();
}

void PanoramaPane::SetModel(QAbstractItemModel* model) {
  m_Model = model;
  connect(m_Model, SIGNAL(rowsInserted(QModelIndex, int, int)),
          SLOT(OnModelChanged()));
  connect(m_Model, SIGNAL(modelReset()), SLOT(OnModelChanged()));
}

void PanoramaPane::mousePressEvent(QMouseEvent* event) {
  // consume the event. do not propagate it further, e.g. to a parent element
  event->accept();

  auto const location = LocalEventPosToLocalFrame(event->localPos());
  auto const it = std::find_if(m_Locations.cbegin(), m_Locations.cend(),
                               [&location](auto const& key_val) {
                                 return key_val.second.contains(location);
                               });

  if (it != m_Locations.cend()) {
    m_SelectedImage = it->first;
    m_MouseStartLocation = location;

    auto ret =
        m_Model->match(m_Model->index(0, 0), m_Model->roleNames().key("uuid"),
                       m_SelectedImage, 1, Qt::MatchExactly);
    auto const img =
        m_Model->data(ret.first(), m_Model->roleNames().key("image"))
            .value<QImage>();

    emit StartFeatureDetection(img, img, QRect{});
  } else {
    m_SelectedImage = QUuid{};
  }

  update();
}

void PanoramaPane::mouseMoveEvent(QMouseEvent* event) {
  if (m_SelectedImage.isNull()) return;

  event->accept();

  auto const local_mouse_pos = LocalEventPosToLocalFrame(event->localPos());
  auto const movement = local_mouse_pos - m_MouseStartLocation;
  m_MouseStartLocation = local_mouse_pos;

  m_Locations.at(m_SelectedImage).translate(movement);

  auto const top_left = m_Locations.at(m_SelectedImage).topLeft();
  if (top_left.y() < 0)
    m_Locations.at(m_SelectedImage).translate(0, -top_left.y());
  if (top_left.x() < 0)
    m_Locations.at(m_SelectedImage).translate(-top_left.x(), 0);

  UpdateImplicitSize();
  update();
}

void PanoramaPane::mouseReleaseEvent(QMouseEvent* event) {
  if (m_SelectedImage.isNull()) return;

  UpdateImplicitSize();
  event->accept();
}

void PanoramaPane::wheelEvent(QWheelEvent* event) {
  auto const updated_scaling =
      GetCurrentScaling() * (event->angleDelta().y() > 0 ? 1.2 : 0.8);
  m_Transformation =
      QTransform{}.scale(updated_scaling.x(), updated_scaling.y());

  UpdateImplicitSize();
  update();
}

QPoint PanoramaPane::LocalEventPosToLocalFrame(const QPointF& local_pos) const {
  // The user interacts with the transformed (scaled) pane. In order to get back
  // to our unscaled coordinate frame, we need to apply the inverse
  // transformation first
  assert(m_Transformation.isInvertible());
  return m_Transformation.inverted().map(local_pos.toPoint());
}

QPointF PanoramaPane::GetCurrentScaling() const {
  return QPointF{m_Transformation.m11(), m_Transformation.m22()};
}

std::pair<QRegion, QRegion> PanoramaPane::IntersectionPartition(
    QUuid id) const {
  auto const image_rect = m_Locations.at(id);

  auto intersection_part = std::accumulate(
      m_Locations.cbegin(), m_Locations.cend(), QRegion{},
      [&id, &image_rect](auto acc, auto const& location_pair) {
        if (location_pair.first == id) return acc;

        return acc.united(image_rect.intersected(location_pair.second));
      });

  return {QRegion{image_rect}.subtracted(intersection_part),
          std::move(intersection_part)};
}
void PanoramaPane::UpdateFeatureDetectionInProgress(bool is_running) {
  m_FeatureDetectionInProgress = is_running;
  update();
}
void PanoramaPane::UpdateCorrespondences(
    std::vector<std::pair<QPoint, QPoint>> res) {
  m_Correspondences = std::move(res);
  update();
}
