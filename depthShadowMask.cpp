#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QImage>
#include <QRgb>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <QVector3D>

#include "Array2D.h"
#include "OptionParser.h"
#include "PLYData.h"
#include "StreamUtilities.h"
#include "VoxelPixelArea.h"

#include "TextProgress.h"

#include "Camera.h"

#define qLocalized( S ) qPrintable(QLocale::system().toString(S))


QVector3D convert(const QRgb &c)
{
  return QVector3D(qRed(c), qGreen(c), qBlue(c));
}

QRgb convert(const QVector3D& v)
{
  return qRgb(v.x(), v.y(), v.z());
}

class Pixel
{
public:
  Pixel() : color(qRgb(0, 0, 0)), distance(qInf()) {}
  QRgb color;
  float distance;
};

void renderDepth(const Camera& camera, const Cube&c, Array2D<double>& depth)
{
  float area = VoxelPixelArea::area(camera, c.center(), c.halfExtent());

  if(area <= 0) return;

  if(area <= 1.0)
  {
    QPoint position = camera.imageCoordinate(c.center()).toPoint();

    if(depth.contains(position.x(), position.y()))
    {
      double d = depth(position.x(), position.y());
      double distance = (c.center() - camera.position()).length();

      if(distance < d) depth(position.x(), position.y()) = distance;

    }
    return;
  }

  // Voxel is larger than a single pixel, subdivide
  for(int i = 0; i < 8; ++i)
  {
    QVector3D origin = c.center();
    origin[0] += c.halfExtent() * (i & 4 ? 0.5f : -0.5f);
    origin[1] += c.halfExtent() * (i & 2 ? 0.5f : -0.5f);
    origin[2] += c.halfExtent() * (i & 1 ? 0.5f : -0.5f);

    renderDepth(camera, Cube(origin, c.halfExtent() * 0.5), depth);
  }

}

void renderVoxelPosition(const Camera& camera, const Cube &c,
                         Array2D<QVector3D>& result)
{
  float area = VoxelPixelArea::area(camera, c.center(), c.halfExtent());
  if(area <= 0) return; // This shouldn't happen!

  if(area <= 1.0)
  {
    // Map to 2D array through camera
    QPoint position = camera.imageCoordinate(c.center()).toPoint();
    // Validate position is in bounds
    if(result.contains(position.x(), position.y()))
    {
      // See if 3D position is valid
      QVector3D bufferValue = result(position.x(), position.y());

      // Compare depth of buffer value to current value
      if(camera.depth(c.center()) < camera.depth(bufferValue))
      {
        // If closer than what's in buffer, replace
        result(position.x(), position.y()) = c.center();
      }
    }
    return;

  } else {

    // Voxel is larger than a single pixel, subdivide
    for(int i = 0; i < 8; ++i)
    {
      QVector3D origin = c.center();
      origin[0] += c.halfExtent() * (i & 4 ? 0.5f : -0.5f);
      origin[1] += c.halfExtent() * (i & 2 ? 0.5f : -0.5f);
      origin[2] += c.halfExtent() * (i & 1 ? 0.5f : -0.5f);

      renderVoxelPosition(camera, Cube(origin, c.halfExtent() * 0.5), result);
    }
  }
}

