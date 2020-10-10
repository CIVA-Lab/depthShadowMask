#include "Cube.h"

Cube::Cube()
{
}

Cube::Cube(const QVector3D &center, float halfExtent) : m_center(center),
  m_halfExtent(halfExtent)
{
}

QVector3D Cube::minimum() const
{
  return m_center - QVector3D(m_halfExtent, m_halfExtent, m_halfExtent);
}

QVector3D Cube::maximum() const
{
  return m_center + QVector3D(m_halfExtent, m_halfExtent, m_halfExtent);

}
