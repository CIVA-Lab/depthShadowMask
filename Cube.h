#ifndef CUBE_H
#define CUBE_H
#include <QVector3D>

class Cube
{
public:
  Cube();
  Cube(const QVector3D& center, float halfExtent);

  QVector3D center() const { return m_center; }
  float halfExtent() const { return m_halfExtent; }

  QVector3D minimum() const;
  QVector3D maximum() const;

private:
  QVector3D m_center;
  float m_halfExtent;
};

#endif // CUBE_H
