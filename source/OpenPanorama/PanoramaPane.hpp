#include <qabstractitemmodel.h>
#include <qobjectdefs.h>
#include <qquickpainteditem.h>
#include <qwindowdefs.h>

#include <QQuickPaintedItem>
#include <QTransform>
#include <QUuid>
#include <map>

class PanoramaPane : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(QAbstractItemModel* model MEMBER m_Model WRITE SetModel)

 public:
  explicit PanoramaPane(QQuickItem* parent = nullptr);

  void paint(QPainter* painter) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

  void UpdateImplicitSize();
  void SetModel(QAbstractItemModel* model);

 public slots:
  void OnModelChanged();

 private:
  QPoint LocalEventPosToLocalFrame(QPointF const& local_pos) const;
  QPointF GetCurrentScaling() const;

 private:
  QUuid m_SelectedImage;
  QAbstractItemModel* m_Model;

  std::map<QUuid, QRect> m_Locations;
  QPoint m_MouseStartLocation;
  QTransform m_Transformation;
};