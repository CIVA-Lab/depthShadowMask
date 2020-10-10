#include "Camera.h"

QPointF Camera::imageCoordinate(const QVector3D& v) const
{
  if(!m_isOrtho) return m_krt.imageCoordinate(v);

  return m_ortho.map(v).toPointF();
}

QSize Camera::imagePlaneSize() const
{
  if(!m_isOrtho) return m_krt.imagePlaneSize();

  return m_orthoSize;
}

QVector3D Camera::position() const
{
  if(!m_isOrtho) return m_krt.position();

  return m_orthoPosition;
}

float Camera::depth(const QVector3D &v) const
{
  if(!m_isOrtho) return (v - m_krt.position()).length();

  return m_ortho.map(v).z();
}
