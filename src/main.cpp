#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "ray_trace/camera.h"
#include "ray_trace/color.h"
#include "ray_trace/material.h"
#include "ray_trace/renderer.h"
#include "ray_trace/scene.h"
#include "ray_trace/scene_object.h"
#include "ray_trace/transform.h"

const size_t SCREEN_WIDTH  = 800;
const size_t SCREEN_HEIGHT = 450;

int main()
{
  Camera camera(Transform(Vec(0, 0, 0)), 30);
  SceneObject sphere(ObjectType::Sphere,
                     Material(0.95, Color::Red + Color::White*0.33),
                     Transform(
                       /* position = */ Vec(0, 0, 10),
                       /* scale    = */ Vec(1, 1, 1.3)));
  sphere.transform().rotate(Vec::UNIT_X, 30);
  sphere.transform().rotate(Vec::UNIT_Y, -60);
  SceneObject plane(ObjectType::Plane,
                    Material(1, Color::White),
                    Transform(Vec(0, -2, 0)));


  SceneObject light1(ObjectType::Sphere,
                    Material(1, Color::White, Color::White*2),
                    Transform(Vec(0, 10, 8)));

  Color blue_light = Color::Blue + 0.5 * Color::White;
  SceneObject light2(ObjectType::Sphere,
                    Material(1, blue_light, blue_light),
                    Transform(Vec(2, -0.5, 8), Vec(0.1, 0.1, 0.1)));

  Scene scene(camera, Color::White * 0.25);
  scene.addObject(sphere);
  scene.addObject(plane);
  scene.addObject(light1);
  scene.addObject(light2);

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
    scene[0].transform().rotate(Vec::UNIT_Y, rotation_speed * delta_time);

    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();
  }

  return 0;
}
