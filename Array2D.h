#ifndef ARRAY2D_H
#define ARRAY2D_H
#include <QList>
#include <QSize>

template<class T>
class Array2D
{
public:
  Array2D() : m_width(0), m_height(0) { }
  Array2D(int width, int height) : m_width(width), m_height(height)
  {
    m_data.reserve(m_width * m_height);
    for(int i = 0; i < (m_width * m_height); ++i)
      m_data.push_back(T());
  }

  Array2D(const QSize& size) : Array2D(size.width(), size.height()) { }

  int width()  const { return m_width;  }
  int height() const { return m_height; }
  QSize size() const { return QSize(m_width, m_height); }

  int count() const { return m_data.count(); }

  bool isEmpty() const { return count() == 0; }

  bool contains(int x, int y) const
  {
    return (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height);
  }

  void fill(const T& value)
  {
    for(int i = 0; i < m_data.size(); ++i)
      m_data[i] = value;
  }

  const T& operator()(int x, int y) const
  {
    return m_data.at(x + (y * m_width));
  }

  T& operator()(int x, int y)
  {
    return m_data[x + (y * m_width)];
  }

  const T& at(int i) const
  {
    return m_data.at(i);
  }

  T& operator[](int i)
  {
    return m_data[i];
  }

  const QList<T>& data() const { return m_data; }

private:
  int m_width;
  int m_height;
  QList<T> m_data;
};

#endif // ARRAY2D_H
