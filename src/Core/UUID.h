#pragma once
#include <numeric>

namespace SOF {

class UUID
{
  public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &other);

    operator uint64_t() { return m_UUID; }
    operator const uint64_t() const { return m_UUID; }

  private:
    uint64_t m_UUID;
};

class UUID32
{
  public:
    UUID32();
    UUID32(uint32_t uuid);
    UUID32(const UUID32 &other);

    operator uint32_t() { return m_UUID; }
    operator const uint32_t() const { return m_UUID; }

  private:
    uint32_t m_UUID;
};

}// namespace SOF

namespace std {

template<> struct hash<SOF::UUID>
{
    std::size_t operator()(const SOF::UUID &uuid) const
    {
        // uuid is already a randomly generated number, and is suitable as a hash key as-is.
        // this may change in future, in which case return hash<uint64_t>{}(uuid); might be more appropriate
        return uuid;
    }
};

template<> struct hash<SOF::UUID32>
{
    std::size_t operator()(const SOF::UUID32 &uuid) const { return hash<uint32_t>()((uint32_t)uuid); }
};
}// namespace std
