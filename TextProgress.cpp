#include "TextProgress.h"
#include <QTextStream>

TextProgress::TextProgress(int total, int updateInterval) :
  m_total(total), m_update(updateInterval)
{
  m_timer.start();
}

void TextProgress::update(int value)
{
  if(m_timer.hasExpired(m_update))
  {
    // Output progress
    QTextStream(stdout) << '\r' << static_cast<float>(value)/m_total * 100.0
                        << " %                ";
    // Restart timer
    m_timer.restart();
  }

  if(value >= (m_total - 1))
    QTextStream(stdout) << "\r100 %           " << endl;
}
