#ifndef BOX_H
#define BOX_H
#include "Cube.h"
#include "Ray.h"
#include <QVector3D>
#include <QTextStream>

class Box
{
public:
  Box();
  Box(const QVector3D& min, const QVector3D& max);
  Box(const Cube& cube);

  bool intersects(const Box& box) const;
  bool intersects(const Ray& ray, double *tmin, double *tmax) const;

  bool contains(const QVector3D& point) const;

  bool isNull() const;

  QVector3D maximum() const { return m_maximum; }
  QVector3D minimum() const { return m_minimum; }
  QVector3D center()  const { return m_minimum + (m_maximum - m_minimum)/2.0; }

  float width()  const { return m_maximum.x() - m_minimum.x(); }
  float height() const { return m_maximum.y() - m_minimum.y(); }
  float depth()  const { return m_maximum.z() - m_minimum.z(); }

  float maximumExtent() const { return qMax(width(), qMax(height(), depth())); }

private:
  QVector3D m_minimum;
  QVector3D m_maximum;
};

QTextStream& operator>>(QTextStream& s, Box &b);
#endif // BOX_H
