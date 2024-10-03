#include "pch.h"
#include "Audio.h"

namespace SOF
{

    Audio::Audio(const char *data, size_t data_size, double duration)
      : m_Duration(duration), m_Data(new std::vector<char>(reinterpret_cast<const char *>(data),
                                reinterpret_cast<const char *>(data) + data_size))
    {}

    Audio::~Audio() {}
}// namespace SOF