// Measure distance to center of voxel when rendering depth
void renderDepth(const Camera& camera, const Cube&c, const QVector3D center,
                 Array2D<double>& depth)
{
  float area = VoxelPixelArea::area(camera, c.center(), c.halfExtent());

  if(area <= 0) return;

  if(area <= 1.0)
  {
    QPoint position = camera.imageCoordinate(c.center()).toPoint();

    if(depth.contains(position.x(), position.y()))
    {
      double d = depth(position.x(), position.y());
      double distance = camera.depth(c.center());

      if(distance < d) depth(position.x(), position.y()) = distance;

    }
    return;
  }

  // Voxel is larger than a single pixel, subdivide
  for(int i = 0; i < 8; ++i)
  {
    QVector3D origin = c.center();
    origin[0] += c.halfExtent() * (i & 4 ? 0.5f : -0.5f);
    origin[1] += c.halfExtent() * (i & 2 ? 0.5f : -0.5f);
    origin[2] += c.halfExtent() * (i & 1 ? 0.5f : -0.5f);

    renderDepth(camera, Cube(origin, c.halfExtent() * 0.5), center, depth);
  }

}
void renderVoxel(const Camera& camera, const Cube& c, const KRtCamera& krt,
                 const QImage& img, Array2D<double> &depth,
                 Array2D<Pixel>& result)
{
  float area = VoxelPixelArea::area(camera, c.center(), c.halfExtent());

  if(area <= 0) return;

  if(area <= 1.0)
  {
    QPoint position = camera.imageCoordinate(c.center()).toPoint();

    if(result.contains(position.x(), position.y()))
    {
      Pixel& p = result(position.x(), position.y());
      double distance = camera.depth(c.center());
      if(distance < p.distance)
      {
        p.distance = distance;

        // Get voxel in krt image
        QPoint p2 = krt.imageCoordinate(c.center()).toPoint();

        if(depth.contains(p2.x(), p2.y()))
        {
          double d = (c.center() - krt.position()).length();
          // Check for occlusion from projection camera
//          if(d <= depth(p2.x(), p2.y()))
          if(qAbs(d - depth(p2.x(), p2.y())) < 1)
          {
            // Get color from image; default to 128 if not in bounds
            QRgb color = qRgb(128, 128, 128);
            if(img.rect().contains(p2)) color = img.pixel(p2);
            p.color = color;
          }
          else
          {
//            qDebug() << "Buffer depth/real depth:" << depth(p2.x(), p2.y())
//                     << d;
//            p.color = qRgb(64, 64, 64);
            p.color = qRgb(255, 0, 0);
          }
        }
      }
    }
    return;
  }

  // Voxel is larger than a single pixel, subdivide
  for(int i = 0; i < 8; ++i)
  {
    QVector3D origin = c.center();
    origin[0] += c.halfExtent() * (i & 4 ? 0.5f : -0.5f);
    origin[1] += c.halfExtent() * (i & 2 ? 0.5f : -0.5f);
    origin[2] += c.halfExtent() * (i & 1 ? 0.5f : -0.5f);

    renderVoxel(camera, Cube(origin, c.halfExtent() * 0.5), krt, img, depth,
                result);
  }

}

double normalize(double min, double value, double max)
{
  if(max == min) return 0.0;

  return (value - min)/(max - min);
}

QRgb invert(QRgb color)
{
  return qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color));
}

QRgb gray(double min, double value, double max)
{
  int gray = 255.0 * normalize(min, value, max);
  return qRgb(gray, gray, gray);
}

QRgb gray(double v)
{
  return qRgb(v * 255, v * 255, v * 255);
}

QRgb jet(double v)
{
  return qRgb(qBound(0.0, 1.5 - qAbs(4 * v - 3), 1.0) * 255,
              qBound(0.0, 1.5 - qAbs(4 * v - 2), 1.0) * 255,
              qBound(0.0, 1.5 - qAbs(4 * v - 1), 1.0) * 255);
}

QRgb hotcold(double v)
{
  return qRgb(qBound(0.0, 2 - qAbs(4 * v - 4), 1.0) * 255,
              qBound(0.0, 2 - qAbs(4 * v - 2), 1.0) * 255,
              qBound(0.0, 2 - qAbs(4 * v - 0), 1.0) * 255);
}

template<typename F>
QRgb map(double min, double value, double max, F f)
{
  return f(normalize(min, value, max));
}

QStringList getFilePaths(const QString& directory)
{
  QStringList paths;

  QDir dir(directory);
  for(const auto &entry: dir.entryInfoList(QDir::Files, QDir::Name))
  {
    paths << entry.absoluteFilePath();
  }

  return paths;
}

void saveDepth(const Array2D<double>& depth, const QString& path)
{
  double min = qInf();
  double max = -qInf();

  QImage image(depth.size(), QImage::Format_BGR888);

  for(int i = 1; i < depth.count(); ++i)
  {
    // Skip empty values
    if(depth.at(i) == qInf()) continue;

    if(depth.at(i) < min) min = depth.at(i);
    if(depth.at(i) > max) max = depth.at(i);
  }

  qDebug() << "Depth min/max:" << min << max;

  for(int y = 0; y < depth.height(); ++y)
    for(int x = 0; x < depth.width(); ++x)
    {
      image.setPixel(x, y, gray(min, depth(x,y), max));
    }

  image.save(path);
}

