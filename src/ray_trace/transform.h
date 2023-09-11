/**
 * @file transform.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Position, scale and rotation of scene object
 *
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_TRANSFORM_H
#define __RAY_TRACE_TRANSFORM_H

#include "ray_trace/vec.h"
#include "ray_trace/matrix.h"

class Transform
{
public:
  Transform(const Point&  position = Point(0, 0, 0),
            const Vec&    scale    = Point(1, 1, 1), const Matrix& rotation = Matrix::One)
    : m_position(position), m_scale(scale), m_rotation(rotation)
  {
  }

  Transform(const Transform& other) = default;
  Transform& operator=(const Transform& other) = default;

  const Point&  position() const { return m_position; }
  const Vec&    scale()    const { return m_scale; }
  const Matrix& rotation() const { return m_rotation; }


  Vec right()    const { return rotation() * Vec::UNIT_X; }
  Vec left()     const { return -right(); }
  Vec up()       const { return rotation() * Vec::UNIT_Y; }
  Vec down()     const { return -up(); }
  Vec forward()  const { return rotation() * Vec::UNIT_Z; }
  Vec backward() const { return -forward(); }

  void move  (const Vec& translation) { m_position += translation; }

  void scale(const Vec& scale)
  {
    m_scale.m_x *= scale.m_x;
    m_scale.m_y *= scale.m_y;
    m_scale.m_z *= scale.m_z;
  }

  void rotate(const Vec& axis, double angle_deg)
  {
    m_rotation = Matrix::fromRotation(axis, angle_deg) * m_rotation;
  }

  void moveTo (const Vec& target) { m_position  = target; }
  void scaleTo(const Vec& scale)  { m_scale = scale; }

private:
  Point  m_position;
  Vec    m_scale;
  Matrix m_rotation;
};

#endif /* transform.h */
