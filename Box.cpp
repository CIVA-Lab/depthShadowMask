#include "Box.h"

Box::Box() : m_minimum(0,0,0), m_maximum(0,0,0)
{
}

Box::Box(const QVector3D& min, const QVector3D& max) : m_minimum(min),
  m_maximum(max) { }

Box::Box(const Cube &cube) :
  m_minimum(cube.minimum()),
  m_maximum(cube.maximum())
{
//  qWarning("Using cube to box conversion constructor.");
}

bool Box::intersects(const Box &box) const
{
  for(int i = 0; i < 3; ++i)
  {
    if(box.minimum()[i] >= maximum()[i]) return false;
    if(box.maximum()[i] <= minimum()[i]) return false;
  }
  return true;
}

bool Box::intersects(const Ray& ray, double *tmin, double *tmax) const
{
  QVector3D inverseDirection = QVector3D(1, 1, 1)/ray.direction();

  // Test intersection against x-planes of box
  if(inverseDirection.x() >= 0)
  {
    *tmin = (m_minimum.x() - ray.origin().x()) * inverseDirection.x();
    *tmax = (m_maximum.x() - ray.origin().x()) * inverseDirection.x();
  } else {
    *tmin = (m_maximum.x() - ray.origin().x()) * inverseDirection.x();
    *tmax = (m_minimum.x() - ray.origin().x()) * inverseDirection.x();
  }

  // Test intersection against y-planes of box
  double tminy, tmaxy;
  if(inverseDirection.y() >= 0)
  {
    tminy = (m_minimum.y() - ray.origin().y()) * inverseDirection.y();
    tmaxy = (m_maximum.y() - ray.origin().y()) * inverseDirection.y();
  } else {
    tminy = (m_maximum.y() - ray.origin().y()) * inverseDirection.y();
    tmaxy = (m_minimum.y() - ray.origin().y()) * inverseDirection.y();
  }

  if((*tmin > tmaxy) || (tminy > *tmax)) return false;
  if(tminy > *tmin) *tmin = tminy;
  if(tmaxy < *tmax) *tmax = tmaxy;

  // Test intersection against z-planes of box
  double tminz, tmaxz;
  if(inverseDirection.z() >= 0)
  {
    tminz = (m_minimum.z() - ray.origin().z()) * inverseDirection.z();
    tmaxz = (m_maximum.z() - ray.origin().z()) * inverseDirection.z();
  } else {
    tminz = (m_maximum.z() - ray.origin().z()) * inverseDirection.z();
    tmaxz = (m_minimum.z() - ray.origin().z()) * inverseDirection.z();
  }

  if((*tmin > tmaxz) || (tminz > *tmax)) return false;
  if(tminz > *tmin) *tmin = tminz;
  if(tmaxz < *tmax) *tmax = tmaxz;

  return true;
}

bool Box::contains(const QVector3D& point) const
{
  if(point.x() < m_minimum.x() || point.x() > m_maximum.x()) return false;
  if(point.y() < m_minimum.y() || point.y() > m_maximum.y()) return false;
  if(point.z() < m_minimum.z() || point.z() > m_maximum.z()) return false;

  return true;
}

bool Box::isNull() const
{
  return m_minimum == m_maximum;
}

QTextStream& operator>>(QTextStream& s, Box &b)
{
  QVector3D min, max;

  for(int i = 0; i < 3; ++i)
    s >> min[i];

  for(int i = 0; i < 3; ++i)
    s >> max[i];

  if(s.status() == QTextStream::Ok)
    b = Box(min, max);

  return s;
}
