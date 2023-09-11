/**
 * @file matrix.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief 3x3 matrix for vector transformations
 *
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_MATRIX_H
#define __RAY_TRACE_MATRIX_H

#include <cmath>
#include <cstddef>

#include "ray_trace/vec.h"

class Matrix
{
public:
  static const Matrix One;

  Matrix() :
    m_coords{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
  {
  }
  Matrix(const double (&matrix)[3][3])
  {
    for (size_t i = 0; i < 3; ++i)
      for (size_t j = 0; j < 3; ++j)
        m_coords[i][j] = matrix[i][j];
  }
  Matrix(const Matrix& other) = default;
  Matrix& operator=(const Matrix& other) = default;

  static Matrix fromRotation(const Vec& axis, double angle_deg)
  {
    const double angle = angle_deg / 180 * M_PI;
    const double cos_a = cos(angle);
    const double sin_a = sin(angle);
    const double u_x = axis.m_x;
    const double u_y = axis.m_y;
    const double u_z = axis.m_z;

    return Matrix({
        { u_x*u_x*(1 - cos_a) +     cos_a, u_x*u_y*(1 - cos_a) - u_z*sin_a, u_x*u_z*(1 - cos_a) + u_y*sin_a },
        { u_y*u_x*(1 - cos_a) + u_z*sin_a, u_y*u_y*(1 - cos_a) +     cos_a, u_y*u_z*(1 - cos_a) - u_x*sin_a },
        { u_z*u_x*(1 - cos_a) - u_y*sin_a, u_z*u_y*(1 - cos_a) + u_x*sin_a, u_z*u_z*(1 - cos_a) +     cos_a }
    });
  }

  static Matrix fromScale(const Vec& scale)
  {
    return Matrix({
        { scale.m_x, 0,         0         },
        { 0,         scale.m_y, 0         },
        { 0,         0,         scale.m_z }
    });
  }

  typedef       double (&     Row)[3];
  typedef const double (&ConstRow)[3];

  ConstRow operator[](size_t index) const { return m_coords[index]; }
       Row operator[](size_t index)       { return m_coords[index]; }

  Matrix& operator*=(const Matrix& other)
  {
    for (size_t i = 0; i < 3; ++i)
    {
      double row[3] = { 0, 0, 0 };
      for (size_t j = 0; j < 3; ++j)
        for (size_t k = 0; k < 3; ++k)
          row[j] += m_coords[i][k] * other[k][j];

      for (size_t j = 0; j < 3; ++j)
        m_coords[i][j] = row[j];
    }
    return *this;
  }

  Matrix& operator*=(double scale)
  {
    for (size_t i = 0; i < 3; ++i)
      for (size_t j = 0; j < 3; ++j)
        m_coords[i][j] *= scale;

    return *this;
  }

  Matrix& operator+=(const Matrix& other)
  {
    for (size_t i = 0; i < 3; ++i)
      for (size_t j = 0; j < 3; ++j)
        m_coords[i][j] += other[i][j];
    return *this;
  }

  Matrix& operator-=(const Matrix& other)
  {
    for (size_t i = 0; i < 3; ++i)
      for (size_t j = 0; j < 3; ++j)
        m_coords[i][j] -= other[i][j];
    return *this;
  }

  Matrix operator*(const Matrix& other) const { return Matrix(*this) *= other; }
  Matrix operator*(double scale)        const { return Matrix(*this) *= scale; }
  Matrix operator+(const Matrix& other) const { return Matrix(*this) += other; }
  Matrix operator-(const Matrix& other) const { return Matrix(*this) -= other; }
  Matrix operator-()                    const { return Matrix(*this) *= -1; }

  double determinant() const
  {
    return m_coords[0][0]*m_coords[1][1]*m_coords[2][2]
         + m_coords[0][1]*m_coords[1][2]*m_coords[2][0]
         + m_coords[0][2]*m_coords[1][0]*m_coords[2][1]
         - m_coords[0][2]*m_coords[1][1]*m_coords[2][0]
         - m_coords[0][1]*m_coords[1][0]*m_coords[2][2]
         - m_coords[0][0]*m_coords[1][2]*m_coords[2][1];
  }

  bool hasInverse() const
  {
    return fabs(determinant()) >= EPS;
  }

  Vec operator*(const Vec& vec) const
  {
    const Vec row_x(m_coords[0][0], m_coords[0][1], m_coords[0][2]);
    const Vec row_y(m_coords[1][0], m_coords[1][1], m_coords[1][2]);
    const Vec row_z(m_coords[2][0], m_coords[2][1], m_coords[2][2]);

    return Vec(
      Vec::dotProduct(row_x, vec),
      Vec::dotProduct(row_y, vec),
      Vec::dotProduct(row_z, vec)
    );
  }

  Matrix getInverse() const
  {
    if (!hasInverse())
      return Matrix();

    return Matrix({
      {getAdjoint(0, 0), getAdjoint(0, 1), getAdjoint(0, 2)},
      {getAdjoint(1, 0), getAdjoint(1, 1), getAdjoint(1, 2)},
      {getAdjoint(2, 0), getAdjoint(2, 1), getAdjoint(2, 2)}
    }) *= 1.0 / determinant();
  }

private:
  static double constexpr EPS = 1e-6;
  double m_coords[3][3];

  double getAdjoint(size_t i, size_t j) const
  {
    return getCofactor(j, i);
  }

  double getCofactor(size_t i, size_t j) const
  {
    const int sign = (i + j) % 2 == 0 ? 1 : -1;

    size_t i0 = i == 0 ? 1 : 0;
    size_t j0 = j == 0 ? 1 : 0;
    size_t i1 = i == 1 || i0 == 1 ? 2 : 1;
    size_t j1 = j == 1 || j0 == 1 ? 2 : 1;

    return sign * ( m_coords[i0][j0]*m_coords[i1][j1]
                  - m_coords[i0][j1]*m_coords[i1][j0] );
  }
};

inline Matrix operator*(double scale, const Matrix& matrix)
{
  return matrix * scale;
}

#endif /* matrix.h */
