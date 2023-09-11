#include "ray_trace/renderer.h"

#include <SFML/Config.hpp>
#include <cmath>

#include "ray_trace/material.h"
#include "ray_trace/scene_object.h"

class RayHit
{
public:
  friend class Ray;

  RayHit(const RayHit& other) = default;
  RayHit& operator=(const RayHit& other) = default;
  
  double             distance() const { return m_hitDistance; }
  const Point&       point()    const { return m_hitPoint; }
  const Vec&         normal()   const { return m_hitNormal; }
  const SceneObject* object()   const { return m_hitObject; }

  bool hasHit() const { return object() != nullptr; }

  ~RayHit() = default;
private:
  double             m_hitDistance;
  Point              m_hitPoint;
  Vec                m_hitNormal;
  const SceneObject* m_hitObject;
  
  RayHit(double       distance         = INFINITY,
         const Point& hit_point        = Vec(0, 0, 0),
         const Vec& hit_normal         = Vec(0, 0, 0),
         const SceneObject* hit_object = nullptr) :
    m_hitDistance(distance),
    m_hitPoint(hit_point),
    m_hitNormal(hit_normal),
    m_hitObject(hit_object)
  {
  }
};

class Ray
{
public:
  Ray(const Point& point,
      const Vec& direction,
      const Color& color = Color::Black) :
    m_source(point),
    m_direction(direction.normalized()),
    m_color(color)
  {
  }
  Ray(const Ray& other) = default;
  Ray& operator=(const Ray& other) = default;

  ~Ray() = default;

  const Vec& source() const { return m_source; }
        Vec& source()       { return m_source; }

  const Vec& direction() const { return m_direction; }
        Vec& direction()       { return m_direction; }

  const Color& color() const { return m_color; }
        Color& color()       { return m_color; }
  
  RayHit getRayHit(const SceneObject& object);
  RayHit getClosestRayHit(const Scene& scene);

private:
  Point m_source;
  Vec   m_direction;
  Color m_color;
};

class RenderPlane
{
public:
  RenderPlane(const Camera& camera,
              size_t width,
              size_t height,
              double pixel_size) :
    m_camera(camera), m_width(width), m_height(height), m_pixelSize(pixel_size)
  {
  }
  RenderPlane(const RenderPlane& other) = default;
  RenderPlane& operator=(const RenderPlane& other) = delete;

  size_t width()  const { return m_width; }
  size_t height() const { return m_height; }

  Ray getRayFrom(size_t x, size_t y) const
  {
    const size_t max_dim    = m_width > m_height ? m_width : m_height;
    const long   mid_x      = m_width  / 2;
    const long   mid_y      = m_height / 2;
    const long   max_offset = max_dim / 2;

    const long x_offset = (long) x - mid_x;
    const long y_offset = (long) mid_y - y;
    const Vec direction = m_camera.getDirectionAt(
                                    (double) x_offset / max_offset,
                                    (double) y_offset / max_offset);
    const Point start = m_camera.position()
                      + m_pixelSize * (
                          m_camera.right() * x_offset
                        + m_camera.up()    * y_offset);
    return Ray(start, direction);
  }

  ~RenderPlane() = default;

private:
  const Camera& m_camera;
  size_t        m_width;
  size_t        m_height;
  double        m_pixelSize;
};

static Color rayCast(const Ray& ray, const Scene& scene);

struct Pixel
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
};


void Renderer::renderScene(const Scene& scene)
{
  const size_t texture_width  = m_texture.getSize().x;
  const size_t texture_height = m_texture.getSize().y;
  Pixel* pixels = new Pixel[texture_width * texture_height];

  // Create render plane
  RenderPlane render_plane = RenderPlane(scene.camera(),
                                         texture_width,
                                         texture_height,
                                         1.0 / texture_height);
  // For each row of pixels
  for (size_t y = 0; y < texture_height; ++y)
  {
    // For each column of pixels
    for (size_t x = 0; x < texture_width; ++x)
    {
      // Cast ray from render plane
      Ray ray = render_plane.getRayFrom(x, y);
      Color pixel_color = rayCast(ray, scene);

      // Color pixel with ray color
      pixels[y * texture_width + x] = {
        .red   = pixel_color.red(),
        .green = pixel_color.green(),
        .blue  = pixel_color.blue(),
        .alpha = uint8_t(255)
      };
    }
  }

  // Update texture
  m_texture.update((const sf::Uint8*)pixels);

  delete[] pixels;
}

