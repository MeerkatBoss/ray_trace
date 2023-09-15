#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <cstdio>

#include "ray_trace/camera.h"
#include "ray_trace/color.h"
#include "ray_trace/material.h"
#include "ray_trace/renderer.h"
#include "ray_trace/scene.h"
#include "ray_trace/scene_object.h"
#include "ray_trace/transform.h"
#include "ui/button.h"
#include "ui/click_button.h"

const size_t SCREEN_WIDTH     = 2048 / 2;
const size_t SCREEN_HEIGHT    = 1280 / 2;
const bool   ROTATION_ENABLED = false;

class DebugController : public Clickable
{
public:
  bool isClickActive() const override { return true; }
  void onClick() override { puts("Clicked!"); }
};

static void populateScene(Scene& scene);

int main()
{
  Camera camera(Transform(Vec(0, 0, 0)), 30);
  Scene scene(camera,
              Color::White * 0.3,
              DirectedLight(Vec(0, -1, 1), Color::White * 1.5));
  populateScene(scene);

  sf::Texture texture;
  texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);

  Renderer renderer(texture);
  renderer.renderScene(scene);
  texture.copyToImage().saveToFile("render.png");
  sf::Sprite sprite(texture);

  sf::RenderTexture button_texture;
  button_texture.create(300, 200);
  button_texture.draw(sf::RectangleShape(sf::Vector2f(300, 200)));

  DebugController controller;
  ClickButton debug_button(controller, button_texture.getTexture());

  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                          "Ray trace");
  window.clear(sf::Color::Black);
  window.display();

  double rotation_speed = 90;

  sf::Clock clock;
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
          window.close();

      if (event.type == sf::Event::MouseButtonPressed)
      {
        const sf::Event::MouseButtonEvent& mouse_button_event =
          event.mouseButton;
        MouseEvent::Button button = MouseEvent::Button::None;
        switch (mouse_button_event.button)
        {
        case sf::Mouse::Button::Left: 
          button = MouseEvent::Button::Left; 
          break;
        case sf::Mouse::Button::Right: 
          button = MouseEvent::Button::Right; 
          break;
        case sf::Mouse::Button::Middle: 
          button = MouseEvent::Button::Middle; 
          break;
        case sf::Mouse::Button::XButton1: 
        case sf::Mouse::Button::XButton2: 
        case sf::Mouse::Button::ButtonCount: 
        default:
          break;
        }

        debug_button.handleEvent(MouseEvent{.type=MouseEvent::Type::Click,
                                            .button = button,
                                            .position=sf::Vector2f(
                                                          mouse_button_event.x,
                                                          mouse_button_event.y)}
                                );
      }
    }

    window.clear(sf::Color::White);
    window.draw(sprite);
    window.draw(debug_button.sprite());
    window.display();

    if (ROTATION_ENABLED)
    {
      double delta_time = clock.restart().asMilliseconds() / 1000.0;
      scene[0].transform().rotate(Vec::UNIT_Y, rotation_speed * delta_time);
      renderer.renderScene(scene);
    }
  }

  return 0;
}

static void populateScene(Scene& scene)
{
  SceneObject ellipsoid(ObjectType::Sphere,
                        Material(0.85, Color::Red + Color::White * 0.33),
                        Transform(
                            /* position = */ Vec(0, 0, 10),
                            /* scale    = */ Vec(0.8, 0.8, 1.5)));
  ellipsoid.transform().rotate(Vec::UNIT_X, -45);
  ellipsoid.transform().rotate(Vec::UNIT_Y, -75);

  SceneObject mirror(ObjectType::Sphere,
                     Material(0.3, Color::fromNormalized(0.5, 0.7, 0.6)),
                     Transform(
                       /* position = */ Vec(-2, -1, 11),
                       /* scale    = */ Vec(0.7, 0.7, 0.7)));
  SceneObject sphere(ObjectType::Sphere,
                     Material(0.98, Color::fromNormalized(0.8, 0.7, 0.65)),
                     Transform(
                       /* position = */ Vec(-0.7, -1.5, 8.5),
                       /* scale    = */ Vec(0.5, 0.5, 0.5)));

  SceneObject floor(ObjectType::Plane,
                    Material(1, Color::White),
                    Transform(Vec(0, -2, 0)));
  SceneObject left_wall(ObjectType::Plane,
                        Material(1, Color::Blue),
                        Transform(Vec(-3, 0, 0)));
  left_wall.transform().rotate(Vec::UNIT_Z, 90);

  SceneObject right_wall(ObjectType::Plane,
                        Material(1, Color::Green),
                        Transform(Vec(3, 0, 0)));
  right_wall.transform().rotate(Vec::UNIT_Z, 90);

  SceneObject back_wall(ObjectType::Plane,
                        Material(1, Color::White*0.3),
                        Transform(Vec(0, 0, 15)));
  back_wall.transform().rotate(Vec::UNIT_X, 90);

  Color blue_light = Color::Blue*0.7 + 0.5 * Color::White;
  SceneObject light(ObjectType::Sphere,
                    Material(1, blue_light, blue_light),
                    Transform(Vec(1.7, -0.1, 8), Vec(0.2, 0.2, 0.2)));

  scene.addObject(ellipsoid);
  /*
  scene.addObject(mirror);
  scene.addObject(sphere);
  scene.addObject(floor);
  scene.addObject(back_wall);
  scene.addObject(left_wall);
  scene.addObject(right_wall);
  scene.addObject(light);
  */
}
