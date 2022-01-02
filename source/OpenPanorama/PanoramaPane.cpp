#include "PanoramaPane.hpp"

#include <qnamespace.h>

#include <QPainter>

#include "PanoramaImageModel.hpp"

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

  for (int i = 0; i < m_Model->rowCount(); i++) {
    auto const model_idx = m_Model->index(i, 0);
    auto const img = m_Model->data(model_idx, m_Model->roleNames().key("image"))
                         .value<QImage>();
    auto const uuid = m_Model->data(model_idx, m_Model->roleNames().key("uuid"))
                          .value<QUuid>();
    auto const& target =
        m_Model->data(model_idx, m_Model->roleNames().key("location"))
            .value<QRect>();

    auto const [non_overlapping_region, overlapping_region] =
        m_Model->GetOverlap(uuid);

    painter->setClipping(true);
    painter->setClipRegion(non_overlapping_region);
    painter->drawImage(target, img, img.rect());

    painter->setClipRegion(overlapping_region);
    painter->setOpacity(0.5);
    painter->drawImage(target, img, img.rect());

    painter->setOpacity(1);
    painter->setClipping(false);
  }

  auto const selected_img =
      m_Model->match(m_Model->index(0, 0), m_Model->roleNames().key("uuid"),
                     m_SelectedImage, 1, Qt::MatchExactly);

  if (!selected_img.empty()) {
    auto const& selected_location =
        m_Model
            ->data(selected_img.first(), m_Model->roleNames().key("location"))
            .value<QRect>();
    painter->drawRect(
        QRect{selected_location.x(), selected_location.y(),
              selected_location.width() - painter->pen().width() / 2,
              selected_location.height() - painter->pen().width() / 2});

    for (auto const& [p1, p2] : m_Correspondences) {
      painter->drawPoint(p1 +
                         QPoint{selected_location.x(), selected_location.y()});
    }
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
    auto const& target =
        m_Model->data(model_idx, m_Model->roleNames().key("location"))
            .value<QRect>();

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

void PanoramaPane::SetModel(PanoramaImageModel* model) {
  m_Model = model;
  connect(m_Model, SIGNAL(rowsInserted(QModelIndex, int, int)),
          SLOT(OnModelChanged()));
  connect(m_Model, SIGNAL(modelReset()), SLOT(OnModelChanged()));
  connect(m_Model,
          SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&,
                             const QVector<int>&)),
          SLOT(OnModelChanged()));
}

void PanoramaPane::mousePressEvent(QMouseEvent* event) {
  event->accept();

  m_MouseStartLocation = LocalEventPosToLocalFrame(event->localPos());
  m_SelectedImage = m_Model->GetImageAtLocation(m_MouseStartLocation);

  update();

  if (m_SelectedImage.isNull()) {
    return;
  }

  auto const selected_img_idx =
      m_Model
          ->match(m_Model->index(0, 0), m_Model->roleNames().key("uuid"),
                  m_SelectedImage, 1, Qt::MatchExactly)
          .first();
  auto const selected_img =
      m_Model->data(selected_img_idx, m_Model->roleNames().key("image"))
          .value<QImage>();

  emit StartFeatureDetection(selected_img, selected_img, QRect{});
}

void PanoramaPane::mouseMoveEvent(QMouseEvent* event) {
  event->accept();

  if (m_SelectedImage.isNull()) return;

  auto const local_mouse_pos = LocalEventPosToLocalFrame(event->localPos());
  auto const movement = local_mouse_pos - m_MouseStartLocation;
  m_MouseStartLocation = local_mouse_pos;

  m_Model->translateImage(m_SelectedImage, movement);
}

void PanoramaPane::mouseReleaseEvent(QMouseEvent* event) {
  event->accept();

  if (m_SelectedImage.isNull()) return;
}

void PanoramaPane::wheelEvent(QWheelEvent* event) {
  event->accept();

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

void PanoramaPane::UpdateFeatureDetectionInProgress(bool is_running) {
  m_FeatureDetectionInProgress = is_running;
  update();
}
void PanoramaPane::UpdateCorrespondences(
    std::vector<std::pair<QPoint, QPoint>> res) {
  m_Correspondences = std::move(res);
  update();
}
