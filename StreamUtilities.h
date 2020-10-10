#ifndef STREAMUTILITIES_H
#define STREAMUTILITIES_H
#include <QSize>
#include <QTextStream>
#include <QVector3D>
#include <QDebug>
#include <QDebugStateSaver>

template<typename T>
class Eatable
{
public:
  Eatable(T toEat) : m_toEat(toEat) { }
  T toEat() const { return m_toEat; }
private:
  T m_toEat;
};

template<typename T> Eatable<T> eat(T toEat)
{
  return Eatable<T>(toEat);
}

template<typename T>
QTextStream &operator>>(QTextStream &s, Eatable<T> e)
{
  T value;
  s >> value;
  if(value != e.toEat()) s.setStatus(QTextStream::ReadCorruptData);

  return s;
}

QTextStream& operator>>(QTextStream& s, QVector3D &v);
QDebug operator<<(QDebug s, const QVector3D& v);
QTextStream& operator<<(QTextStream& s, const QVector3D& size);

QTextStream& operator>>(QTextStream& s, QSize& size);
QDebug operator<<(QDebug s, const QSize& size);
QTextStream& operator<<(QTextStream& s, const QSize& size);


#endif // STREAMUTILITIES_H
