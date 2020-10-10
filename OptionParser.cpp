#include "OptionParser.h"
#include <QDebug>
#include <QFileInfo>

OptionParser::OptionParser()
{
  addOption('h', "help", "Displays help on commandline options.");
}

bool OptionParser::parse(const QStringList &args)
{
  bool result = true;

  // Parse all options
  m_parser.process(args);

  if(m_parser.isSet("help"))
  {
    m_parser.showHelp();
    exit(EXIT_SUCCESS);
  }

  return result;
}
