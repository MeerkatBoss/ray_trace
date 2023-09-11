#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "ray_trace/camera.h"
#include "ray_trace/material.h"
#include "ray_trace/renderer.h"
#include "ray_trace/scene.h"
#include "ray_trace/scene_object.h"
#include "ray_trace/transform.h"

const size_t SCREEN_WIDTH  = 720;
const size_t SCREEN_HEIGHT = 480;

int main()
{
  Camera camera(Transform(Vec(0, 0, 0)), 80);
  SceneObject sphere(ObjectType::Sphere,
                     Material(1, Color::Red),
                     Transform(
                       /* position = */ Vec(0, 0, 5),
                       /* scale    = */ Vec(2, 3, 2)));

  // sphere.transform().rotate(Vec::UNIT_Y, 15);

  Scene scene(camera, Color::White * 1.0);
  scene.addObject(sphere);

  sf::Texture texture;
  texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);

  Renderer renderer(texture);
  // renderer.renderScene(scene);

  sf::Sprite sprite(texture);

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
    }

    double delta_time = clock.restart().asMilliseconds() / 1000.0;
    renderer.renderScene(scene);
    scene[0].transform().rotate(Vec::UNIT_Z, rotation_speed * delta_time);
    scene[0].transform().rotate(Vec::UNIT_X, rotation_speed * delta_time);

    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();
  }

  return 0;
}
