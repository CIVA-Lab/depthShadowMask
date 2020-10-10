#include <QDebug>
#include <QCoreApplication>
#include "PLYData.h"

#include <limits>

PLYData::PLYData(QObject *parent) :
  QObject(parent), m_loadCanceled(false), m_valuesLoaded(0),
  m_progressInterval(1)
{
}

bool PLYData::hasError() const
{
  // Discount error occurring from canceling load
  return !m_loadCanceled && !m_errorString.isEmpty();
}

bool PLYData::load(const QString &path)
{
  bool result = false;

  // Save path for file being loaded
  m_path = path;

  // Open ply file
  p_ply ply = ply_open(qPrintable(path), errorCallback, 0, this);

  // If failure, exit early.  m_errorString contains recent error
  if(!ply)
  {
    emit loadFinished();
    return false;
  }

  // Read ply header
  result = ply_read_header(ply);

  // Early exit on header failure
  if(!result)
  {
    ply_close(ply);
    emit loadFinished();
    return false;
  }

  // Gather comments
  const char *comment = NULL;
  while((comment = ply_get_next_comment(ply, comment)))
  {
    // rply has a bug when file uses \r\n for newlines (dos mode).  Comments
    // from these files will contain spurious \n or \r characters.  The
    // following converts to QString and sanitizes the comments by removing
    // any newline characters.
    QString sanitized = QString(comment).remove('\r').remove('\n');

    // Add to sanitized comment to comments list
    m_comments.push_back(sanitized);
  }

  // Accumulator for how many values to be read
  int totalValues = 0;

  // Loop over all elements
  p_ply_element e = NULL;
  while((e = ply_get_next_element(ply, e)))
  {
    const char *name = NULL;
    long count = 0;

    // Inspect element
    ply_get_element_info(e, &name, &count);

    // For now, only support vertex elements
    if(QString(name) == "vertex")
    {
      p_ply_property property = NULL;

      // For each vertex property
      int propertyIndex = 0;
      while((property = ply_get_next_property(e, property)))
      {
        // Get property name
        const char *name = NULL;
        ply_get_property_info(property, &name, NULL, NULL, NULL);

        // Set up callback and get count
        int count = ply_set_read_cb(ply, "vertex", name, vertexCallback, this,
                                    propertyIndex);
        totalValues += count;

        // Allocate vector storage
        QVector<float> data;
        data.reserve(count);
        // Put on list
        m_vertexData.push_back(data);

        // Add property name to list
        m_vertexProperties.push_back(QString(name));

        // Initialize minimum and maximum to extremes
        m_minimums.push_back(std::numeric_limits<float>::max());
        m_maximums.push_back(-std::numeric_limits<float>::max());

        // Increment property counter
        propertyIndex++;
      }
    }
  }

  // Reset cancel load flag
  m_loadCanceled = false;

  // Reset progress
  m_valuesLoaded = 0;

  // Determine progress update frequency; larger of 1% or per-value
  m_progressInterval = qMax(totalValues/100.0, 1.0);

  emit loadStarted(totalValues);

  // Read all the data
  result = ply_read(ply);

  // Clear error if load was cancelled
  if(m_loadCanceled) m_errorString.clear();

  emit loadFinished();

  // close handle
  ply_close(ply);

  // Return true if load successfully completed
  return result && (!m_loadCanceled);
}

const QVector<float> &PLYData::vertexData(const QString &property) const
{
  // Find property name
  int whichProperty = m_vertexProperties.indexOf(property);

  // Ensure property exists
  if(whichProperty > -1)
  {
    // return const data
    return m_vertexData.at(whichProperty);
  }

  // return empty vector
  return m_nullData;
}

int PLYData::vertexCount() const
{
  if(m_vertexData.size() == 0) return 0;

  return m_vertexData.at(0).size();
}

float PLYData::minimum(const QString &property) const
{
  int whichProperty = m_vertexProperties.indexOf(property);

  if((whichProperty < 0))
  {
    qDebug() << "PLYData::minimum() no such property" << property;
    return 0;
  }

  // QVector::value() returns defaulted value if out of bounds
  return m_minimums.value(whichProperty);
}

float PLYData::maximum(const QString &property) const
{
  int whichProperty = m_vertexProperties.indexOf(property);

  if((whichProperty < 0))
  {
    qDebug() << "PLYData::maximum() no such property" << property;
    return 0;
  }

  // QVector::value() returns defaulted value if out of bounds
  return m_maximums.value(whichProperty);
}

QVector<float> PLYData::interleaved(const QString &a, const QString &b,
                                    const QString &c) const
{
  QVector<float> result;

  // Reference property data
  const QVector<float>& first = vertexData(a);
  const QVector<float>& second = vertexData(b);
  const QVector<float>& third = vertexData(c);

  // Validate sizes
  if((first.count() == second.count()) && (second.count() == third.count()))
  {
    // Allocate memory for result vector; all three have same number of items
    result.reserve(first.count() * 3);

    // Swizzle properties into result
    for(int i = 0; i < first.count(); ++i)
    {
      result.push_back(first[i]);
      result.push_back(second[i]);
      result.push_back(third[i]);
    }
  }

  return result;
}

void PLYData::cancelLoad()
{
  m_loadCanceled = true;
}

void PLYData::errorCallback(p_ply ply, const char *message)
{
  PLYData* pdata = NULL;
  long idata = 0;

  // Get user data
  ply_get_ply_user_data(ply, (void **)&pdata, &idata);

  // Set error string
  if(pdata) pdata->m_errorString = message;
}

int PLYData::vertexCallback(p_ply_argument arg)
{
  // Which property for this callback
  long propertyIndex = -1;

  // Pointer to PLYData class that started this load
  PLYData *pdata = NULL;

  // Get user data
  ply_get_argument_user_data(arg, (void**)&pdata, &propertyIndex);

  // Add property to vector
  double value = ply_get_argument_value(arg);
  pdata->m_vertexData[propertyIndex].push_back(value);

  // Update min/max
  if(value < (pdata->m_minimums[propertyIndex]))
  {
    pdata->m_minimums[propertyIndex] = value;
  } else if(value > (pdata->m_maximums[propertyIndex]))
  {
    pdata->m_maximums[propertyIndex] = value;
  }

  // Update count of values
  pdata->m_valuesLoaded++;

  // Only emit per-interval to prevent too many signals.
  if((pdata->m_valuesLoaded % pdata->m_progressInterval) == 0)
  {
    // Process events for current event loop; needed for signals in
    // a thread, or to update the GUI from the main thread
    QCoreApplication::processEvents();

    emit pdata->loadProgress(pdata->m_valuesLoaded);
  }

  // Return 1 to continue loading, 0 to cancel.  m_loadCanceled flag is set by
  // a signal to cancel the rply read loop.
  return !(pdata->m_loadCanceled);
}