static Color rayCast(const Ray& ray, const Scene& scene)
{
  Ray cast = ray;

  // Apply ambient light to ray
  cast.color() += scene.ambientLight();

  // Try to get closest ray hit
  RayHit hit = cast.getClosestRayHit(scene);

  // If no object hit
  if (!hit.hasHit())
  {
    // Render background pixel
    return Color::Black;
  }

  // Apply material to ray
  const double cosine = Vec::dotProduct(ray.direction(), hit.normal());
  const Material& material = hit.object()->material();

  cast.color() *= cosine * material.diffusion() * material.color();

  return cast.color();
}

RayHit Ray::getRayHit(const SceneObject& object)
{
  // If object is Hidden
  if (object.type() == ObjectType::None ||
      object.material().isHidden())
  {
    // No hit
    return RayHit();
  }
  // If object is Box
  if (object.type() == ObjectType::Box)
  {
    // TODO: render boxes
    // No hit
    return RayHit();
  }
  
  // Equation for ellipsoid:
  // (x - x_c)^2   (y - y_c)^2   (z - z_c)^2
  // ----------- + ----------- + ----------- = 1
  //    s_x^2         s_y^2         s_z^2
  //
  // Equations for ray
  // x = x_0 + d_x * t
  // y = y_0 + d_y * t
  // z = z_0 + d_z * t
  //
  // Equation for intersection:
  //
  // A = (d_x^2 / s_x^2 + d_y^2 / s_y^2 + d_z^2 / s_z^2)
  // B = (2*d_x*(x_0 - x_c) / s_x^2 + 2*d_y*(y_0 - y_c) / s_y^2
  //        + 2*d_z*(z_0 - z_c) / s_z^2)
  // C = ((x_0 - x_c)^2 / s_x^2 + (y_0 - y_c)^2 / y_x^2 + (z_0 - z_c)^2 / z_x^2
  //       - 1)
  // At^2 + Bt + C = 0

  const double x_c = object.position().m_x;
  const double y_c = object.position().m_y;
  const double z_c = object.position().m_z;

  const double s_x = object.scale().m_x;
  const double s_y = object.scale().m_y;
  const double s_z = object.scale().m_z;

  const double x_0 = source().m_x;
  const double y_0 = source().m_y;
  const double z_0 = source().m_z;

  const double d_x = direction().m_x;
  const double d_y = direction().m_y;
  const double d_z = direction().m_z;

  const double delta_x = x_0 - x_c;
  const double delta_y = y_0 - y_c;
  const double delta_z = z_0 - z_c;
  
  const double s_x2 = s_x * s_x;
  const double s_y2 = s_y * s_y;
  const double s_z2 = s_z * s_z;

  const double A = d_x*d_x / s_x2
                 + d_y*d_y / s_y2
                 + d_z*d_z / s_z2;
  const double B_half = d_x*delta_x / s_x2
                      + d_y*delta_y / s_y2
                      + d_z*delta_z / s_z2;
  const double C = delta_x*delta_x / s_x2
                 + delta_y*delta_y / s_y2
                 + delta_z*delta_z / s_z2
                 - 1;

  double D_half = B_half*B_half - A*C;

  if (D_half < 0)
  {
    // No hit
    return RayHit();
  }

  D_half = sqrt(D_half);

  const double t_1 = (-B_half - D_half) / A;
  const double t_2 = (-B_half - D_half) / A;
  
  double t_res = -1;
  if      (t_1 > 0) t_res = t_1;
  else if (t_2 > 0) t_res = t_2;

  if (t_res < 0)
  {
    // No hit
    return RayHit();
  }

  Point hit_point = source() + t_res * direction();
  Vec normal = (hit_point - object.position()).normalized();
  double distance = t_res;

  return RayHit(distance, hit_point, normal, &object);
}

RayHit Ray::getClosestRayHit(const Scene& scene)
{
  RayHit best_hit;
  size_t object_count = scene.objectCount();
  // For each scene object
  for(size_t i = 0; i < object_count; ++i)
  {
    // Get ray hit
    RayHit hit = getRayHit(scene[i]);

    // If hit object closer than best hit
    if (hit.hasHit() && hit.distance() < best_hit.distance())
    {
      // Update best hit
      best_hit = hit;
    }
  }

  return best_hit;
}
