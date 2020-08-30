#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>
#include <memory>

#include "ImageProviderWrapper.hpp"
#include "PanoramaImageModel.hpp"

int main(int argc, char **argv) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);

  auto image_model = std::make_shared<PanoramaImageModel>();
  ImageProviderWrapper image_provider(image_model);
  image_model->AddDummyImage();
  image_model->AddDummyImage();

  QQmlApplicationEngine engine;
  engine.addImageProvider("images", &image_provider);
  engine.rootContext()->setContextProperty("imageModel", image_model.get());
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
  if (engine.rootObjects().isEmpty()) {
    return -1;
  };

  return app.exec();
}