#pragma once

#include <foundation/utils/asset_definitions.h>

#include <assert.h>

namespace sulphur
{
  namespace engine
  {
    class Application;
    class BaseAssetHandle;
    class GPUAssetHandle;

    /**
     * @class sulphur::engine::IGPUAssetManager
     * @brief Interface for asset managers that reference data used by the renderer.
     * @author Jelle de Haan
     */
    class IGPUAssetManager
    {
    public:
      /**
       * @brief Releases the asset in the renderer.
       * @param[in] handle (sulphur::engine::GPUAssetHandle&) A handle to a GPU asset.
       */
      virtual void Release(GPUAssetHandle& handle) = 0;
    };

    /**
     * @class sulphur::engine::GPUAssetHandle
     * @brief A handle that references data used by the renderer.
     * @author Jelle de Haan
     */
    class GPUAssetHandle
    {
    public:
      /**
       * @brief Default constructor.
       */
      GPUAssetHandle();

      /**
       * @brief Constructor
       * @param[in] manager (sulphur::engine::IGPUAssetManager*) The manager that manages this asset.
       * @param[in] asset_ptr (uintptr_t) Handle to the asset.
       */
      GPUAssetHandle(IGPUAssetManager* manager, uintptr_t asset_ptr);

      /**
       * @brief Release the asset.
       */
      void Release();

      /**
       * @brief Implicit bool conversion operator.
       */
      operator bool() const;

      /**
       * @brief Implicit uintptr_t convertsion operator.
       */
      operator uintptr_t() const;

    private:
      uintptr_t ptr_;             //!< Handle to the asset.
      IGPUAssetManager* manager_; //!< The manager that owns the asset.
    };

    /**
     * @class sulphur::engine::IAssetManager
     * @brief Interface for asset managers.
     * @author Timo van Hees
     */
    class IAssetManager
    {
    public:
      /**
       * @brief Default constructor.
       */
      IAssetManager() = default;
      IAssetManager(const IAssetManager&) = delete; //!< Deleted copy constructor.

      /**
       * @brief Default virtual destructor.
       */
      virtual ~IAssetManager() = default;
      /**
      * @brief Initializes the manager. Loads the package cache from disk.
      * @param[in] application (sulphur::engine::Application&) The application that owns the asset managers.
      */
      virtual void Initialize(Application& application) = 0;
      /**
       * @brief Deletes all assets owned by the manager.
       */
      virtual void Shutdown() = 0;

      IAssetManager& operator=(const IAssetManager&) = delete; //!< Deleted copy-assignment operator.

      /**
      * @brief Re-loads the package cache from disk.
      */
      virtual void RefreshCache() = 0;

