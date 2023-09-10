#include "ray_trace/camera.h"

const Vec Camera::default_forward = Vec::UNIT_Y;
const Vec Camera::default_up = Vec::UNIT_Z;

Camera::Camera(const Point& position, const Vec& forward,
               const Vec& up, double fov_deg) :
  m_position(position),
  m_forward(default_forward),
  m_up(default_up),
  m_fov(default_fov)
{
  if (!forward.isZero())
  {
    m_forward = forward.normalized();
  }
  if (!up.isZero())
  {
    m_up = up.normalized();
  }

  // Orthogonalize m_up
  if (m_up.isParallelTo(m_forward))
  {
    if (Vec::UNIT_Z.isParallelTo(m_forward))
    {
      m_up = Vec::UNIT_Y - Vec::UNIT_Y.projectOn(m_forward);
    }
    else
    {
      m_up = Vec::UNIT_Z - Vec::UNIT_Z.projectOn(m_forward);
    }
  }
  else
  {
    m_up = m_up - m_up.projectOn(m_forward);
  }
  m_up = m_up.normalized();

  if (0 < fov_deg && fov_deg < 180)
  {
    m_fov = fov_deg / 180 * M_PI;
  }
}

