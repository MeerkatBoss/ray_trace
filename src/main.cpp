#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "ray_trace/camera.h"
#include "ray_trace/material.h"
#include "ray_trace/renderer.h"
#include "ray_trace/scene.h"
#include "ray_trace/scene_object.h"

int main()
{
  Camera camera(Point(0, 0, 0),
                Vec::UNIT_X, Vec::UNIT_Z,
                90);
  SceneObject sphere(ObjectType::Sphere,
                     Material(1, Color::Red),
                     Point(1.5, 0, 0));

  Scene scene(camera, Color::White * 0.5);
  scene.addObject(sphere);

  sf::Texture texture;
  texture.create(1600, 900);

  Renderer renderer(texture);
  renderer.renderScene(scene);

  sf::Sprite sprite(texture);

  sf::RenderWindow window(sf::VideoMode(1600, 900), "Ray trace");
  
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
          window.close();
    }

    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();
  }

  return 0;
}
