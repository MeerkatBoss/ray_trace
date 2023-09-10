/**
 * @file material.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_MATERIAL_H
#define __RAY_TRACE_MATERIAL_H

#include "ray_trace/color.h"

enum class MaterialType
{
  Hidden,
  SolidColor
};

class Material
{
public:
  Material() :
    m_type(MaterialType::Hidden),
    m_diffusion(0),
    m_color(Color::Black),
    m_glowColor(Color::Black)
  {
  }

  Material(double diffusion,
           const Color& color,
           const Color& glowColor = Color::Black):
    m_type(MaterialType::SolidColor),
    m_diffusion(0),
    m_color(color),
    m_glowColor(glowColor)
  {
    if      (diffusion < 0) m_diffusion = 0;
    else if (diffusion > 1) m_diffusion = 1;
    else                    m_diffusion = diffusion;
  }

  Material(const Material& other) = default;
  Material& operator=(const Material& other) = default;
  
  ~Material() = default;

  MaterialType type()         const { return m_type; }
  double       diffusion()    const { return m_diffusion; }
  double       reflectivity() const { return 1 - m_diffusion; }
  const Color& color()        const { return m_color; }
  const Color& glowColor()    const { return m_glowColor; }

  bool isHidden() const { return type() == MaterialType::Hidden; }
  bool hasGlow()  const { return glowColor() != Color::Black; }
private:
  MaterialType m_type;
  double       m_diffusion;
  Color        m_color;
  Color        m_glowColor;
};

#endif /* material.h */