      /**
       * @brief Get the handle of an asset by ID.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (int) A handle to the asset or -1.
       * @remark -1 means that the there is no asset with this id.
       */
      virtual int GetHandle(foundation::AssetID id) = 0;
      /**
      * @brief Get the handle of an asset by name.
      * @param[in] name (sulphur::foundation::AssetName) The name of the asset.
      * @return (int) A handle to the asset or -1.
      * @remark -1 means that the there is no asset with this name.
      */
      virtual int GetHandle(const foundation::AssetName& name) = 0;
      /**
      * @brief Load an asset from the package by ID.
      * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
      * @return (int) A handle to the asset or -1.
      * @remark If the asset is already loaded it will return the handle without reloading the asset.
      * @remark -1 means that the there is no asset with this id.
      */
      virtual int Load(foundation::AssetID id) = 0;
      /**
      * @brief Load an asset from the package by name.
      * @param[in] name (sulphur::foundation::AssetName) The name of the asset.
      * @return (int) A handle to the asset or -1.
      * @remark If the asset is already loaded it will return the handle without reloading the asset.
      * @remark -1 means that the there is no asset with this name.
      */
      virtual int Load(const foundation::AssetName& name) = 0;
      /**
       * @brief Releases all assets that reference data used by the renderer.
       */
      virtual void ReleaseGPUHandles() = 0;
      /**
       * @brief Release an asset by ID.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset to release.
       * @remark All handles that point to this asset will be invalidated.
       */
      virtual void Release(foundation::AssetID id) = 0;
      /**
      * @brief Release an asset by name.
      * @param[in] name (sulphur::foundation::AssetName) The name of the asset to release.
      * @remark All handles that point to this asset will be invalidated.
      */
      virtual void Release(const foundation::AssetName& name) = 0;
      /**
       * @brief Increase the number of references to an asset.
       * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
       */
      virtual void IncreaseRef(const BaseAssetHandle& handle) = 0;
      /**
      * @brief Decrease the number of references to an asset.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      */
      virtual void DecreaseRef(const BaseAssetHandle& handle) = 0;
      /**
      * @brief Get the GPU asset handle from the asset handle.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      */
      virtual GPUAssetHandle& GetGPUHandle(const BaseAssetHandle& handle) = 0;
      /**
       * @brief Get the number of references to an asset by ID.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (int) The number of references to the asset.
       */
      virtual int GetReferenceCount(foundation::AssetID id) = 0;
      /**
      * @brief Get the number of references to an asset by name.
      * @param[in] name (sulphur::foundation::AssetName) The name of the asset.
      * @return (int) The number of references to the asset.
      */
      virtual int GetReferenceCount(const foundation::AssetName& name) = 0;
      /**
      * @brief Get the number of references to an asset by handle.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      * @return (int) The number of references to the asset.
      */
      virtual int GetReferenceCount(const BaseAssetHandle& handle) = 0;
      /**
      * @brief Check if the handle is valid.
      * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
      * @return (bool) If the handle points to a valid asset.
      */
      virtual bool ValidateHandle(const BaseAssetHandle& handle) const = 0;
      /**
       * @brief Get a raw pointer to the asset by handle.
       * @param[in] handle (const sulphur::engine::BaseAssetHandle&) A handle to the asset.
       * @return (void*) Pointer to the asset.
       */
      virtual void* GetAsset(const BaseAssetHandle& handle) const = 0;
    };

    /**
     * @class sulphur::engine::BaseAssetHandle
     * @brief Base class for asset handles.
     * @author Timo van Hees
     */
    class BaseAssetHandle
    {
    public:
      /**
       * @brief Default constructor.
       */
      BaseAssetHandle();

      /**
       * @brief Constructor.
       * @param[in] manager (sulphur::engine::IAssetManager*) The manager that owns the asset.
       * @param[in] asset_id (int) The id of the asset in the manager.
       */
      BaseAssetHandle(IAssetManager* manager, int asset_id);

      /**
       * @brief Copy constructor.
       */
      BaseAssetHandle(const BaseAssetHandle& other);

      /**
       * @brief Move constructor.
       */
      BaseAssetHandle(BaseAssetHandle&& other);

      /**
       * @brief Virtual destructor.
       */
      virtual ~BaseAssetHandle();

      /**
       * @brief Copy assignment operator.
       */
      BaseAssetHandle& operator=(const BaseAssetHandle& other);

      /**
       * @brief Move assignment operator.
       */
      BaseAssetHandle& operator=(BaseAssetHandle&& other);

      /**
       * @brief Negation operator.
       */
      bool operator !() const;
      /**
       * @brief Implicit bool conversion operator.
       */
      operator bool() const;
      /**
       * @brief Implicit int conversion operator.
       */
      operator int() const;

      /**
       * @brief Equal to operator.
       */
      bool operator==(const BaseAssetHandle& other) const;

      /**
      * @brief Less than operator.
      * @param[in] other (const sulphur::engine::BaseAssetHandle&) The handle to compare against.
      * @return (bool) True if this handle's asset ID is less than the other handle.
      */
      bool operator<(const BaseAssetHandle& other) const;

      /**
       * @brief Release and invalidate the handle.
       */
      void Release();

    protected:
      int asset_id_;            //!< The id of the asset in the manager.
      IAssetManager* manager_;  //!< The manager that owns the asset.
    };

    /**
     * @class sulphur::engine::AssetHandle : public sulphur::engine::BaseAssetHandle
     * @brief Typed handle to an asset owned by an asset manager.
     * @tparam AssetT The type of asset the handle references.
     * @author Timo van Hees
     */
    template <class AssetT>
    class AssetHandle : public BaseAssetHandle
    {
      /**
       * @brief Empty helper struct used for template magic.
       */
      struct Empty{};

