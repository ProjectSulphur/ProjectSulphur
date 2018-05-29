local Game = 
{
  CamVel = {x = 0, y = 0, z = 0},
  CamAcc = 10,
  CamResist = 0.85,

  Init = function(self, towerSize)

    self.assets = 
    {
      cube = AssetSystem.Load(AssetTypes.kModel, "GrassTile"),
    }

    self.world = World.Create()
    self.mainCamera = self.world:CreateEntity()
    self.mainCamera:AddComponent(Types.CameraComponent)
    self.mainCamera:GetComponent(Types.TransformComponent):SetWorldPosition({0,2,-10})

    self.floor = self.world:CreateEntity()
    self.floor:AddComponent(Types.TransformComponent):SetLocalPosition({0, 0, 0}):SetLocalScale({64,1,64})
    self.floor:AddComponent(Types.MeshRenderComponent):SetModel(self.assets.cube)
    self.floor:AddComponent(Types.BoxColliderComponent):SetExtents({32,0.5,32})

    self.tower = {}
    
    for i = 1, towerSize, 1 do
      for j = 1, towerSize, 1 do
        for k = 1, towerSize, 1 do
          local ent =  self.world:CreateEntity()
          table.insert(self.tower, ent)
          ent:AddComponent(Types.TransformComponent):SetLocalPosition({i,j + towerSize,k}):SetLocalScale({1.1,1.1,1.1})
          ent:AddComponent(Types.MeshRenderComponent):SetModel(self.assets.cube)
          ent:AddComponent(Types.BoxColliderComponent)
          ent:AddComponent(Types.RigidBodyComponent)
        end
      end
    end

  end,

  UpdateCamera = function(self, dt)
    local acc = self.CamAcc * dt

    local right = 0
    if (Input.IsButtonDown(Buttons.kD) == true) then
      right = acc
    elseif (Input.IsButtonDown(Buttons.kA) == true) then
      right = -acc
    end
    
    local forward = 0
    if (Input.IsButtonDown(Buttons.kW) == true) then
      forward = -acc
    elseif (Input.IsButtonDown(Buttons.kS) == true) then
      forward = acc
    end

    local up = 0
    if (Input.IsButtonDown(Buttons.kSpace) == true) then
      up = acc
    elseif (Input.IsButtonDown(Buttons.kLeftControl) == true) then
      up = -acc
    end

    local camUp = self.mainCamera:GetComponent(Types.TransformComponent):GetWorldUp()
    local camForward = self.mainCamera:GetComponent(Types.TransformComponent):GetWorldForward()
    local camRight = self.mainCamera:GetComponent(Types.TransformComponent):GetWorldRight()
    local camPos = self.mainCamera:GetComponent(Types.TransformComponent):GetWorldPosition()
    -- Debug.Log("X: " .. camPos.x .. " Y: " .. camPos.y .. " Z: " .. camPos.z .. "\n")

    local newCamPos = camPos;

    -- Apply forward
    newCamPos.x = newCamPos.x + (camForward.x * forward)
    newCamPos.y = newCamPos.y + (camForward.y * forward)
    newCamPos.z = newCamPos.z + (camForward.z * forward)

    -- Apply right
    newCamPos.x = newCamPos.x + (camRight.x * right)
    newCamPos.y = newCamPos.y + (camRight.y * right)
    newCamPos.z = newCamPos.z + (camRight.z * right)

    -- Apply Up
    newCamPos.x = newCamPos.x + (camUp.x * up)
    newCamPos.y = newCamPos.y + (camUp.y * up)
    newCamPos.z = newCamPos.z + (camUp.z * up)

    self.mainCamera:GetComponent(Types.TransformComponent):SetWorldPosition(newCamPos)
  end,

  Update = function(self, dt)
    self.UpdateCamera(self, dt)
  end
}

function OnInitialize()
  Game:Init(3)
end

function Update(dt)
  Game:Update(dt)
end

function FixedUpdate()
end