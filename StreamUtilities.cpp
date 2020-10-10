#include "StreamUtilities.h"

QTextStream& operator>>(QTextStream& s, QVector3D &v)
{
  s >> v[0] >> eat(',') >> v[1] >> eat(',') >> v[2];
  return s;
}

QDebug operator<<(QDebug s, const QVector3D& v)
{
  QDebugStateSaver save(s);

  s.nospace() << v.x() << "," << v.y() << "," << v.z();
  return s;
}

QTextStream& operator>>(QTextStream& s, QSize& size)
{
  int width = 0;
  int height = 0;

  s >> width >> eat('x') >> height;
  size = QSize(width, height);

  return s;
}

QTextStream& operator<<(QTextStream& s, const QSize& size)
{
  s << size.width() << "x" << size.height();
  return s;
}

QDebug operator<<(QDebug s, const QSize& size)
{
  QDebugStateSaver save(s);

  s.nospace() << size.width() << "x" << size.height();
  return s;
}

QTextStream &operator<<(QTextStream &s, const QVector3D &v)
{
  s << v.x() << "," << v.y() << "," << v.z();
  return s;
}
