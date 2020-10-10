#include "VoxelPixelArea.h"

VoxelPixelArea::VoxelPixelArea()
{

}

float VoxelPixelArea::area(const Camera &c, const QVector3D &center,
                           float halfDim)
{
  QPointF points[8];
  float result = 0.0f;

  int code = hullCode(c.position(), center, halfDim);
  int num = m_hull[code][6];

  // Project corners to image plane
  for(int i = 0; i < num; i++)
  {
    points[i] = c.imageCoordinate(indexToVertex(m_hull[code][i], center,
                                   halfDim));
  }
  for(int i = 0; i < num; ++i)
  {
    result += (points[i].x() + points[(i + 1) % num].x())
            * (points[i].y() - points[(i + 1) % num].y());
  }
  return qAbs(result * 0.5f);

}

float VoxelPixelArea::area(const Camera &c, const Cube &cube)
{
  return area(c, cube.center(), cube.halfExtent());
}

float VoxelPixelArea::approximateArea(const Camera &c, const QVector3D &center,
                                      float halfDim)
{
  QPointF points[8];

  int code = hullCode(c.position(), center, halfDim);
  int num = m_hull[code][6];

  points[0] = c.imageCoordinate(indexToVertex(m_hull[code][0], center,
                                 halfDim));

  float minX, maxX, minY, maxY;
  minX = maxX = points[0].x();
  minY = maxY = points[0].y();

  // Project corners to image plane
  for(int i = 1; i < num; i++)
  {
    points[i] = c.imageCoordinate(indexToVertex(m_hull[code][i], center,
                                   halfDim));
    if(points[i].x() > maxX) maxX = points[i].x();
    if(points[i].x() < minX) minX = points[i].x();
    if(points[i].y() > maxY) maxY = points[i].y();
    if(points[i].y() < minY) minY = points[i].y();
  }
  return (maxX - minX) * (maxY - minY);
}

// Maps cube corner index to x,y,z
QVector3D VoxelPixelArea::indexToVertex(int index, const QVector3D &center,
                                        float halfDim)
{
  QVector3D vertex = center;

  vertex[0] += halfDim * (index & 4 ? 1 : -1);
  vertex[1] += halfDim * (index & 2 ? 1 : -1);
  vertex[2] += halfDim * (index & 1 ? 1 : -1);

  return vertex;
}

int VoxelPixelArea::hullCode(const QVector3D &eye, const QVector3D &center,
                             float halfDim)
{
  QVector3D min = center - QVector3D(halfDim, halfDim, halfDim);
  QVector3D max = center + QVector3D(halfDim, halfDim, halfDim);

  return (eye.x() < min.x() ?  1 : 0)
      + (eye.x() > max.x() ?  2 : 0)
      + (eye.y() < min.y() ?  4 : 0)
      + (eye.y() > max.y() ?  8 : 0)
      + (eye.z() < min.z() ? 16 : 0)
      + (eye.z() > max.z() ? 32 : 0);
}

int const VoxelPixelArea::m_hull[64][8] =
{
  {},
  { 0, 1, 3, 2, 0, 0, 4 },
  { 5, 4, 6, 7, 0, 0, 4 },
  {},
  { 1, 0, 4, 5, 0, 0, 4 },
  { 4, 5, 1, 3, 2, 0, 6 },
  { 1, 0, 4, 6, 7, 5, 6 },
  {},
  { 2, 3, 7, 6, 0, 0, 4 },
  { 0, 1, 3, 7, 6, 2, 6 },
  { 5, 4, 6, 2, 3, 7, 6 },
  {},
  {},
  {},
  {},
  {},
  { 4, 0, 2, 6, 0, 0, 4 },
  { 4, 0, 1, 3, 2, 6, 6 },
  { 5, 4, 0, 2, 6, 7, 6 },
  {},
  { 5, 1, 0, 2, 6, 4, 6 },
  { 4, 5, 1, 3, 2, 6, 6 },
  { 5, 1, 0, 2, 6, 7, 6 },
  {},
  { 4, 0, 2, 3, 7, 6, 6 },
  { 4, 0, 1, 3, 7, 6, 6 },
  { 5, 4, 0, 2, 3, 7, 6 },
  {},
  {},
  {},
  {},
  {},
  { 1, 5, 7, 3, 0, 0, 4 },
  { 0, 1, 5, 7, 3, 2, 6 },
  { 1, 5, 4, 6, 7, 3, 6 },
  {},
  { 0, 4, 5, 7, 3, 1, 6 },
  { 0, 4, 5, 7, 3, 2, 6 },
  { 1, 0, 4, 6, 7, 3, 6 },
  {},
  { 1, 5, 7, 6, 2, 3, 6 },
  { 0, 1, 5, 7, 6, 2, 6 },
  { 1, 5, 4, 6, 2, 3, 6 },
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {}
};


//int const VoxelPixelArea::m_hull[64][8] =
//{
//  {},
//  { 4, 0, 1, 3, 2 },
//  { 4, 5, 4, 6, 7 },
//  {},
//  { 4, 1, 0, 4, 5 },
//  { 6, 4, 5, 1, 3, 2, 0 },
//  { 6, 1, 0, 4, 6, 7, 5 },
//  {},
//  { 4, 2, 3, 7, 6 },
//  { 6, 0, 1, 3, 7, 6, 2 },
//  { 6, 5, 4, 6, 2, 3, 7 },
//  {},
//  {},
//  {},
//  {},
//  {},
//  { 4, 4, 0, 2, 6 },
//  { 6, 4, 0, 1, 3, 2, 6 },
//  { 6, 5, 4, 0, 2, 6, 7 },
//  {},
//  { 6, 5, 1, 0, 2, 6, 4 },
//  { 6, 4, 5, 1, 3, 2, 6 },
//  { 6, 5, 1, 0, 2, 6, 7 },
//  {},
//  { 6, 4, 0, 2, 3, 7, 6 },
//  { 6, 4, 0, 1, 3, 7, 6 },
//  { 6, 5, 4, 0, 2, 3, 7 },
//  {},
//  {},
//  {},
//  {},
//  {},
//  { 4, 1, 5, 7, 3 },
//  { 6, 0, 1, 5, 7, 3, 2 },
//  { 6, 1, 5, 4, 6, 7, 3 },
//  {},
//  { 6, 0, 4, 5, 7, 3, 1 },
//  { 6, 0, 4, 5, 7, 3, 2 },
//  { 6, 1, 0, 4, 6, 7, 3 },
//  {},
//  { 6, 1, 5, 7, 6, 2, 3 },
//  { 6, 0, 1, 5, 7, 6, 2 },
//  { 6, 1, 5, 4, 6, 2, 3 },
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {},
//  {}
//};
