/**
 * @file renderer.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Main class for ray tracing
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_RENDERER_H
#define __RAY_TRACE_RENDERER_H

#include <SFML/Graphics/Texture.hpp>
#include <cstdint>

#include "ray_trace/scene.h"

class Renderer
{
public:
  Renderer(sf::Texture& texture):
    m_texture(texture)
  {
  }

  const sf::Texture& texture() const { return m_texture; }
        sf::Texture& texture()       { return m_texture; }

  void renderScene(const Scene& scene);

  ~Renderer() = default;
private:
  sf::Texture& m_texture;
};

#endif /* renderer.h */
