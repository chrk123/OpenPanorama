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

  void paint(QPainter* painter);

  void SetModel(QAbstractItemModel* model);

 private:
  QAbstractItemModel* m_Model;
};