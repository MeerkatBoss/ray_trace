/**
 * @file button.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-09-14
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __UI_BUTTON_H
#define __UI_BUTTON_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

struct MouseEvent
{
  enum class Type {
    Hover,
    Click,
    Hold,
    Release
  };

  enum class Button {
    None,
    Left,
    Right,
    Middle
  };
  
  Type type;
  Button button;
  sf::Vector2f position;
};

class Button
{
public:
  Button(const sf::Texture& texture, const sf::Text& text = sf::Text()) :
    m_sprite(texture),
    m_text(text)
  {
  }

  virtual void handleEvent(const MouseEvent& event) = 0;

  const sf::Sprite& sprite() const { return m_sprite; }
        sf::Sprite& sprite()       { return m_sprite; }

  const sf::Text& text() const { return m_text; }
        sf::Text& text()       { return m_text; }

  virtual ~Button() = default;

private:
  sf::Sprite    m_sprite;
  sf::Text      m_text;
};

#endif /* button.h */
