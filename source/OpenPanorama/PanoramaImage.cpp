#include "PanoramaImage.hpp"

#include <quuid.h>

PanoramaImage::PanoramaImage(QImage img)
    : m_Image(std::move(img)), m_Uuid(QUuid::createUuid()) {}

QImage PanoramaImage::GetImage() const { return m_Image; }
QUuid PanoramaImage::GetUuid() const { return m_Uuid; }