#include "ray_trace/renderer.h"

#include <SFML/Config.hpp>
#include <cmath>

#include "ray_trace/material.h"
#include "ray_trace/matrix.h"
#include "ray_trace/scene.h"
#include "ray_trace/scene_object.h"
#include "ray_trace/transform.h"

constexpr double render_margin=1e-6;

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

  bool hasHit() const { return std::isfinite(distance()); }

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
  RayHit hitEmpty () const { return RayHit(); }
  RayHit hitSphere() const;
  RayHit hitBox   () const;
  RayHit hitPlane () const;

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
    const Point start = m_camera.transform().position()
                      + m_pixelSize * (
                          m_camera.transform().right() * x_offset
                        + m_camera.transform().up()    * y_offset);
    return Ray(start, direction);
  }

  ~RenderPlane() = default;

private:
  const Camera& m_camera;
  size_t        m_width;
  size_t        m_height;
  double        m_pixelSize;
};

static Color rayCast(const Ray& ray, const Scene& scene,
                     size_t max_reflections=0);

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
                                         3.0/texture_width);
  // For each row of pixels
  for (size_t y = 0; y < texture_height; ++y)
  {
    // For each column of pixels
    for (size_t x = 0; x < texture_width; ++x)
    {
      // Cast ray from render plane
      Ray ray = render_plane.getRayFrom(x, y);
      Color pixel_color = rayCast(ray, scene, 1);

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

static Color getLighting(const RayHit& hit, const Scene& scene);

static Color rayCast(const Ray& ray, const Scene& scene,
                     size_t max_reflexions)
{
  Ray cast = ray;

  // Try to get closest ray hit
  RayHit hit = cast.getClosestRayHit(scene);

  // If no object hit
  if (!hit.hasHit())
  {
    // If directed light present
    if (scene.hasDirectedLight())
    {
      const double cosine =
        Vec::dotProduct(ray.direction(), -scene.directedLight().direction);
      if (cosine < 0)
      {
        // Render background pixel
        return Color::Black;
      }

      // Render directed light
      return scene.directedLight().color * cosine;
    }

    // Render background pixel
    return Color::Black;
  }

  // Apply surrounding light
  cast.color() += getLighting(hit, scene);


  // Apply material to ray
  const double dot_product = Vec::dotProduct(ray.direction(), hit.normal());
  const double cosine = fabs(dot_product);
  const Material& material = hit.object()->material();

  // Apply emitted light
  cast.color() += material.glowColor();

  // Get diffused light
  cast.color() *= cosine * material.diffusion() * material.color();

  // If rendering reflections
  if (max_reflexions > 0)
  {
    // Add reflection
    Vec ortho = ray.direction() - dot_product*hit.normal();
    Vec reflected = -ray.direction() + 2*ortho;
    Ray reflected_cast(hit.point(), reflected);
    Color reflection = rayCast(reflected_cast, scene, max_reflexions - 1);
    cast.color() += material.reflectivity() * reflection;
  }

  // If ambient light present
  if (scene.hasAmbientLight())
  {
    // Apply ambient light to ray
    cast.color() += scene.ambientLight()*material.color();
  }

  return cast.color();
}

static Color getLighting(const RayHit& hit, const Scene& scene)
{
  Color light = Color::Black;

  // For each object in scene
  for (size_t i = 0; i < scene.objectCount(); ++i)
  {
    const SceneObject& object = scene[i];
    // If object is not light source
    //    or object is the same as hit->object()
    if (!object.isLightSource() || &object == hit.object())
    {
      // Skip object
      continue;
    }

    // Cast ray towards light source
    Vec direction = (object.transform().position() - hit.point()).normalized();
    Ray cast(hit.point(), direction);
    RayHit cast_hit = cast.getClosestRayHit(scene);

    // If hit light source
    if (cast_hit.object() == &object)
    {
      // Add lighting
      double cosine = fabs(Vec::dotProduct(direction, hit.normal()));
      light += cosine*object.material().glowColor();
    }
  }

  // If directed light source present
  if (scene.hasDirectedLight())
  {
    const Vec direction = -scene.directedLight().direction;
    Ray cast(hit.point(), direction);
    RayHit cast_hit = cast.getClosestRayHit(scene);
   
    // If not occluded
    if (!cast_hit.hasHit())
    {
      // Add lighting
      double cosine = fabs(Vec::dotProduct(direction, hit.normal()));
      light += cosine*scene.directedLight().color;
    }
  }

  // Return lighting
  return light;
}

RayHit Ray::getRayHit(const SceneObject& object)
{
  // If object is Hidden
  if (object.type() == ObjectType::Empty ||
      object.material().isHidden())
  {
    // No hit
    return RayHit();
  }

  const Vec    translation = object.transform().position();
  const Matrix scale       = Matrix::fromScale(object.transform().scale());
  const Matrix rotation    = object.transform().rotation();

  const Matrix scale_inv    = scale.getInverse();
  const Matrix rotation_inv = rotation.getInverse();

  Ray transformed(scale_inv*rotation_inv*(m_source - translation),
                  scale_inv*rotation_inv*m_direction,
                  m_color);

  RayHit hit;

  switch (object.type())
  {
  case ObjectType::Sphere:
    hit = transformed.hitSphere();
    break;
  case ObjectType::Box:
    hit = transformed.hitBox();
    break;
  case ObjectType::Plane:
    hit = transformed.hitPlane();
    break;

  case ObjectType::Empty:
  default: return RayHit();
  }

  if (!hit.hasHit())
  {
    // No hit
    return RayHit();
  }

  hit.m_hitPoint    =  rotation*scale    *hit.m_hitPoint + translation;
  hit.m_hitNormal   = (rotation*scale_inv*hit.m_hitNormal).normalized();
  hit.m_hitDistance = (m_source - hit.m_hitPoint).length();
  hit.m_hitObject   = &object;

  return hit;
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

RayHit Ray::hitSphere() const
{
  // Equation for sphere:
  // (r, r) = 1
  // Equation for ray:
  // r = s + d*t
  // Equation for intersection:
  // A = (d, d)
  // B = 2*(s, d)
  // C = (s, s) - 1
  // At^2 + Bt + C = 0
  const double A = Vec::dotProduct(direction(), direction());
  const double B_half = Vec::dotProduct(source(), direction());
  const double C = Vec::dotProduct(source(), source()) - 1;
  const double D_half = B_half*B_half - A*C;

  if (D_half < 0)
  {
    // No hit
    return RayHit();
  }

  const double D_sqrt = sqrt(D_half);
  const double t_0 = (-B_half - D_sqrt) / A;
  const double t_1 = (-B_half + D_sqrt) / A;

  double t_res = -1;
  if      (t_0 > render_margin) t_res = t_0;
  else if (t_1 > render_margin) t_res = t_1;

  if (t_res < 0)
  {
    // No hit
    return RayHit();
  }

  const Point  hit_point    = source() + t_res * direction();
  const Vec    hit_normal   = hit_point.normalized();
  const double hit_distance = t_res;

  return RayHit(hit_distance, hit_point, hit_normal);
}

RayHit Ray::hitBox   () const
{
  // TODO: Render boxes
  return RayHit();
}

RayHit Ray::hitPlane () const
{
  // Plane equation:
  // (n, r) = 0
  // Ray equation:
  // r = s + d*t
  // Intersection equation:
  //     -(n, s)
  // t = -------
  //      (n, d)
  // In standard position n = (0, 1, 0), therefore:
  // t = -s.y / d.y
  
  if (fabs(direction().m_y) < render_margin)
  {
    // Plane parallel to ray, no hit
    return RayHit();
  }

  const double t = - source().m_y / direction().m_y;
  if (t < render_margin)
  {
    // No hit
    return RayHit();
  }

  const Point hit_point = source() + t*direction();
  return RayHit(t, hit_point, Vec::UNIT_Y);
}