    public:
      /**
       * @brief Default constructor.
       */
      AssetHandle();
      /**
       * @brief Constructor.
       * @param manager (sulphur::engine::IAssetManager*) The manager that owns the asset.
       * @param asset_id (int) The id of the asset in the manager.
       */
      AssetHandle(IAssetManager* manager, int asset_id);
      /**
       * @brief Copy constructor.
       */
      AssetHandle(const AssetHandle& other);

      /**
       * @brief Move constructor.
       */
      AssetHandle(AssetHandle&& other);
      /**
       * @brief Destructor.
       */
      ~AssetHandle() override { Release(); }


      /**
       * @brief Copy assignment operator.
       */
      AssetHandle& operator=(const AssetHandle& other);

      /**
      * @brief Move assignment operator.
      */
      AssetHandle& operator=(AssetHandle&& other);

      /**
       * @brief Type of void handle specific functions. 
       * Type is not void when AssetT == void to stop multiple definitions error.
       */
      using void_type = typename std::conditional<std::is_same<AssetT, void>::value, 
        Empty, void>::type;

      /**
      * @brief Copy constructor from AssetHandle<void>.
      */
      AssetHandle(const AssetHandle<void_type>& other);

      /**
      * @brief Move constructor from AssetHandle<void>.
      */
      AssetHandle(AssetHandle<void_type>&& other);

      /**
       * @brief Implicit conversion operator to asset handle of type T.
       * @tparam T The the type of the other asset handle.
       * @remark Only implicit cast to AssetHandle<void> is allowed.
       */
      template<typename T>
      operator AssetHandle<T>() const;

      /**
       * @brief Copy assignment operator for AssetHandle<void>
       */
      AssetHandle& operator=(const AssetHandle<void_type>& other);

      /**
       * @brief Move assignment operator for AssetHandle<void>
       */
      AssetHandle& operator=(AssetHandle<void_type>&& other);

      /**
       * @brief Member of pointer operator.
       */
      AssetT* operator->() const;

      /**
       * @brief Get a raw pointer to the asset referenced by this handle.
       * @return (AssetT*) Pointer to asset.
       */
      AssetT* GetRaw() const;

      /**
       * @brief Get handle to GPU asset.
       * @return (sulphur::engnie::GPUAssetHandle&) GPU asset handle.
       */
      GPUAssetHandle& GetGPUHandle() const;
    };

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>::AssetHandle()
    {}

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>::AssetHandle(IAssetManager* manager, int asset_id):
      BaseAssetHandle(manager, asset_id)
    {
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>::AssetHandle(const AssetHandle& other): 
    BaseAssetHandle(other)
    {
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>::AssetHandle(AssetHandle&& other): 
    BaseAssetHandle(eastl::move(other))
    {
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>& AssetHandle<AssetT>::operator=(const AssetHandle& other)
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
    template <class AssetT>
    AssetHandle<AssetT>& AssetHandle<AssetT>::operator=(AssetHandle&& other)
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
    template <class AssetT>
    AssetHandle<AssetT>::AssetHandle(const AssetHandle<void_type>& other): 
    BaseAssetHandle(other)
    {
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>::AssetHandle(AssetHandle<void_type>&& other): 
    BaseAssetHandle(eastl::move(other))
    {
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    template <typename T>
    AssetHandle<AssetT>::operator AssetHandle<T>() const
    {
      static_assert(eastl::is_same<T, void>::value, "Only implicit cast to AssetHandle<void> is allowed.");
      return AssetHandle<T>(manager_, asset_id_);
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetHandle<AssetT>& AssetHandle<AssetT>::operator=(const AssetHandle<void_type>& other)
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
    template <class AssetT>
    AssetHandle<AssetT>& AssetHandle<AssetT>::operator=(AssetHandle<void_type>&& other)
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
    template <class AssetT>
    AssetT* AssetHandle<AssetT>::operator->() const
    {
      return GetRaw();
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    AssetT* AssetHandle<AssetT>::GetRaw() const
    {
      assert(asset_id_ >= 0);
      return reinterpret_cast<AssetT*>(manager_->GetAsset(*this));
    }

    //--------------------------------------------------------------------------------
    template <class AssetT>
    GPUAssetHandle& AssetHandle<AssetT>::GetGPUHandle() const
    {
      assert(asset_id_ >= 0);
      return manager_->GetGPUHandle(*this);
    }
  }
}
