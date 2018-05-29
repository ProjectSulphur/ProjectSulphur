#pragma once

#include "asset_interfaces.h"
#include "engine/application/application.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>
#include <foundation/containers/deque.h>
#include <foundation/logging/logger.h>

namespace sulphur 
{
  namespace engine 
  {
    /**
     * @brief Base asset manager that asset managers should derive from. 
     * Handles all of the work managing the assets.
     * @tparam T The asset type this manager needs to manager.
     */
    template<class T>
    class BaseAssetManager : public IAssetManager
    {
    protected:
      /**
       * @brief A handle to a reference of an asset and a reference counter.
       */
      struct ReferenceHandle
      {
        /**
         * @brief Creates an invalid reference handle.
         */
        ReferenceHandle():
          handle(-1),
          ref_count(-1)
        {}

        /**
         * @brief Creates a reference handle.
         * @param[in] handle (int) The index of the reference of the asset.
         * @param[in] ref_count (int) The number of references to the asset.
         */
        ReferenceHandle(int handle, int ref_count) :
          handle(handle),
          ref_count(ref_count)
        {}

        int handle;                 //!< The index of the reference to the asset.
        GPUAssetHandle gpu_handle;  //!< The GPU handle.
        int ref_count;              //!< Number of handles referencing this handle.
      };

    public:
      /**
       * @see sulphur::engine::IAssetManager::Initialize
       */
      void Initialize(Application& application) override;

      /**
      * @brief Releases all assets
      */
      void Shutdown() override;

      /**
      * @brief Re-loads the package cache from disk.
      */
      void RefreshCache() override;

      /**
       * @brief Adds an asset to the manager.
       * @param[in] asset (T*) Pointer to the unmanaged asset.
       * @param[in] name (const sulphur::foundation::AssetName&)
       * @return (sulphur::engine::AssetHandle<T>) A handle to a managed asset.
       */
      AssetHandle<T> Add(T* asset, const foundation::AssetName& name);
      /**
      * @brief Get the index of a handle to an asset by ID.
      * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
      * @return (int) Index of a handle to an asset. If < 0 the handle is invalid.
      */
      int GetHandle(foundation::AssetID id) override;
      /**
      * @brief Get the index of a handle to an asset by name.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @return (int) Index of a handle to an asset. If < 0 the handle is invalid.
      */
      int GetHandle(const foundation::AssetName& name) override;
      /**
      * @brief Loads an asset from a package created with the builder by ID.
      * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
      * @return (int) Index of a handle to an asset. If < 0 the handle is invalid.
      */
      int Load(foundation::AssetID id) override;
      /**
      * @brief Loads an asset from a package created with the builder by name.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @return (int) Index of a handle to an asset. If < 0 the handle is invalid.
      */
      int Load(const foundation::AssetName& name) override;
      /**
      * @brief Releases all GPU handles and resets them to invalid without invalidating the CPU handles.
      */
      void ReleaseGPUHandles() override;
      /**
      * @brief Release an asset instantly by ID without invalidating the handles.
      * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
      */
      void Release(foundation::AssetID id) override;
      /**
      * @brief Release an asset instantly by name without invalidating the handles.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      */
      void Release(const foundation::AssetName& name) override;
      /**
       * @brief Increase the reference count of a handle.
       * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
       */
      void IncreaseRef(const BaseAssetHandle& handle) override;
      /**
      * @brief Decrease the reference count of a handle.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      */
      void DecreaseRef(const BaseAssetHandle& handle) override;
      /**
      * @brief Get the GPU handle of a handle.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      * @return (sulphur::engine::GPUAssetHandle&) A GPU handle.
      */
      GPUAssetHandle& GetGPUHandle(const BaseAssetHandle& handle) override;
      /**
      * @brief Get the number of references to a handle.
      * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
      * @return (int) The number of references to a handle.
      */
      int GetReferenceCount(foundation::AssetID id) override;
      /**
      * @brief Get the number of references to a handle.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @return (int) The number of references to a handle.
      */
      int GetReferenceCount(const foundation::AssetName& name) override;
      /**
      * @brief Get the number of references to a handle.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      * @return (int) The number of references to a handle.
      */
      int GetReferenceCount(const BaseAssetHandle& handle) override;
      /**
      * @brief Check if the handle and the asset are valid.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      * @return (bool) If the handle is valid.
      */
      bool ValidateHandle(const BaseAssetHandle& handle) const override;

