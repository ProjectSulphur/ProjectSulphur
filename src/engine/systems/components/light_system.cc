#include "light_system.h"

#include "engine/application/application.h"
#include "engine/rewinder/rewind_system.h"
#include "engine/rewinder/system_stored_data.h"
#include <glm/gtc/matrix_transform.hpp>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    LightComponent::LightComponent() :
      system_(nullptr)
    {
    }

    //------------------------------------------------------------------------------------------------------
    LightComponent::LightComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //------------------------------------------------------------------------------------------------------
    LightSystem::LightSystem() :
      IComponentSystem("LightSystem")
    {
    }
    //-------------------------------------------------------------------------
    template<>
    inline void* Store( foundation::Color* buffer, size_t size )
    {
      void* raw_array = foundation::Memory::Allocate( size * sizeof( foundation::Color ), 64 );
      memcpy_s( raw_array, size * sizeof( foundation::Color ), buffer, size * sizeof( foundation::Color ) );
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore( foundation::Color* buffer, void* old, size_t size )
    {
      memcpy_s( buffer, size * sizeof( foundation::Color ), old, size * sizeof( foundation::Color ) );
    }
    //-------------------------------------------------------------------------
    template<>
    inline void* Store( float* buffer, size_t size )
    {
      void* raw_array = foundation::Memory::Allocate( size * sizeof( float ), 64 );
      memcpy_s( raw_array, size * sizeof( float ), buffer, size * sizeof( float ) );
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore( float* buffer, void* old, size_t size )
    {
      memcpy_s( buffer, size * sizeof( float ), old, size * sizeof( float ) );
    }
    //-------------------------------------------------------------------------
    template<>
    inline void* Store( LightType* buffer, size_t size )
    {
      void* raw_array = foundation::Memory::Allocate( size * sizeof( LightType ), 64 );
      memcpy_s( raw_array, size * sizeof( LightType ), buffer, size * sizeof( LightType ) );
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore( LightType* buffer, void* old, size_t size )
    {
      memcpy_s( buffer, size * sizeof( LightType ), old, size * sizeof( LightType ) );
    }
    //------------------------------------------------------------------------------------------------------
    void LightSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      renderer_ = &app.platform_renderer();
      rewind_storage_ = foundation::Memory::Construct<RewindStorage>( &component_data_.data,
        StoreFunc<foundation::Color>(),
        StoreFunc<float>(),
        StoreFunc<float>(),
        StoreFunc<float>(),
        StoreFunc<LightType>() );
      app.GetService<RewindSystem>().Register( *rewind_storage_ );
    }
  }
}