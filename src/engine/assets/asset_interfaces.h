#pragma once

#include <foundation/utils/asset_definitions.h>
#include <foundation/logging/logger.h>

#include <assert.h>
#include <atomic>

namespace sulphur
{
  namespace engine
  {
    class BaseAssetHandle;
    class GPUAssetHandle;

    class IGPUAssetManager
    {
    public:
      virtual void Release(GPUAssetHandle& handle) = 0;
    };

    class GPUAssetHandle
    {
    public:
      GPUAssetHandle() :
        ptr_(0x0), manager_(nullptr)
      {}

      GPUAssetHandle(IGPUAssetManager* manager, uintptr_t asset_ptr) :
        ptr_(asset_ptr),
        manager_(manager)
      {
        assert(manager); // Manager is nullptr
        assert(asset_ptr); // The asset bound to this handle is nullptr (The handle is invalid)
      }

      void Release()
      {
        manager_->Release(*this);
        ptr_ = 0x0;
      }

      operator bool() const { return ptr_ != 0x0; }
      operator uintptr_t() const { return ptr_; }

    private:
      uintptr_t ptr_;
      IGPUAssetManager * manager_;
    };

    class IAssetManager
    {
    public:
      IAssetManager() = default;
      IAssetManager(const IAssetManager&) = delete; // no copies

      virtual ~IAssetManager() = default;
      virtual void Shutdown() = 0;

      IAssetManager& operator=(const IAssetManager&) = delete; // no copy-assignments

      virtual int GetHandle(foundation::AssetID id) = 0;
      virtual int GetHandle(const foundation::AssetName& name) = 0;
      virtual int Load(foundation::AssetID id) = 0;
      virtual int Load(const foundation::AssetName& name) = 0;
      virtual void ReleaseGPUHandles() = 0;
      virtual void Release(foundation::AssetID id) = 0;
      virtual void Release(const foundation::AssetName& name) = 0;
      virtual void IncreaseRef(const BaseAssetHandle& handle) = 0;
      virtual void DecreaseRef(const BaseAssetHandle& handle) = 0;
      virtual GPUAssetHandle& GetGPUHandle(const BaseAssetHandle& handle) = 0;
      virtual int GetReferenceCount(foundation::AssetID id) = 0;
      virtual int GetReferenceCount(const foundation::AssetName& name) = 0;
      virtual int GetReferenceCount(const BaseAssetHandle& handle) = 0;
      virtual bool ValidateHandle(const BaseAssetHandle& handle) const = 0;
      virtual void* GetAsset(const BaseAssetHandle& handle) const = 0;
    };

    class BaseAssetHandle
    {
    public:
      BaseAssetHandle() :
        asset_id_(-1),
        manager_(nullptr)
      {}

      BaseAssetHandle(IAssetManager* manager, int asset_id) :
        asset_id_(asset_id),
        manager_(manager)
      {
        assert(manager); // Manager is nullptr
        // The asset bound to this handle is nullptr (The handle is invalid)
        assert(manager->GetAsset(*this)); 
        manager->IncreaseRef(*this);
      }

      BaseAssetHandle(const BaseAssetHandle& other) : // Copy constructor
        asset_id_(other.asset_id_),
        manager_(other.manager_)
      {
        if (asset_id_ < 0)
        {
          return;
        }

        manager_->IncreaseRef(*this);
      }

      BaseAssetHandle(BaseAssetHandle&& other) : // Move constructor
        asset_id_(other.asset_id_),
        manager_(other.manager_)
      {
        other.asset_id_ = -1;
      }
      
      BaseAssetHandle& operator=(const BaseAssetHandle& other)
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
      };

      BaseAssetHandle& operator=(BaseAssetHandle&& other)
      {
        if (asset_id_ >= 0) // Free existing asset
        {
          Release();
        }

        manager_ = other.manager_;
        asset_id_ = other.asset_id_;
        other.asset_id_ = -1;
        return *this;
      };

      void Release()
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

      bool operator !() const { return asset_id_ < 0; };
      operator bool() const { return asset_id_ >= 0; }
      operator int() const { return asset_id_; }

      virtual ~BaseAssetHandle() { Release(); }

    protected:
      int asset_id_;
      IAssetManager* manager_;
    };

    template <class AssetT>
    struct AssetHandle : public BaseAssetHandle
    {
      AssetHandle()
      {}
      AssetHandle(IAssetManager* manager, int asset_id) :
        BaseAssetHandle(manager, asset_id)
      {}
      ~AssetHandle() override { Release(); }

      AssetHandle(AssetHandle&& other) : BaseAssetHandle(eastl::move(other)) {};
      AssetHandle(const AssetHandle& other) : BaseAssetHandle(other) {};

      AssetHandle& operator=(const AssetHandle& other)
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
      };

      AssetHandle& operator=(AssetHandle&& other) {
        if (asset_id_ >= 0) // Free existing asset
        {
          Release();
        }

        manager_ = other.manager_;
        asset_id_ = other.asset_id_;
        other.asset_id_ = -1;
        return *this;
      };

      AssetT* operator->() const { return GetRaw(); }

      AssetT* GetRaw() const
      {
        assert(asset_id_ >= 0);
        return reinterpret_cast<AssetT*>(manager_->GetAsset(*this));
      }

      GPUAssetHandle& GetGPUHandle() const
      {
        assert(asset_id_ >= 0);
        return manager_->GetGPUHandle(*this);
      }
    };
  }
}
