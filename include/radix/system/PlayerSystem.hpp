#ifndef RADIX_PLAYER_SYSTEM_HPP
#define RADIX_PLAYER_SYSTEM_HPP

#include <random>

#include <radix/system/System.hpp>
#include <radix/PlayerTask.hpp>

namespace radix {

class Entity;

class PlayerSystem : public System {
protected:
  void mouseLook(Entity &entity);
  void move(Entity &entity, TDelta dtime);
  void runTasks(Entity &entity);
  bool allowedToRun(std::list<PlayerTask*> &blackList, PlayerTask *task);
public:
  PlayerSystem(World&);
  ~PlayerSystem();

  const char* getName() const {
    return "PlayerSystem";
  }

  TypeId getTypeId() const {
    return System::getTypeId<std::remove_reference<decltype(*this)>::type>();
  }

  bool runsBefore(const System&);
  void update(TDelta dtime);
};

} /* namespace radix */

#endif /* RADIX_PLAYER_SYSTEM_HPP */
