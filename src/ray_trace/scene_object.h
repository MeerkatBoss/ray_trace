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

#include "ray_trace/material.h"
#include "ray_trace/transform.h"

enum class ObjectType
{
  Empty,
  Sphere,
  Box,
  Plane
};

class SceneObject
{
public:
  SceneObject() :
    m_type(ObjectType::Empty),
    m_material(),
    m_transform()
  {
  }
  SceneObject(ObjectType       type,
              const Material&  material = Material(),
              const Transform& transform = Transform()) :
    m_type(type),
    m_material(material),
    m_transform(transform)
  {
  }

  SceneObject(const SceneObject& other) = default;
  SceneObject& operator=(const SceneObject& other) = default;

  ~SceneObject() = default;

  ObjectType       type()      const { return m_type; }
  const Material&  material()  const { return m_material; }
        Material&  material()        { return m_material; }
  const Transform& transform() const { return m_transform; }
        Transform& transform()       { return m_transform; }


  bool isLightSource() const { return m_material.hasGlow(); }

private:
  ObjectType m_type;
  Material   m_material;
  Transform  m_transform;
};

#endif /* scene_object.h */
