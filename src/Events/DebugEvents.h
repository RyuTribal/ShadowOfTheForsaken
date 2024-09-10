#pragma once
#include "Event.h"

namespace SOF {
class ImGuiUpdateEvent : public Event
{
public:
  ImGuiUpdateEvent() {}

  EVENT_CLASS_TYPE(ImGuiUpdate)
  EVENT_CLASS_CATEGORY(EventCategoryDebug)
};
}// namespace SOF
