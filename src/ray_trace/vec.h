/**
 * @file vec.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief 3D vector with double-precision floating-point coordinates
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_VEC_H
#define __RAY_TRACE_VEC_H

#include <cmath>

class Vec
{
public:
  double m_x, m_y, m_z;

  static const Vec UNIT_X;
  static const Vec UNIT_Y;
  static const Vec UNIT_Z;

  Vec(double x, double y, double z) :
    m_x(x), m_y(y), m_z(z)
  {
  }

  Vec(const Vec& other) = default;
  Vec& operator=(const Vec& other) = default;

  ~Vec()
  {
    m_x = NAN;
    m_y = NAN;
    m_z = NAN;
  }

  Vec& operator+=(const Vec& other)
  {
    m_x += other.m_x;
    m_y += other.m_y;
    m_z += other.m_z;
    return *this;
  }

  Vec& operator-=(const Vec& other)
  {
    m_x -= other.m_x;
    m_y -= other.m_y;
    m_z -= other.m_z;
    return *this;
  }

  Vec& operator*=(double scale)
  {
    m_x *= scale;
    m_y *= scale;
    m_z *= scale;
    return *this;
  }

  Vec& operator/=(double scale)
  {
    return *this *= 1.0 / scale;
  }

  Vec operator+(const Vec& other) const { return Vec(*this) += other; }
  Vec operator-(const Vec& other) const { return Vec(*this) -= other; }

  Vec operator*(double scale) const { return Vec(*this) *= scale; }
  Vec operator/(double scale) const { return Vec(*this) /= scale; }

  Vec operator-() const { return *this * (-1); }
  Vec operator+() const { return *this; }

  double length() const
  {
    return sqrt(dotProduct(*this, *this));
  }

  bool isZero() const { return fabs(length()) < EPS; }

  Vec normalized() const { return *this / length(); }

  Vec projectOn(const Vec& other) const
  {
    if (other.isZero())
    {
      return Vec(NAN, NAN, NAN);
    }
    const double scale = dotProduct(*this, other) / other.length();
    return other * scale;
  }

  bool isParallelTo(const Vec& other) const
  {
    return Vec::crossProduct(*this, other).isZero();
  }

  static double dotProduct(const Vec& vec1, const Vec& vec2)
  {
    return vec1.m_x * vec2.m_x
         + vec1.m_y * vec2.m_y
         + vec1.m_z * vec2.m_z;
  }

  static Vec crossProduct(const Vec& vec1, const Vec& vec2)
  {
    return Vec(vec1.m_y * vec2.m_z - vec1.m_z * vec2.m_y,
               vec1.m_z * vec2.m_x - vec1.m_x * vec2.m_z,
               vec1.m_x * vec2.m_y - vec1.m_y * vec2.m_x);
  }

private:
  static constexpr double EPS = 1e-6;
};

inline Vec operator*(double scale, const Vec& vec) { return vec * scale; }
inline bool operator==(const Vec& lhs, const Vec& rhs)
{
  return (lhs - rhs).isZero();
}
inline bool operator!=(const Vec& lhs, const Vec& rhs)
{
  return !(lhs == rhs);
}

using Point = Vec;

#endif /* vec.h */