    protected:
      /**
       * @brief Get the asset referenced to by the handle.
       * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
       * @return (void*) Pointer to the asset.
       */
      void* GetAsset(const BaseAssetHandle& handle) const override;
      /**
       * @brief Function to import an asset from a package.
       * @param[in] asset_file (const sulphur::foundation::Path&) The package 
       * containing the asset.
       * @return (T*) Unmanaged pointer to the imported asset.
       */
      virtual T* ImportAsset(const foundation::Path& asset_file) = 0;
      /**
      * @brief Asset pipelines deriving from this class override this method to
      * set the name of the package cache.
      * @return (const sulphur::foundation::String) The file name of package cache.
      */
      virtual const foundation::String GetCacheName() const = 0;
      /**
      * @brief Asset pipelines deriving from this class override this method to
      * set the location of the package cache.
      * @return (const sulphur::foundation::String) The file location of package cache.
      * @remark Location should end with a '/'.
      */
      virtual const foundation::String GetCacheLocation() const;

      Application* application_; //!< Keeps a pointer to the main application that owns everything.

    private:
      /**
       * @brief Add an asset to the manager.
       * @param[in] asset (T*) The asset to add.
       * @return (int) The index of the asset in the list of assets.
       */
      int AddAsset(T* asset);
      /**
       * @brief Add a handle to an asset to the manager.
       * @param[in] asset_slot (int) The index of the asset in the list.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (int) The index of the handle in the list of handles.
       */
      int AddHandle(int asset_slot, foundation::AssetID id);
      /**
       * @brief Delete an asset from the manager.
       * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
       */
      void DeleteAsset(ReferenceHandle& handle);
      /**
       * @brief Delete a handle from the manager.
       * @param[in] slot (int) The index of the handle.
       */
      void DeleteHandle(int slot);      

      public:
      foundation::Vector<T*> assets_;                             //!< List of assets.
      private:
      foundation::Vector<ReferenceHandle> asset_handles_;         //!< List of handles.
      foundation::Deque<int> unused_asset_slots_;                 //!< Queue of unused asset locations.
      foundation::Deque<int> unused_handle_slots_;                //!< Queue of unused handle locations.
      foundation::Map<foundation::AssetID, int> asset_locations_; //!< Map of locations of assets in the list of assets by ID.

