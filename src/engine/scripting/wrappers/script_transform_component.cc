#include "engine/scripting/wrappers/script_transform_component.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

namespace sulphur
{
  namespace engine
  {
    foundation::SharedPointer<ScriptableValue>
      ScriptTransformComponent::Instantiate(ScriptSystem* script_system, TransformComponent transform_component)
    {
      World& world = script_system->application().GetService<WorldProviderSystem>().GetWorld();

      TransformComponent* component
        = reinterpret_cast<TransformComponent*>(lua_newuserdata(script_system->lua_state(), sizeof(TransformComponent)));

      // Construct component in-place so it has a valid handle and reference to its system
      new (component) TransformComponent(world.GetSystem<TransformSystem>(), transform_component.handle);

      foundation::SharedPointer<ScriptableValue> script_component = script_system->GetFromStack(-1);
      SetMetaTable(script_component, Name());

      return script_component;
    }

    void ScriptTransformComponent::SetParent(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      TransformComponent* parent = CheckUserData<TransformComponent>(Name(), args->GetArg(1));

      component->SetParent(*parent);
    }

    void ScriptTransformComponent::UnsetParent(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      component->UnsetParent();
    }

    void ScriptTransformComponent::AttachChild(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      TransformComponent* child = CheckUserData<TransformComponent>(Name(), args->GetArg(1));

      component->AttachChild(*child);
    }

    void ScriptTransformComponent::DetachChild(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      TransformComponent* child = CheckUserData<TransformComponent>(Name(), args->GetArg(1));

      component->DetachChild(*child);
    }

