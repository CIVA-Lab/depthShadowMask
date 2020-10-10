#ifndef PLYDATA_H
#define PLYDATA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QVector>
#include "rply.h"

class PLYData : public QObject
{
  Q_OBJECT
public:
  explicit PLYData(QObject *parent = 0);

  // Path for loaded data
  const QString& fileName() const { return m_path; }

  // Indicates if most recent call to load was canceled.  Call this if load()
  // returns false to distinguish user cancel from load error.
  bool loadCanceled() const { return m_loadCanceled; }

  // Get list of comments contained in PLY file
  const QStringList& comments() const { return m_comments; }

  // Indicates whether an error occurred during load
  bool hasError() const;

  // Get most recent error from loading; empty for no error
  QString errorString() const { return m_errorString; }

  // Vertex property names
  const QStringList& vertexProperties() const { return m_vertexProperties; }

  // Vertex data
  const QVector<float>& vertexData(const QString& property) const;
  int vertexCount() const;

  // Get minimum/maximum for a vertex property
  float minimum(const QString& property) const;
  float maximum(const QString& property) const;

  // Interleave properties and return as a new vector
  QVector<float> interleaved(const QString& a, const QString& b,
                             const QString &c) const;

signals:
  void loadStarted(int);
  void loadProgress(int);
  void loadFinished();

public slots:
  // Loads data for path internally.  Result indicates whether data was loaded
  // successfully.  Will also return false for a canceled load; must check
  // loadCanceled() to distinguish from error.  If load failed due to error,
  // errorString() may provide more information.
  bool load(const QString& path);

  // Slot for interrupting a file load.  Only works while read is in progress.
  void cancelLoad();

private:
  // Prevent copy for QObject subclasses
  Q_DISABLE_COPY(PLYData)

  // File path for contained data
  QString m_path;

  // Current processing error; normally empty
  QString m_errorString;

  // List of comments contained in PLY file
  QStringList m_comments;

  // Vertex property names
  QStringList m_vertexProperties;

  // Data for each property
  QVector< QVector<float> > m_vertexData;

  // Minimum and maximum values for each property
  QVector<float> m_minimums;
  QVector<float> m_maximums;

  // Null return value used for vertexData()
  QVector<float> m_nullData;

  // Whether or not load was canceled; only valid after opening a file
  bool m_loadCanceled;

  // Current number of values loaded during open
  int m_valuesLoaded;

  // Number of items to load between progress updates
  int m_progressInterval;

  // Callbacks
  static void errorCallback(p_ply ply, const char *message);
  static int vertexCallback(p_ply_argument arg);
};

#endif // PLYDATA_H
