#include "system.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    //! @TODO(Kenneth): Set the parent resource to the world
    ISystemBase::ISystemBase(const char* name)
      : BaseResource(name)
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnInitialize(Application&, foundation::JobGraph&)
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnTerminate()
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnUpdate(float)
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnFixedUpdate()
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnLateUpdate()
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnPreRender()
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnRender()
    {
    }

    //--------------------------------------------------------------------------
    void ISystemBase::OnPostRender()
    {
    }
  }
}