#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/systems/components/transform_system.h"
#include "engine/scripting/script_system.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptTransformComponent : public ScriptableObject
    {
    public:
      static foundation::SharedPointer<ScriptableValue> Instantiate(ScriptSystem* script_system, TransformComponent transform_component);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "TransformComponent";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return{};
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "SetParent" , SetParent },
          { "UnsetParent" , UnsetParent },
          { "AttachChild" , AttachChild },
          { "DetachChild" , DetachChild },
          { "HasParent" , HasParent },
          { "GetParent" , GetParent },
          { "GetSiblings" , GetSiblings },
          { "GetChildren" , GetChildren },
          { "SetWorldPosition" , SetWorldPosition },
          { "SetWorldScale" , SetWorldScale },
          { "SetWorldRotationEuler" , SetWorldRotationEuler },
          { "SetLocalPosition" , SetLocalPosition },
          { "SetLocalScale" , SetLocalScale },
          { "SetLocalRotationEuler" , SetLocalRotationEuler },
          { "GetWorldPosition" , GetWorldPosition },
          { "GetWorldScale" , GetWorldScale },
          { "GetWorldRotationEuler" , GetWorldRotationEuler },
          { "GetLocalPosition" , GetLocalPosition },
          { "GetLocalScale" , GetLocalScale },
          { "GetLocalRotationEuler" , GetLocalRotationEuler },
          { "SetWorldUp" , SetWorldUp },
          { "SetWorldRight" , SetWorldRight },
          { "SetWorldForward" , SetWorldForward },
          { "SetLocalUp" , SetLocalUp },
          { "SetLocalRight" , SetLocalRight },
          { "SetLocalForward" , SetLocalForward },
          { "GetWorldUp" , GetWorldUp },
          { "GetWorldRight" , GetWorldRight },
          { "GetWorldForward" , GetWorldForward },
          { "GetLocalUp" , GetLocalUp },
          { "GetLocalRight" , GetLocalRight },
          { "GetLocalForward" , GetLocalForward },
          { "TranslateWorld" , TranslateWorld },
          { "ScaleWorld" , ScaleWorld },
          { "RotateWorldEuler" , RotateWorldEuler },
          { "TranslateLocal" , TranslateLocal },
          { "ScaleLocal" , ScaleLocal },
          { "RotateLocalEuler" , RotateLocalEuler },
          { "RotateWorldX" , RotateWorldX },
          { "RotateWorldY" , RotateWorldY },
          { "RotateWorldZ" , RotateWorldZ },
          { "RotateAroundWorld" , RotateAroundWorld },
          { "RotateLocalX" , RotateLocalX },
          { "RotateLocalY" , RotateLocalY },
          { "RotateLocalZ" , RotateLocalZ },
          { "RotateAroundLocal" , RotateAroundLocal },
          { "TransformPoint" , TransformPoint },
          { "TransformVector" , TransformVector },
          { "TransformLocalPoint" , TransformLocalPoint },
          { "TransformLocalVector" , TransformLocalVector },
          { "InverseTransformPoint" , InverseTransformPoint },
          { "InverseTransformVector" , InverseTransformVector },
          { "Concatenate" , Concatenate },
          { "Blend" , Blend },
          { "LookAt" , LookAt },
          { "LookAtLocal" , LookAtLocal }
        };
      }


      static void SetParent(ScriptableArgs* args);
      static void UnsetParent(ScriptableArgs* args);
      static void AttachChild(ScriptableArgs* args);
      static void DetachChild(ScriptableArgs* args);
      static void HasParent(ScriptableArgs* args);
      static void GetParent(ScriptableArgs* args);
      static void GetSiblings(ScriptableArgs* args);
      static void GetChildren(ScriptableArgs* args);
      static void SetWorldPosition(ScriptableArgs* args);
      static void SetWorldScale(ScriptableArgs* args);
      static void SetWorldRotationEuler(ScriptableArgs* args);
      static void SetLocalPosition(ScriptableArgs* args);
      static void SetLocalScale(ScriptableArgs* args);
      static void SetLocalRotationEuler(ScriptableArgs* args);
      static void GetWorldPosition(ScriptableArgs* args);
      static void GetWorldScale(ScriptableArgs* args);
      static void GetWorldRotationEuler(ScriptableArgs* args);
      static void GetLocalPosition(ScriptableArgs* args);
      static void GetLocalScale(ScriptableArgs* args);
      static void GetLocalRotationEuler(ScriptableArgs* args);
      static void SetWorldUp(ScriptableArgs* args);
      static void SetWorldRight(ScriptableArgs* args);
      static void SetWorldForward(ScriptableArgs* args);
      static void SetLocalUp(ScriptableArgs* args);
      static void SetLocalRight(ScriptableArgs* args);
      static void SetLocalForward(ScriptableArgs* args);
      static void GetWorldUp(ScriptableArgs* args);
      static void GetWorldRight(ScriptableArgs* args);
      static void GetWorldForward(ScriptableArgs* args);
      static void GetLocalUp(ScriptableArgs* args);
      static void GetLocalRight(ScriptableArgs* args);
      static void GetLocalForward(ScriptableArgs* args);
      static void TranslateWorld(ScriptableArgs* args);
      static void ScaleWorld(ScriptableArgs* args);
      static void RotateWorldEuler(ScriptableArgs* args);
      static void TranslateLocal(ScriptableArgs* args);
      static void ScaleLocal(ScriptableArgs* args);
      static void RotateLocalEuler(ScriptableArgs* args);
      static void RotateWorldX(ScriptableArgs* args);
      static void RotateWorldY(ScriptableArgs* args);
      static void RotateWorldZ(ScriptableArgs* args);
      static void RotateAroundWorld(ScriptableArgs* args);
      static void RotateLocalX(ScriptableArgs* args);
      static void RotateLocalY(ScriptableArgs* args);
      static void RotateLocalZ(ScriptableArgs* args);
      static void  RotateAroundLocal(ScriptableArgs* args);
      static void  TransformPoint(ScriptableArgs* args);
      static void  TransformVector(ScriptableArgs* args);
      static void TransformLocalPoint(ScriptableArgs* args);
      static void  TransformLocalVector(ScriptableArgs* args);
      static void  InverseTransformPoint(ScriptableArgs* args);
      static void  InverseTransformVector(ScriptableArgs* args);
      static void  Concatenate(ScriptableArgs* args);
      static void  Blend(ScriptableArgs* args);
      static void  LookAt(ScriptableArgs* args);
      static void LookAtLocal(ScriptableArgs* args);
    };
  }
 }
