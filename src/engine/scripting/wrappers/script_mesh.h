#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/mesh.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptMesh : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);

      static foundation::SharedPointer<ScriptableValue> Copy(ScriptSystem* script_system, const Mesh& mesh);

      /**
      * @brief Get the name of the scriptable class
      * @returns (const char*) the name
      */
      static const char* Name()
      {
        return "Mesh";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return
        {
          { "New", New},
          { "CreatePoint" , CreatePoint },
          { "CreateTriangle" , CreateTriangle },
          { "CreateQuad" , CreateQuad },
          { "CreateCube" , CreateCube },
          { "CreatePlane" , CreatePlane },
          { "CreatePyramid" , CreatePyramid },
          { "CreateCone" , CreateCone },
          { "CreateCylinder" , CreateCylinder },
          { "CreateCircle" , CreateCircle },
          { "CreateHexPointy" , CreateHexPointy },
          { "CreateHexFlat" , CreateHexFlat },
          { "CreateSphere" , CreateSphere }
        };
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "CalculateBounds" , CalculateBounds },
          { "RecalculateNormals" , RecalculateNormals },
          { "RecalculateTangents" , RecalculateTangents },
          { "FlipNormals" , FlipNormals },
          { "AttachMesh" , AttachMesh },
          { "Validate" , Validate },
          { "Optimize" , Optimize },
          { "Clear" , Clear },
          { "Shade" , Shade },
          { "SetColor" , SetColor },
          { "MarkDynamic" , MarkDynamic },
          { "HasChanged" , HasChanged },
          { "ShouldUpdateIndexBuffer" , ShouldUpdateIndexBuffer },
          { "ShouldUpdatePositionBuffer" , ShouldUpdatePositionBuffer },
          { "ShouldUpdateColorBuffer" , ShouldUpdateColorBuffer },
          { "ShouldUpdateDataBuffer" , ShouldUpdateDataBuffer },
          { "IsStatic" , IsStatic },
          { "IsEmpty" , IsEmpty },
          { "IsAlwaysOnTop" , IsAlwaysOnTop },
          { "SetAlwaysOnTop" , SetAlwaysOnTop },
          { "GetVertexCount" , GetVertexCount },
          { "GetIndexCount" , GetIndexCount }
        };
      }

      static void CalculateBounds(ScriptableArgs* args);
      static void RecalculateNormals(ScriptableArgs* args);
      static void RecalculateTangents(ScriptableArgs* args);
      static void FlipNormals(ScriptableArgs* args);
      static void AttachMesh(ScriptableArgs* args);
      static void Validate(ScriptableArgs* args);
      static void Optimize(ScriptableArgs* args);
      static void Clear(ScriptableArgs* args);
      static void Shade(ScriptableArgs* args);
      static void SetColor(ScriptableArgs* args);
      static void MarkDynamic(ScriptableArgs* args);
      static void HasChanged(ScriptableArgs* args);
      static void ShouldUpdateIndexBuffer(ScriptableArgs* args);
      static void ShouldUpdatePositionBuffer(ScriptableArgs* args);
      static void ShouldUpdateColorBuffer(ScriptableArgs* args);
      static void ShouldUpdateDataBuffer(ScriptableArgs* args);
      static void IsStatic(ScriptableArgs* args);
      static void IsEmpty(ScriptableArgs* args);
      static void IsAlwaysOnTop(ScriptableArgs* args);
      static void SetAlwaysOnTop(ScriptableArgs* args);
      static void GetVertexCount(ScriptableArgs* args);
      static void GetIndexCount(ScriptableArgs* args);

      static void CreatePoint(ScriptableArgs* args);
      static void CreateTriangle(ScriptableArgs* args);
      static void CreateQuad(ScriptableArgs* args);
      static void CreateCube(ScriptableArgs* args);
      static void CreatePlane(ScriptableArgs* args);
      static void CreatePyramid(ScriptableArgs* args);
      static void CreateCone(ScriptableArgs* args);
      static void CreateCylinder(ScriptableArgs* args);
      static void CreateCircle(ScriptableArgs* args);
      static void CreateHexPointy(ScriptableArgs* args);
      static void CreateHexFlat(ScriptableArgs* args);
      static void CreateSphere(ScriptableArgs* args);
    };
  }
}