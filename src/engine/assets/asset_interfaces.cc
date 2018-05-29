#include "engine/assets/asset_interfaces.h"
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    GPUAssetHandle::GPUAssetHandle():
      ptr_(0x0), manager_(nullptr)
    {
    }

    //--------------------------------------------------------------------------------
    GPUAssetHandle::GPUAssetHandle(IGPUAssetManager* manager, uintptr_t asset_ptr):
      ptr_(asset_ptr),
      manager_(manager)
    {
      assert(manager); // Manager is nullptr
      assert(asset_ptr); // The asset bound to this handle is nullptr (The handle is invalid)
    }

    //--------------------------------------------------------------------------------
    void GPUAssetHandle::Release()
    {
      manager_->Release(*this);
      ptr_ = 0x0;
    }

    //--------------------------------------------------------------------------------
    GPUAssetHandle::operator bool() const
    {
      return ptr_ != 0x0;
    }

    //--------------------------------------------------------------------------------
    GPUAssetHandle::operator uintptr_t() const
    {
      return ptr_;
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::BaseAssetHandle():
      asset_id_(-1),
      manager_(nullptr)
    {
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::BaseAssetHandle(IAssetManager* manager, int asset_id):
      asset_id_(asset_id),
      manager_(manager)
    {
      assert(manager); // Manager is nullptr
      // The asset bound to this handle is nullptr (The handle is invalid)
      assert(manager->GetAsset(*this));
      manager->IncreaseRef(*this);
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::BaseAssetHandle(const BaseAssetHandle& other):
      asset_id_(other.asset_id_),
      manager_(other.manager_)
    {
      if (asset_id_ < 0)
      {
        return;
      }

      manager_->IncreaseRef(*this);
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::BaseAssetHandle(BaseAssetHandle&& other):
      asset_id_(other.asset_id_),
      manager_(other.manager_)
    {
      other.asset_id_ = -1;
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::~BaseAssetHandle()
    {
      Release();
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle& BaseAssetHandle::operator=(const BaseAssetHandle& other)
    {
      if (this == &other)
        return *this;

      if (asset_id_ >= 0) // Free existing asset
      {
        Release();
      }

      asset_id_ = other.asset_id_;
      manager_ = other.manager_;

      if (asset_id_ >= 0)
      {
        manager_->IncreaseRef(*this);
      }

      return *this;
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle& BaseAssetHandle::operator=(BaseAssetHandle&& other)
    {
      if (asset_id_ >= 0) // Free existing asset
      {
        Release();
      }

      manager_ = other.manager_;
      asset_id_ = other.asset_id_;
      other.asset_id_ = -1;
      return *this;
    }

    //--------------------------------------------------------------------------------
    bool BaseAssetHandle::operator!() const
    {
      return asset_id_ < 0;
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::operator bool() const
    {
      return asset_id_ >= 0;
    }

    //--------------------------------------------------------------------------------
    BaseAssetHandle::operator int() const
    {
      return asset_id_;
    }

    //--------------------------------------------------------------------------------
    bool BaseAssetHandle::operator==(const BaseAssetHandle& other) const
    {
      return asset_id_ == other.asset_id_;
    }

    //--------------------------------------------------------------------------------
    bool BaseAssetHandle::operator<(const BaseAssetHandle& other) const
    {
      return asset_id_ < other.asset_id_;
    }

    //--------------------------------------------------------------------------------
    void BaseAssetHandle::Release()
    {
      if (asset_id_ < 0)
      {
        // If this handle was already released we can't find it's asset anymore
        return;
      }

      if (manager_->GetReferenceCount(*this) == 0)
      {
        PS_LOG(Error, "Trying to release a deleted asset or nullptr handle");
        asset_id_ = -1; // Invalidate this handle
        return;
      }

      manager_->DecreaseRef(*this);
      asset_id_ = -1; // Invalidate this handle
    }
  }
}
