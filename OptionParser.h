#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextStream>
#include <QVector3D>

#include <QDebug>

class OptionParser
{
public:
  OptionParser();

  void addOption(QChar shortName, const QString& longName,
                 const QString description)
  {
    QCommandLineOption option(QStringList() << shortName << longName,
                              description);
    m_parser.addOption(option);
  }

  void addOption(QChar shortName, const QString& longName,
                 const QString description, const QString& valueName)
  {
    QCommandLineOption option(QStringList() << shortName << longName,
                              description, valueName);
    m_parser.addOption(option);
  }

  void addOption(const QString& longName,
                 const QString description, const QString& valueName)
  {
    QCommandLineOption option(QStringList() << longName,
                              description, valueName);
    m_parser.addOption(option);
  }

  template<typename T>
  void addOption(QChar shortName, const QString& longName,
                 const QString description, const QString& valueName,
                 const T& defaultValue)
  {
    QString defaultString;
    QTextStream stream(&defaultString);

    stream << defaultValue;

    QCommandLineOption option(QStringList() << shortName << longName,
                              description +
                              " (Default: " + defaultString + ")", valueName,
                              defaultString);
    m_parser.addOption(option);
  }

  template<typename T>
  void addOption(const QString& longName,
                 const QString description, const QString& valueName,
                 const T& defaultValue)
  {
    QString defaultString;
    QTextStream stream(&defaultString);

    stream << defaultValue;

    QCommandLineOption option(QStringList() << longName,
                              description +
                              " (Default: " + defaultString + ")", valueName,
                              defaultString);
    m_parser.addOption(option);
  }

  template<typename T>
  void getRequiredValue(const QString& option, T* value)
  {
    if(!m_parser.isSet(option))
    {
      qWarning("Missing required option %s", qPrintable(option));
      exit(EXIT_FAILURE);
    }

    // Convert parsed option to value
    QString stringValue = m_parser.value(option);
    QTextStream stream(&stringValue);

    // Force stream to interpret zero-padded numbers as decimal
    // Work-around for Qt bug that interprets zero-padded numbers as octal
    stream.setIntegerBase(10);

    stream >> *value;

    if(stream.status() != QTextStream::Ok || !stream.atEnd())
    {
      qWarning("Failed parsing option %s", qPrintable(option));
      exit(EXIT_FAILURE);
    }
  }

  template<typename T>
  bool getOptionalValue(const QString& option, T* value)
  {
    if(m_parser.value(option).isNull())
      return false;

    // Convert parsed option to value
    QString stringValue = m_parser.value(option);
    QTextStream stream(&stringValue);

    stream >> *value;

    if(stream.status() != QTextStream::Ok || !stream.atEnd())
    {
      qWarning("Failed parsing option %s", qPrintable(option));
      exit(EXIT_FAILURE);
    }

    return true;
  }

  // Specialization for QString. Unlike other types, no conversion is
  // necessary. Conversion by QTextStream would break on white space.
  void getRequiredValue(const QString& option, QString* value)
  {
    if(!m_parser.isSet(option))
    {
      qWarning("Missing required option %s", qPrintable(option));
      exit(EXIT_FAILURE);
    }

    *value = m_parser.value(option);
  }

  // Specialization for QString. Unlike other types, no conversion is
  // necessary. Conversion by QTextStream would break on white space.
  bool getOptionalValue(const QString& option, QString* value)
  {
    if(m_parser.value(option).isNull())
      return false;

    *value = m_parser.value(option);

    return true;
  }

  bool getOptionalValue(const QString &option, bool *value)
  {
    *value = m_parser.isSet(option);
    return *value;
  }

  void showHelp() { m_parser.showHelp(); }

  bool parse(const QStringList& args);
  bool isSet(const QString& name) const
  {
    return m_parser.isSet(name);
  }

private:
  QCommandLineParser m_parser;
};

#endif // OPTIONPARSER_H
