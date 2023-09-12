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
const size_t SCREEN_HEIGHT = 500;

static void populateScene(Scene& scene);

int main()
{
  Camera camera(Transform(Vec(0, 0, 0)), 40);
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
    scene[0].transform().rotate(Vec::UNIT_Y, rotation_speed * delta_time);

    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();

    renderer.renderScene(scene);
  }

  return 0;
}

static void populateScene(Scene& scene)
{
  SceneObject sphere(ObjectType::Sphere,
                     Material(0.95, Color::Red + Color::White*0.33),
                     Transform(
                       /* position = */ Vec(0, 0, 10),
                       /* scale    = */ Vec(1, 1, 1.3)));
  SceneObject mirror(ObjectType::Sphere,
                     Material(0.4, Color::fromNormalized(0.5, 0.7, 0.6)),
                     Transform(
                       /* position = */ Vec(-2, -1, 11),
                       /* scale    = */ Vec(0.7, 0.7, 0.7)));
  sphere.transform().rotate(Vec::UNIT_X, 30);
  sphere.transform().rotate(Vec::UNIT_Y, -60);
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

  Color blue_light = Color::Blue + 0.5 * Color::White;
  SceneObject light(ObjectType::Sphere,
                    Material(1, blue_light, blue_light),
                    Transform(Vec(2, -0.5, 8), Vec(0.2, 0.2, 0.2)));

  scene.addObject(sphere);
  scene.addObject(mirror);
  scene.addObject(floor);
  scene.addObject(back_wall);
  scene.addObject(left_wall);
  scene.addObject(right_wall);
  scene.addObject(light);
}
