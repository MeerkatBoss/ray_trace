/**
 * @file camera.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Scene camera representation
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_CAMERA_H
#define __RAY_TRACE_CAMERA_H

#include <cmath>

#include "ray_trace/vec.h"
#include "ray_trace/transform.h"

class Camera
{
public:
  Camera(const Transform& transform, double fov_deg = 120) :
    m_transform(transform), m_fov(default_fov)
  {
    if (0 < fov_deg && fov_deg < 180)
      m_fov = fov_deg / 180 * M_PI;
  }
  Camera(const Camera& other) = default;
  Camera& operator=(const Camera& other) = default;

  double fovDeg() const { return m_fov / M_PI * 180; }

  void setFov(double fov_deg)
  {
    if (0 < fov_deg && fov_deg < 180)
      m_fov = fov_deg / 180 * M_PI;
  }

  const Transform& transform() const { return m_transform; }
        Transform& transform()       { return m_transform; }

  Vec getDirectionAt(double x, double y) const
  {
    double cot = cos(m_fov / 2) / sin(m_fov / 2);
    return (transform().forward() * cot
          + transform().right()   * x
          + transform().up()      * y).normalized();
  }

  ~Camera() = default;

private:
  static constexpr double default_fov = 2 * M_PI / 3;

  Transform m_transform;
  double    m_fov;
};

#endif /* camera.h */
