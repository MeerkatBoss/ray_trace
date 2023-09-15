/**
 * @file movement_controller.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-09-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __CONTROLLERS_MOVEMENT_CONTROLLER_H
#define __CONTROLLERS_MOVEMENT_CONTROLLER_H

#include "ray_trace/transform.h"
#include "ui/click_button.h"

class MovementController : public Clickable
{
public:
  MovementController(Transform& transform, const Vec& step) :
    m_transform(transform),
    m_step(step)
  {
  }

  bool isClickActive() const override { return true; }

  void onClick() override
  {
    m_transform.move(m_step);
  }

private:
  Transform& m_transform;
  Vec        m_step;
};

#endif /* movement_controller.h */
