#ifndef TEXTPROGRESS_H
#define TEXTPROGRESS_H

#include <QElapsedTimer>

class TextProgress
{
public:

  TextProgress(int total, int updateInterval = 500);

  void update(int value);

private:

  int m_total;
  int m_update;
  QElapsedTimer m_timer;

};

#endif // TEXTPROGRESS_H
