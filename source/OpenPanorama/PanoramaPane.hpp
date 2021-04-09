#include <qabstractitemmodel.h>
#include <qobjectdefs.h>
#include <qquickpainteditem.h>
#include <qwindowdefs.h>

#include <QQuickPaintedItem>

class PanoramaPane : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(QAbstractItemModel* model MEMBER m_Model WRITE SetModel)
 public:
  explicit PanoramaPane(QQuickItem* parent = nullptr);

  void paint(QPainter* painter) override;

  void mousePressEvent(QMouseEvent* event) override;

  void SetModel(QAbstractItemModel* model);

 private:
  int m_Cols;
  QAbstractItemModel* m_Model;
};