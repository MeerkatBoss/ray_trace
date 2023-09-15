#include "ui/click_button.h"
#include "ui/button.h"

void ClickButton::handleEvent(const MouseEvent& event)
{
  if (event.type == MouseEvent::Type::Click &&
      event.button == MouseEvent::Button::Left)
  {
    const sf::FloatRect& bounds = sprite().getGlobalBounds();
    if (m_controller.isClickActive() && bounds.contains(event.position))
    {
      m_controller.onClick();
      sprite().setColor(sf::Color(200, 200, 200));
    }
  }
  
  if (event.type == MouseEvent::Type::Release &&
      event.button == MouseEvent::Button::Left)
  {
    const sf::FloatRect& bounds = sprite().getGlobalBounds();
    if (m_controller.isClickActive() && bounds.contains(event.position))
    {
      sprite().setColor(sf::Color(255, 255, 255));
    }
  }

}
