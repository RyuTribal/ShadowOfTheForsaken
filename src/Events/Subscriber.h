#pragma once
#include "Event.h"

namespace SOF {
class Subscriber
{
public:
  ~Subscriber();
  void Subscribe(std::function<void(Event &)>);

private:
  UUID m_ID = 0;
};
}// namespace SOF
