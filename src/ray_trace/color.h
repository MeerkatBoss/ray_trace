/**
 * @file color.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Color in RGB space
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_COLOR_H
#define __RAY_TRACE_COLOR_H

#include <cstddef>
#include <cmath>

class Color
{
public:

  static const Color Black;
  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color Yellow;
  static const Color Cyan;
  static const Color Magenta;
  static const Color White;

  Color(const Color&) = default;
  Color& operator=(const Color&) = default;
  ~Color() = default;

  static Color fromRGB(size_t red, size_t green, size_t blue)
  {
    return Color((double) red   / rgb_max,
                 (double) green / rgb_max,
                 (double) blue  / rgb_max);
  }

  static Color fromNormalized(double red, double green, double blue)
  {
    return Color(red, green, blue);
  }

  size_t red()   const { return m_red   * rgb_max; }
  size_t green() const { return m_green * rgb_max; }
  size_t blue()  const { return m_blue  * rgb_max; }

  double redNormalized()   const { return m_red; }
  double greenNormalized() const { return m_green; }
  double blueNormalized()  const { return m_blue; }

  Color& operator+=(const Color& other)
  {
    m_red   += other.m_red;
    m_green += other.m_green;
    m_blue  += other.m_blue;
    normalize();
    return *this;
  }

  Color& operator*=(const Color& other)
  {
    m_red   *= other.m_red;
    m_green *= other.m_green;
    m_blue  *= other.m_blue;
    return *this;
  }

  Color& operator*=(double scale)
  {
    m_red   *= fabs(scale);
    m_green *= fabs(scale);
    m_blue  *= fabs(scale);
    return *this;
  }

  Color& operator/=(double scale)
  {
    return *this *= 1.0 / scale;
  }

  Color operator+(const Color& other) const { return Color(*this) += other; }
  Color operator*(const Color& other) const { return Color(*this) *= other; }

  Color operator*(double scale)       const { return Color(*this) *= scale; }
  Color operator/(double scale)       const { return Color(*this) /= scale; }

  friend bool operator==(const Color& lhs, const Color& rhs);

private:
  static constexpr size_t rgb_max = 255;

  double m_red, m_green, m_blue;

  Color(double red, double green, double blue) :
    m_red(red),
    m_green(green),
    m_blue(blue)
  {
  }

  double maxComponent() const
  {
    if (m_red > m_green)
      return m_red > m_blue ? m_red : m_blue;
    else
      return m_green > m_blue ? m_green : m_blue;
  }

  void normalize()
  {
    if (maxComponent() > 1)
      *this /= maxComponent();
  }
};

inline Color operator*(double scale, const Color& color)
{
  return color * scale;
}
inline bool operator==(const Color& lhs, const Color& rhs)
{
  constexpr double eps = 1.0 / Color::rgb_max;
  return fabs(lhs.m_red   - rhs.m_red)   < eps
      && fabs(lhs.m_green - rhs.m_green) < eps
      && fabs(lhs.m_blue  - rhs.m_blue)  < eps;
}
inline bool operator!=(const Color& lhs, const Color& rhs)
{
  return !(lhs == rhs);
}

#endif /* color.h */
