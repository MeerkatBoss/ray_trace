/**
 * @file scene.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Collection of renderable objects
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_SCENE_H
#define __RAY_TRACE_SCENE_H

#include <cstddef>

#include "ray_trace/camera.h"
#include "ray_trace/scene_object.h"
#include "ray_trace/color.h"

struct DirectedLight
{
  Vec direction;
  Color color;

  DirectedLight() : direction(0, 0, 0), color(Color::Black) {}
  DirectedLight(const Vec& dir, const Color& col = Color::White) :
    direction(dir.normalized()), color(col)
  {
  }
};

class Scene
{
public:
  static constexpr size_t MAX_OBJECTS = 16;

  Scene(const Camera&        camera,
        const Color&         ambientLight = Color::Black,
        const DirectedLight& directedLight = DirectedLight()) :
    m_camera(camera),
    m_ambientLight(ambientLight),
    m_directedLight(directedLight),
    m_objects{},
    m_objectCount(0)
  {
  }
  Scene(const Scene& other) = delete;
  Scene& operator=(const Scene& other) = delete;

  ~Scene() = default;

  const Camera& camera() const { return m_camera; }
        Camera& camera()       { return m_camera; }

  const Color& ambientLight() const { return m_ambientLight; }
        Color& ambientLight()       { return m_ambientLight; }
  
  bool hasAmbientLight() const { return m_ambientLight != Color::Black; }

  const DirectedLight& directedLight() const { return m_directedLight; }
        DirectedLight& directedLight()       { return m_directedLight; }

  bool hasDirectedLight() const
  {
    return m_directedLight.color != Color::Black;
  }

  size_t objectCount() const { return m_objectCount; }

  SceneObject& operator[](size_t index) { return m_objects[index]; }

  const SceneObject& operator[](size_t index) const
  {
    return const_cast<Scene*>(this)->operator[](index);
  }

  void addObject(const SceneObject& object)
  {
    if (m_objectCount >= MAX_OBJECTS)
      return;
    m_objects[m_objectCount++] = object;
  }

private:
  Camera        m_camera;
  Color         m_ambientLight;
  DirectedLight m_directedLight;
  // TODO: Turn into dynamic array
  SceneObject   m_objects[MAX_OBJECTS];
  size_t        m_objectCount;
};

#endif /* scene.h */
