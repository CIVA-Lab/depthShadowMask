#ifndef RAY_H
#define RAY_H
#include <QVector3D>
class Ray
{
public:
  Ray(const QVector3D& origin, const QVector3D direction) :
    m_origin(origin), m_direction(direction.normalized()) {}

  const QVector3D& origin() const { return m_origin; }
  const QVector3D& direction() const { return m_direction; }

private:
  QVector3D m_origin;
  QVector3D m_direction;
};
#endif // RAY_H
