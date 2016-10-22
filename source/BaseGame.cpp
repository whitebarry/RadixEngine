
#include <radix/BaseGame.hpp>
#include <radix/env/Environment.hpp>
#include <radix/SoundManager.hpp>
#include <radix/system/PlayerSystem.hpp>
#include <radix/system/PhysicsSystem.hpp>
#include <radix/component/Player.hpp>

namespace radix {

Fps BaseGame::fps;

BaseGame::BaseGame() :
    world(window),
    gameWorld(window),
    closed(false),
    config(Environment::getConfig()){
  radix::Environment::init();
}

void BaseGame::init() {
  if(config.cursorVisibility) {
    window.unlockMouse();
  } else {
    window.lockMouse();
  }
  world.setConfig(config);
  world.create();
  renderer = std::make_unique<Renderer>(world);
  camera = std::make_unique<Camera>();
  World::SystemTransaction systemTransaction = world.systemTransact();
  systemTransaction.addSystem<PlayerSystem>();
  systemTransaction.addSystem<PhysicsSystem>();

  screenshotCallbackHolder =
    world.event.addObserver(InputSource::KeyReleasedEvent::Type, [this](const radix::Event &event) {
        const int key =  ((InputSource::KeyReleasedEvent &) event).key;
        if (key == SDL_SCANCODE_G) {
          this->window.printScreenToFile(radix::Environment::getDataDir() + "/screenshot.bmp");
        }
      });


  nextUpdate = SDL_GetTicks(), lastUpdate = 0, lastRender = 0;

  renderer->setViewport(&window);
}

bool BaseGame::isRunning() {
  return !closed;
}

World* BaseGame::getWorld() {
  return &world;
}

ScreenRenderer* BaseGame::getScreenRenderer() {
  return screenRenderer.get();
}

GameWorld* BaseGame::getGameWorld() {
  return &gameWorld;
}

void BaseGame::update() {
  int skipped = 0;
  currentTime = SDL_GetTicks();

  while (currentTime > nextUpdate && skipped < MAX_SKIP) {
    nextUpdate += SKIP_TIME;
    skipped++;
  }
  int elapsedTime = currentTime - lastUpdate;
  SoundManager::update(world.getPlayer());
  world.update(TimeDelta::msec(elapsedTime));
  lastUpdate = currentTime;
}

void BaseGame::processInput() { } /* to avoid pure virtual function */

void BaseGame::cleanUp() {
  world.destroy();
  window.close();
}

void BaseGame::render() {
  prepareCamera();

  fps.countCycle();
  window.swapBuffers();
  lastRender = currentTime;
}

void BaseGame::prepareCamera() {
  camera->setPerspective();
  int viewportWidth, viewportHeight;
  window.getSize(&viewportWidth, &viewportHeight);
  camera->setAspect((float)viewportWidth / viewportHeight);
  const Transform &playerTransform = world.getPlayer().getComponent<Transform>();
  Vector3f headOffset(0, playerTransform.getScale().y, 0);
  camera->setPosition(playerTransform.getPosition() + headOffset);
  const Player &playerComponent = world.getPlayer().getComponent<Player>();
  camera->setOrientation(playerComponent.getHeadOrientation());
}

void BaseGame::close() {
  closed = true;
}

} /* namespace radix */