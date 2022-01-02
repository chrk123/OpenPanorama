#include "PanoramaImage.hpp"

#include <quuid.h>

PanoramaImage::PanoramaImage(QImage img)
    : m_Image(img),
      m_Uuid(QUuid::createUuid()),
      m_Location(QRect{0, 0, m_Image.width(), m_Image.height()}) {}

QImage PanoramaImage::GetImage() const { return m_Image; }
QUuid PanoramaImage::GetUuid() const { return m_Uuid; }
QRect PanoramaImage::GetLocation() const { return m_Location; }
void PanoramaImage::Translate(QPoint const& t) { m_Location.translate(t); }
