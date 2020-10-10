#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include "KRtCamera.h"

class Camera
{
public:
  Camera(const KRtCamera& krt) : m_isOrtho(false), m_krt(krt) { }
  Camera(const QMatrix4x4& ortho, const QVector3D& position, const QSize& size)
    : m_isOrtho(true), m_ortho(ortho), m_orthoPosition(position),
      m_orthoSize(size) { }

  QPointF imageCoordinate(const QVector3D& v) const;
  QSize imagePlaneSize() const;
  QVector3D position() const;

  float depth(const QVector3D& v) const;

private:
  bool m_isOrtho;
  QMatrix4x4 m_ortho;
  QVector3D m_orthoPosition;
  QSize m_orthoSize;

  KRtCamera m_krt;
};
#endif
