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

class Camera
{
public:
  Camera(const Point& position, const Vec& forward,
         const Vec& up, double fov_deg);
  Camera(const Camera& other) = default;
  Camera& operator=(const Camera& other) = default;

  const Point& getPosition() const { return m_position; }
  const Vec&   getForward()  const { return m_forward; }
  const Vec&   getUp()       const { return m_up; }
  double       getFovDeg()   const { return m_fov / M_PI * 180; }

  Vec getDirectionAt(double x, double y)
  {
    Vec left = Vec::crossProduct(m_up, m_forward).normalized();
    double cot = cos(m_fov) / sin(m_fov);
    return (m_forward * cot + left * x + m_up * y).normalized();
  }

  ~Camera() = default;

private:
  static constexpr double default_fov = 2 * M_PI / 3;
  static const     Vec default_forward;
  static const     Vec default_up;

  Point  m_position;
  Vec    m_forward;
  Vec    m_up;
  double m_fov;
};

#endif /* camera.h */
