/**
 * @file click_button.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-09-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __UI_CLICK_BUTTON_H
#define __UI_CLICK_BUTTON_H

#include <SFML/Graphics/Texture.hpp>
#include "ui/button.h"

class Clickable
{
public:
  virtual bool isClickActive() const = 0;
  virtual void onClick() = 0;

  virtual ~Clickable() = default;
};

class ClickButton : public Button
{
public:
  ClickButton(Clickable& controller,
              const sf::Texture& texture,
              const sf::Text& text = sf::Text()) :
    Button(texture, text),
    m_controller(controller)
  {
  }

  void handleEvent(const MouseEvent& event) override;

private:
  Clickable& m_controller;
};

#endif /* click_button.h */