    void ScriptTransformComponent::HasParent(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), component->HasParent()));
    }

    void ScriptTransformComponent::GetParent(ScriptableArgs* args)
    {

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->GetParent();

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::GetSiblings(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      foundation::Vector<TransformComponent> siblings = component->GetSiblings();
      foundation::SharedPointer<ScriptableValue> script_siblings = ScriptUtils::CreateTable(args->script_system());
      for (int i = 0; i < siblings.size(); ++i)
      {
        ScriptUtils::SetObjectValue(
          script_siblings,
          ScriptUtils::Instantiate(args->script_system(), i),
          Instantiate(args->script_system(), siblings[i]));
      }
      args->AddReturn(script_siblings);
    }

    void ScriptTransformComponent::GetChildren(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      foundation::Vector<TransformComponent> children = component->GetChildren();
      foundation::SharedPointer<ScriptableValue> script_children = ScriptUtils::CreateTable(args->script_system());
      for (int i = 0; i < children.size(); ++i)
      {
        ScriptUtils::SetObjectValue(
          script_children,
          ScriptUtils::Instantiate(args->script_system(), i),
          Instantiate(args->script_system(), children[i]));
      }
      args->AddReturn(script_children);
    }

    void ScriptTransformComponent::SetWorldPosition(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->SetWorldPosition(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetWorldScale(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->SetWorldScale(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetWorldRotationEuler(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->SetWorldRotationEuler(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );
      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetLocalPosition(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->SetLocalPosition(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    //void ScriptTransformComponent::SetLocalRotation(ScriptableArgs* args)
    //{
    //  args->Check("UNNN");
    //  TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
    //
    //  args->AddReturn(Instantiate(args->script_system(), component->SetLocalRotation(
    //    glm::vec3(
    //      ScriptUtils::As<float>(args->GetArg(1)),
    //      ScriptUtils::As<float>(args->GetArg(1)),
    //      ScriptUtils::As<float>(args->GetArg(2))
    //    )
    //  )));
    //}

    void ScriptTransformComponent::SetLocalScale(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->SetLocalScale(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetLocalRotationEuler(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->SetLocalRotationEuler(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );
      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::GetWorldPosition(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetWorldPosition();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetWorldScale(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetWorldScale();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetWorldRotationEuler(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetWorldRotationEuler();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetLocalPosition(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetLocalPosition();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetLocalScale(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetLocalScale();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetLocalRotationEuler(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetLocalRotationEuler();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::SetWorldUp(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 right = glm::vec3(1, 0, 0);
      if (args->NumArgs() == 7)
      {
        right = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }
      component->SetWorldUp(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        right
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetWorldRight(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 forward = glm::vec3(0, 0, 1);
      if (args->NumArgs() == 7)
      {
        forward = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }

      component->SetWorldRight(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        forward
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetWorldForward(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 up = glm::vec3(0, 1, 0);
      if (args->NumArgs() == 7)
      {
        up = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }
      component->SetWorldForward(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        up
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetLocalUp(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 right = glm::vec3(1, 0, 0);
      if (args->NumArgs() == 7)
      {
        right = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }

      component->SetLocalUp(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        right
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetLocalRight(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 forward = glm::vec3(0, 0, 1);
      if (args->NumArgs() == 7)
      {
        forward = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }

      component->SetWorldRight(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        forward
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::SetLocalForward(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 up = glm::vec3(0, 1, 0);
      if (args->NumArgs() == 7)
      {
        up = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }

      component->SetLocalForward(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        up
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::GetWorldUp(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetWorldUp();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetWorldRight(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetWorldRight();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetWorldForward(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetWorldForward();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetLocalUp(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetLocalUp();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetLocalRight(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetLocalRight();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::GetLocalForward(ScriptableArgs* args)
    {
      args->Check("U");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->GetLocalForward();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::TranslateWorld(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->TranslateWorld(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::ScaleWorld(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->ScaleWorld(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateWorldEuler(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateWorldEuler(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::TranslateLocal(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->TranslateLocal(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::ScaleLocal(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->ScaleLocal(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateLocalEuler(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateLocalEuler(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        )
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateWorldX(ScriptableArgs* args)
    {
      args->Check("UN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateWorldX(
        ScriptUtils::As<float>(args->GetArg(1))
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateWorldY(ScriptableArgs* args)
    {
      args->Check("UN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateWorldY(
        ScriptUtils::As<float>(args->GetArg(1))
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateWorldZ(ScriptableArgs* args)
    {
      args->Check("UN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateWorldZ(
        ScriptUtils::As<float>(args->GetArg(1))
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateAroundWorld(ScriptableArgs* args)
    {
      args->Check("UNNNNNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));


      component->RotateAroundWorld(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        ),
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6))
        ),
        ScriptUtils::As<float>(args->GetArg(7)));

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateLocalX(ScriptableArgs* args)
    {
      args->Check("UN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateLocalX(
        ScriptUtils::As<float>(args->GetArg(1))
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateLocalY(ScriptableArgs* args)
    {
      args->Check("UN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateLocalY(
        ScriptUtils::As<float>(args->GetArg(1))
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateLocalZ(ScriptableArgs* args)
    {
      args->Check("UN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      component->RotateLocalZ(
        ScriptUtils::As<float>(args->GetArg(1))
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::RotateAroundLocal(ScriptableArgs* args)
    {
      args->Check("UNNNNNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));


      component->RotateAroundLocal(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))
        ),
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6))
        ),
        ScriptUtils::As<float>(args->GetArg(7)));

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::TransformPoint(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->TransformPoint(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::TransformVector(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->TransformVector(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::TransformLocalPoint(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->TransformLocalPoint(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::TransformLocalVector(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->TransformLocalVector(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::InverseTransformPoint(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->InverseTransformPoint(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::InverseTransformVector(ScriptableArgs* args)
    {
      args->Check("UNNN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->InverseTransformVector(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptTransformComponent::Concatenate(ScriptableArgs* args)
    {
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      TransformComponent* other = CheckUserData<TransformComponent>(Name(), args->GetArg(1));

      component->Concatenate(*other);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::Blend(ScriptableArgs* args)
    {
      args->Check("UUN");
      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));
      TransformComponent* other = CheckUserData<TransformComponent>(Name(), args->GetArg(1));
      float alpha = ScriptUtils::As<float>(args->GetArg(2));
      component->Blend(*other, alpha);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::LookAt(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 up = glm::vec3(0, 1, 0);
      if (args->NumArgs() == 7)
      {
        up = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }

      component->LookAt(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        up
      );

      args->AddReturn(args->GetArg(0));
    }

    void ScriptTransformComponent::LookAtLocal(ScriptableArgs* args)
    {
      if (args->NumArgs() == 7)
      {
        args->Check("UNNNNNN");
      }
      else
      {
        args->Check("UNNN", 4);
      }

      TransformComponent* component = CheckUserData<TransformComponent>(Name(), args->GetArg(0));

      glm::vec3 up = glm::vec3(0, 1, 0);
      if (args->NumArgs() == 7)
      {
        up = glm::vec3(ScriptUtils::As<float>(args->GetArg(4)),
          ScriptUtils::As<float>(args->GetArg(5)),
          ScriptUtils::As<float>(args->GetArg(6)));
      }

      component->LookAtLocal(
        glm::vec3(
          ScriptUtils::As<float>(args->GetArg(1)),
          ScriptUtils::As<float>(args->GetArg(2)),
          ScriptUtils::As<float>(args->GetArg(3))),
        up
      );

      args->AddReturn(args->GetArg(0));
    }
  }
}
