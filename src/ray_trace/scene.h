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

class Scene
{
public:
  static constexpr size_t MAX_OBJECTS = 16;

  Scene(const Camera& camera) :
    m_camera(camera),
    m_objects{},
    m_objectCount(0)
  {
  }
  Scene(const Scene& other) = delete;
  Scene& operator=(const Scene& other) = delete;

  ~Scene() = default;

  const Camera& camera() const { return m_camera; }
        Camera& camera()       { return m_camera; }

  size_t objectCount() const { return m_objectCount; }

  SceneObject& operator[](size_t index) { return m_objects[index]; }

  const SceneObject& operator[](size_t index) const
  {
    return const_cast<Scene*>(this)->operator[](index);
  }

private:
  Camera      m_camera;
  // TODO: Turn into dynamic array
  SceneObject m_objects[MAX_OBJECTS];
  size_t      m_objectCount;
};

#endif /* scene.h */