QImage renderImage(const Camera& camera, const QString& krtPath,
                   const QString& imagePath, const PLYData& ply,
                   float resolution = 1.0)
{
  // Get references to point position arrays
  const QVector<float>& x = ply.vertexData("x");
  const QVector<float>& y = ply.vertexData("y");
  const QVector<float>& z = ply.vertexData("z");

  // Load krt projection path
  KRtCamera projection = KRtCamera::load(krtPath);
//  projection = projection.scaled(0.25);
  projection = projection.scaled(0.5);


//  Camera projection(krt);

  // Load image to project
  QImage image(imagePath);
  image = image.scaled(projection.imagePlaneSize());

  qDebug() << "Loaded" << imagePath << "with size" << image.size();

  qDebug() << "Generating projected depth map";

  // Create depth buffer
  Array2D<double> depth(projection.imagePlaneSize());
  depth.fill(qInf());

  TextProgress depthProgress(ply.vertexCount(), 100);
  for(int v = 0, count = ply.vertexCount(); v < count; v++)
  {
    Cube c(QVector3D(x.at(v), y.at(v), z.at(v)), resolution/2.0);
    renderDepth(projection, c, c.center(), depth);
    depthProgress.update(v);
  }

  saveDepth(depth, "depth.png");

  qDebug() << "done.";

  // Pixel buffer
  Array2D<Pixel> pixels(camera.imagePlaneSize());

  TextProgress progress(ply.vertexCount(), 100);

  for(int v = 0, count = ply.vertexCount(); v < count; v++)
  {
    Cube c(QVector3D(x.at(v), y.at(v), z.at(v)), resolution/2.0);
    renderVoxel(camera, c, projection, image, depth, pixels);

    progress.update(v);
  }

  // Convert pixel buffer to image
  QImage result(pixels.size(), QImage::Format_BGR888);
  for(int y = 0; y < result.height(); ++y)
    for(int x = 0; x < result.width(); ++x)
    {
      result.setPixel(x, y, pixels(x, y).color);
    }

  return result;
}

