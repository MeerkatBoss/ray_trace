/**
 * @file scene_object.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Renderable object
 *
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __RAY_TRACE_SCENE_OBJECT_H
#define __RAY_TRACE_SCENE_OBJECT_H

#include "ray_trace/vec.h"
#include "ray_trace/material.h"

enum class ObjectType
{
  None,
  Sphere,
  Box,
  Plane
};

class SceneObject
{
public:
  SceneObject() :
    m_type(ObjectType::None),
    m_position(0, 0, 0),
    m_scale(1, 1, 1),
    m_rotation(0, 0, 1),
    m_material()
  {
  }
  SceneObject(ObjectType      type,
              const Material& material = Material(),
              const Point&    position = Point(0, 0, 0),
              const Vec&      scale    = Vec(1, 1, 1),
              const Vec&      rotation = Vec(0, 0, 1)) :
    m_type(type),
    m_position(position),
    m_scale(scale),
    m_rotation(rotation.normalized()),
    m_material(material)
  {
  }

  SceneObject(const SceneObject& other) = default;
  SceneObject& operator=(const SceneObject& other) = default;

  ~SceneObject() = default;

  ObjectType      type()        const { return m_type; }
  const Point&    position()    const { return m_position; }
  const Vec&      scale()       const { return m_scale; }
  const Vec&      rotation()    const { return m_rotation; }
  const Material& material()    const { return m_material; }

  Point& position() { return m_position; }
  Vec&   scale()    { return m_scale; }

  void setRotation(const Vec& rotation)
  {
    if (!rotation.isZero())
      m_rotation = rotation.normalized();
  }

  bool isLightSource() const { return m_material.hasGlow(); }

private:
  ObjectType m_type;
  Point      m_position;
  Vec        m_scale;
  Vec        m_rotation;
  Material   m_material;
};

#endif /* scene_object.h */