      foundation::Map<foundation::AssetID, foundation::PackagePtr> packaged_assets_;  //!< Map with information about the packaged assets.
    };

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::Initialize(Application& application)
    {
      application_ = &application;
      RefreshCache();
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::RefreshCache()
    {
      const foundation::String cache_name = GetCacheName();
      if(cache_name.empty() == true)
      {
        return;
      }

      foundation::BinaryReader reader(application_->project_directory().path() + cache_name + ".cache");
      if (reader.is_ok() == true)
      {
        packaged_assets_ = reader.ReadMap<foundation::AssetID, foundation::PackagePtr>();
      }
    }

    //--------------------------------------------------------------------------------
    template<class T>
    inline void BaseAssetManager<T>::Shutdown()
    {
#ifdef _DEBUG
      if (!asset_locations_.empty()) {
        PS_LOG(Warning, "Asset system detected %i leaked assets", asset_locations_.size());
      }
#endif // DEBUG

      for (eastl::pair<const foundation::AssetID, int>& location : asset_locations_)
      {
        if (location.second >= 0)
        {
          DeleteAsset(asset_handles_[location.second]);
        }
      }
    }

    //--------------------------------------------------------------------------------
    template <class T>
    AssetHandle<T> BaseAssetManager<T>::Add(T* asset, const foundation::AssetName& name)
    {
      foundation::AssetID id = foundation::GenerateId(name);

      assert(asset != nullptr);
      assert(name.get_length() > 0);
      assert(asset_locations_.find(id) == asset_locations_.end());

#ifdef _DEBUG
      if(packaged_assets_.find(id) != packaged_assets_.end())
      {
        //PS_LOG(foundation::LineAndFileLogger, foundation::Verbosity::kAssert,
        //  "An asset with the same name exists in the package. By creating an asset with the same name you stop that asset from ever being loaded.");
      }
#endif

      const int asset_slot = AddAsset(asset);
      const int handle_slot = AddHandle(asset_slot, id);

      return AssetHandle<T>(this, handle_slot);
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::GetHandle(foundation::AssetID id)
    {
      const foundation::Map<foundation::AssetID, int>::iterator location = asset_locations_.
          find(id);
      if (location != asset_locations_.end())
      {
        return location->second;
      }

      return -1;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::GetHandle(const foundation::AssetName& name)
    {
      const foundation::AssetID id = foundation::GenerateId(name);
      return GetHandle(id);
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::Load(foundation::AssetID id)
    {
      const int handle = GetHandle(id);
      if(handle >= 0)
      {
        return handle;
      }

      foundation::Map<foundation::AssetID, foundation::PackagePtr>::iterator packaged_asset = 
        packaged_assets_.find(id);

      if(packaged_asset == packaged_assets_.end())
      {
        return -1;
      }

      T* asset = ImportAsset(packaged_asset->second.filepath);
      if(asset != nullptr)
      {
        const int asset_slot = AddAsset(asset);
        const int handle_slot = AddHandle(asset_slot, id);
        return handle_slot;
      }

      return -1;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::Load(const foundation::AssetName& name)
    {
      foundation::AssetID id = foundation::GenerateId(name);
      return Load(id);
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::ReleaseGPUHandles()
    {
      for (foundation::Map<foundation::AssetID, int>::iterator it = asset_locations_.begin(); 
        it != asset_locations_.end(); ++it)
      {
        const int slot = it->second;

        assert(slot >= 0 && slot < asset_handles_.size());
        assert(assets_[asset_handles_[slot].handle] != nullptr);

        ReferenceHandle& reference_handle = asset_handles_[slot];
        if (reference_handle.gpu_handle)
        {
          reference_handle.gpu_handle.Release();
          reference_handle.gpu_handle = sulphur::engine::GPUAssetHandle();
        }
      }
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::Release(foundation::AssetID id)
    {
      const int handle = GetHandle(id);
      if(handle >= 0)
      {
        DeleteAsset(asset_handles_[handle]);
      }
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::Release(const foundation::AssetName& name)
    {
      foundation::AssetID id = foundation::GenerateId(name);
      Release(id);
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::IncreaseRef(const BaseAssetHandle& handle)
    {
      int slot = handle;

      assert(slot >= 0 && slot < asset_handles_.size());
      assert(assets_[asset_handles_[slot].handle] != nullptr);
      auto& ref = asset_handles_[slot];
      ++ref.ref_count;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::DecreaseRef(const BaseAssetHandle& handle)
    {
      int slot = handle;

      assert(slot >= 0 && slot < asset_handles_.size());
      assert(assets_[asset_handles_[slot].handle] != nullptr);

      ReferenceHandle& reference_handle = asset_handles_[slot];
      --reference_handle.ref_count;
      if (reference_handle.handle >= 0 && reference_handle.ref_count <= 0)
      {
        DeleteAsset(reference_handle);
        DeleteHandle(slot);
        return;
      }
    }

    //--------------------------------------------------------------------------------
    template <class T>
    GPUAssetHandle& BaseAssetManager<T>::GetGPUHandle(const BaseAssetHandle& handle)
    {
      int slot = handle;

      assert(slot >= 0 && slot < asset_handles_.size());
      assert(assets_[asset_handles_[slot].handle] != nullptr);

      ReferenceHandle& reference_handle = asset_handles_[slot];
      return reference_handle.gpu_handle;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::GetReferenceCount(foundation::AssetID id)
    {
      const int handle = GetHandle(id);
      if (handle >= 0)
      {
        return asset_handles_[handle].ref_count;
      }
      return 0;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::GetReferenceCount(const foundation::AssetName& name)
    {
      foundation::AssetID id = foundation::GenerateId(name);
      return GetReferenceCount(id);
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::GetReferenceCount(const BaseAssetHandle& handle)
    {
      const int slot = handle;
      assert(slot >= 0 && slot < asset_handles_.size());
      return asset_handles_[slot].ref_count;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    bool BaseAssetManager<T>::ValidateHandle(const BaseAssetHandle& handle) const
    {
      const int slot = handle;
      if (slot < 0 || slot >= asset_handles_.size())
        return false;

      const ReferenceHandle& reference_handle = asset_handles_[slot];
      if (reference_handle.handle < 0 || reference_handle.handle >= assets_.size())
        return false;

      if (assets_[reference_handle.handle] == nullptr)
        return false;

      return reference_handle.ref_count > 0;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void* BaseAssetManager<T>::GetAsset(const BaseAssetHandle& handle) const
    {
      const int slot = handle;
      assert(slot >= 0 && slot < asset_handles_.size());
      return assets_[asset_handles_[slot].handle];
    }

    //--------------------------------------------------------------------------------
    template <class T>
    inline const foundation::String BaseAssetManager<T>::GetCacheLocation() const
    {
      return "./";
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::AddAsset(T* asset)
    {
      assert(asset != nullptr);

      if (assets_.empty() == true)
      {
        assets_.push_back(asset);
        return 0;
      }

      if (unused_asset_slots_.empty() == true)
      {
        size_t prev_size = assets_.size();
        if (prev_size == 0)
        {
          prev_size = 1;
        }
        assert(prev_size * 2 <INT_MAX);
        assets_.resize(prev_size * 2);
        for (size_t i = prev_size; i < assets_.size(); ++i)
          unused_asset_slots_.push_back(static_cast<int>(i));
      }

      const int slot = unused_asset_slots_.front();
      unused_asset_slots_.pop_front();
      assets_[slot] = asset;
      return slot;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    int BaseAssetManager<T>::AddHandle(int asset_slot, foundation::AssetID id)
    {
      assert(assets_[asset_slot] != nullptr);
      assert(id != 0);
      assert(asset_locations_.find(id) == asset_locations_.end());

      if (asset_handles_.empty() == true)
      {
        asset_handles_.push_back(ReferenceHandle(asset_slot, 0));
        asset_locations_[id] = 0;
        return 0;
      }

      if (unused_handle_slots_.empty() == true)
      {
        size_t prev_size = asset_handles_.size();
        assert(prev_size * 2 < INT_MAX);
        asset_handles_.resize(prev_size * 2);
        for (size_t i = prev_size; i < asset_handles_.size(); ++i)
          unused_handle_slots_.push_back(static_cast<int>(i));
      }

      const int slot = unused_handle_slots_.front();
      unused_handle_slots_.pop_front();

      asset_handles_[slot] = ReferenceHandle(asset_slot, 0);

      asset_locations_[id] = slot;

      return slot;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::DeleteAsset(ReferenceHandle& handle)
    {
      assert(handle.handle >= 0);
      assert(assets_[handle.handle] != nullptr);

      if (handle.gpu_handle)
      {
        handle.gpu_handle.Release();
        handle.gpu_handle = GPUAssetHandle();
      }

      foundation::Memory::Destruct(assets_[handle.handle]);
      unused_asset_slots_.push_back(handle.handle);

      handle.handle = -1;
    }

    //--------------------------------------------------------------------------------
    template <class T>
    void BaseAssetManager<T>::DeleteHandle(int slot)
    {
      // TODO: Find a better solution to do this
      for (foundation::Map<foundation::AssetID, int>::iterator it = asset_locations_.begin(); 
        it != asset_locations_.end(); ++it)
      {
        if (it->second == slot)
        {
          asset_locations_.erase(it);
          break;
        }
      }

      ReferenceHandle& handle = asset_handles_[slot];
      handle.handle = -1;

      unused_handle_slots_.push_back(slot);
    }
  }
}
