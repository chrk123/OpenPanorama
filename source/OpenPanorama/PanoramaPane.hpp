#include <qabstractitemmodel.h>
#include <qobjectdefs.h>
#include <qquickpainteditem.h>
#include <qwindowdefs.h>

#include <QQuickPaintedItem>
#include <QRegion>
#include <QThread>
#include <QTransform>
#include <QUuid>
#include <map>

#include "HeavyDutyWorker.h"

class PanoramaImageModel;

class PanoramaPane : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(PanoramaImageModel* model MEMBER m_Model WRITE SetModel)

 public:
  explicit PanoramaPane(QQuickItem* parent = nullptr);

  void paint(QPainter* painter) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

  void UpdateImplicitSize();
  void SetModel(PanoramaImageModel* model);

  ~PanoramaPane() {
    m_HeavyDuties.quit();
    m_HeavyDuties.wait();
  }

 public slots:
  void OnModelChanged();
  void UpdateFeatureDetectionInProgress(bool);
  void UpdateCorrespondences(std::vector<std::pair<QPoint, QPoint>>);

  signals:
   void StartFeatureDetection(QImage, QImage, QRect);

 private:
  QPoint LocalEventPosToLocalFrame(QPointF const& local_pos) const;
  QPointF GetCurrentScaling() const;

 private:
  QUuid m_SelectedImage;
  PanoramaImageModel* m_Model;

  QPoint m_MouseStartLocation;
  QTransform m_Transformation;

  std::vector<std::pair<QPoint, QPoint>> m_Correspondences;
  bool m_FeatureDetectionInProgress{false};

  QThread m_HeavyDuties;
  HeavyDutyWorker m_Worker;
};