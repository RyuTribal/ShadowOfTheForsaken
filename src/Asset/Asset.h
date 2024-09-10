#pragma once

#include "AssetTypes.h"

namespace SOF {

using AssetHandle = UUID;

class Asset
{
  public:
    virtual AssetType GetType() const = 0;
    AssetHandle Handle;// Handling conflicting types just in case
};
}// namespace SOF
