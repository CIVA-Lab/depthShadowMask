#ifndef VOXELPIXELAREA_H
#define VOXELPIXELAREA_H
#include <QVector3D>
#include "Camera.h"
#include "Cube.h"

class VoxelPixelArea
{
public:
  VoxelPixelArea();
  static float area(const Camera& c, const QVector3D& center, float halfDim);
  static float area(const Camera& c, const Cube& cube);

  static float approximateArea(const Camera& c, const QVector3D& center,
                               float halfDim);

private:
  static inline QVector3D indexToVertex(int index, const QVector3D& center,
                                 float halfDim);
  static inline int hullCode(const QVector3D& eye, const QVector3D& center,
                      float halfDim);

  static int const m_hull[64][8];
};

#endif // VOXELPIXELAREA_H