int main(int argc, char *argv[])
{
  char programName[] = "depthShadowMask";
  argv[0] = programName;

  QCoreApplication a(argc, argv);

  float voxelSize = 1.0;
  float depthDimension = 1024.0;
  float bias = 0.005;
  // Values are for TS ABQ dataset
  double azimuth = 183.29;
  double elevation = 62.16;

  QString depthMapPath;

  OptionParser options;
  options.addOption('a', "azimuth", "Sun azimuth", "degrees", azimuth);
  options.addOption('b', "bias", "Depth bias", "bias", bias);
  options.addOption('c', "camera", "Camera krt file", "camera");
  options.addOption('d', "dmapsize", "Width of depthmap", "size",
                    depthDimension);
  options.addOption('e', "elevation", "Sun elevation", "degrees", elevation);
  options.addOption('o', "output", "Output mask image", "file");
  options.addOption('p', "ply", "PLY file", "file");
  options.addOption('r', "resolution", "Voxel size", "size", voxelSize);
  options.addOption('s', "scale", "Output image scale", "scale", 1.0);
  options.addOption("depthmap", "Output path for depthmap (optional)", "file");

//  options.addOption('k', "krt", "Directory containing KRt files", "path");
//  options.addOption('i', "images", "Directory containing images.", "path");

  options.parse(a.arguments());

  // Get PLY file
  QString plypath;
  options.getRequiredValue("ply", &plypath);
  PLYData ply;
  if(!ply.load(plypath))
  {
    qCritical() << "Failed loading PLY file" << plypath;
    exit(EXIT_FAILURE);
  }

  // Get camera
  QString cameraPath;
  options.getRequiredValue("camera", &cameraPath);
  KRtCamera krt = KRtCamera::load(cameraPath);

  // Get metadata directory
//  QString metaPath;
//  options.getRequiredValue("krt", &metaPath);
//  QStringList metaFilenames = getFilePaths(metaPath);
//  qDebug() << "Found" << metaFilenames.count() << "metadata files.";

  // Get images directory
//  QString imagesPath;
//  options.getRequiredValue("images", &imagesPath);
//  QStringList imageFilenames = getFilePaths(imagesPath);

  // Get optional voxel size
  options.getOptionalValue("resolution", &voxelSize);

  // Get output path
  QString outputPath;
  options.getRequiredValue("output", &outputPath);
  qDebug() << "Saving image as" << outputPath;

  // Get optional camera scale
  float cameraScale = 1.0;
  options.getOptionalValue("scale", &cameraScale);
  krt = krt.scaled(cameraScale);
  Camera krtCamera(krt);

  // Get optional depthmap size
  options.getOptionalValue("dmapsize", &depthDimension);

  // Get optional bias
  options.getOptionalValue("bias", &bias);

  // Get optional output for depthmap
  QString outputDepthMap;
  options.getOptionalValue("depthmap", &outputDepthMap);

  // Get sun position
  options.getOptionalValue("azimuth", &azimuth);
  options.getOptionalValue("elevation", &elevation);


//  Camera camera = camera.scaled(cameraScale);
//  qDebug() << "Image plane size:" << camera.imagePlaneSize();
//  qDebug() << "Projecting 0,0,0 to image plane:" << camera.imageCoordinate({0, 0, 0});

  QVector3D min(ply.minimum("x"), ply.minimum("y"), ply.minimum("z"));
  QVector3D max(ply.maximum("x"), ply.maximum("y"), ply.maximum("z"));
  qDebug() << "Minimum:" << min;
  qDebug() << "Maximum:" << max;

//  qDebug() << "Projecting min coord" << camera.imageCoordinate(min);
//  qDebug() << "Projecting max coord" << camera.imageCoordinate(max);

  qDebug() << "PLY file contains" << qLocalized(ply.vertexCount())
           << "vertices.";

//  QVector3D center = min + (max - min)/2.0;

  QSizeF shadowDepthSize(depthDimension, depthDimension);

  QMatrix4x4 projection;
  projection.viewport(QRectF(QPointF(0, 0), shadowDepthSize));
//  projection.ortho(-500, 500, 500, -500, 0.001, 10000.0);
//  projection.ortho(min.x(), max.x(), max.y(), min.y(), min.z(), max.z());
  projection.ortho(min.x(), max.x(), max.y(), min.y(), 0.001, 1000.0);

  QMatrix4x4 lightView;

  // Place light from the north looking south
  lightView.lookAt(QVector3D(0, ply.maximum("y"), 0), {0, 0, 0}, {0, 0, 1});
  qDebug() << "South looking rotation:" << lightView;

  // Rotate for elevation
  lightView.rotate(-elevation, {1, 0, 0});
  // Rotate for azimuth
  lightView.rotate(azimuth, {0, 0, 1});

  Camera sunCamera(projection * lightView, QVector3D(0, 0, max.z()),
                   shadowDepthSize.toSize());

  // Initialize array for depth values to infinity
  Array2D<double> depthArray(sunCamera.imagePlaneSize());
  depthArray.fill(qInf());

  // Get references to point position arrays
  const QVector<float>& x = ply.vertexData("x");
  const QVector<float>& y = ply.vertexData("y");
  const QVector<float>& z = ply.vertexData("z");

  TextProgress depthProgress(ply.vertexCount(), 100);

  // For each voxel in point cloud
  for(int v = 0, count = ply.vertexCount(); v < count; ++v)
  {
    Cube c(QVector3D(x.at(v), y.at(v), z.at(v)), voxelSize/2.0);
    renderDepth(sunCamera, c, c.center(), depthArray);
    depthProgress.update(v);
  }

  // Optionally save depth map image
  if(!outputDepthMap.isEmpty()) saveDepth(depthArray, outputDepthMap);

  // For each voxel, determine visibility from
  Array2D<double> krtDepthArray(krtCamera.imagePlaneSize());
  krtDepthArray.fill(qInf());

  Array2D<QVector3D> positionArray(krtCamera.imagePlaneSize());
  positionArray.fill(QVector3D(qInf(), qInf(), qInf()));

  qDebug() << "Rendering voxel positions...";

  TextProgress positionProgress(ply.vertexCount(), 100);

  // For each voxel in point cloud
  for(int v = 0, count = ply.vertexCount(); v < count; ++v)
  {
    Cube c(QVector3D(x.at(v), y.at(v), z.at(v)), voxelSize/2.0);

    // Save 3D position of visible voxel
    renderVoxelPosition(krtCamera, c, positionArray);
    positionProgress.update(v);
  }

  qDebug() << "done.";

  // Should probably write out image representing contents of position array
  // for evaluation.

  QImage shadowMask(krtCamera.imagePlaneSize(), QImage::Format_RGB32);
  shadowMask.fill(Qt::black);

  qDebug() << "Generating shadow mask...";

  // For each pixel
  for(int i = 0; i < positionArray.count(); ++i)
  {
    QVector3D position3d = positionArray.at(i);

    // If position it empty, skip it
    if(position3d == QVector3D(qInf(), qInf(), qInf())) continue;

    // Get depth through light matrix
    float lightDistance = sunCamera.depth(position3d);

    // Get image plane position in shadow map
    QPoint lightPlanePosition = sunCamera.imageCoordinate(position3d).toPoint();

    if(depthArray.contains(lightPlanePosition.x(), lightPlanePosition.y()))
    {
      float bufferDepth = depthArray(lightPlanePosition.x(),
                                     lightPlanePosition.y());

      if(bufferDepth < (lightDistance - bias))
      {
        // 3D position is in shadow
        QPoint krtPosition = krtCamera.imageCoordinate(position3d).toPoint();
        shadowMask.setPixel(krtPosition, qRgb(255,255,255));
      }
    }
  }

  shadowMask.save(outputPath);

  qDebug() << "done";

//  for(int i = 0; i < metaFilenames.count(); ++i)
//  {

//    QImage image = renderImage(camera, metaFilenames.at(i),
//                               imageFilenames.at(i), ply, voxelSize);

//    QString filename = outputPath +
//        QStringLiteral("-%1.jpg").arg(i, 6, 10, QLatin1Char('0'));

//    qDebug() << "Saving" << filename;

//    image.save(filename);

//  }


  QTimer::singleShot(0, &a, &QCoreApplication::quit);
  return a.exec();
}
