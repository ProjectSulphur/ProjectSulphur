SET (LUA_CLASSES
	"core/world.h"
	"core/entity_system.h"

	"systems/components/camera_system.h"
	"systems/components/transform_system.h"
	"systems/components/skinned_mesh_render_system.h"
	"systems/components/mesh_render_system.h"
	"systems/components/rigid_body_system.h"
	"systems/components/collider_system.h"
  "systems/components/network_component_system.h"
  "systems/components/constraint_system.h"
	"systems/components/audio_event_system.h"
	"systems/components/audio_listener_system.h"
	"systems/components/audio_snapshot_system.h"
	"systems/components/audio_vca_system.h"
  "systems/components/audio_bus_system.h"
	"input/mappings.h"
	"input/input.h"
	"networking/network_system.h"
	"utilities/scriptable_imgui.h"
	"core/script_debug.h"
	"assets/scriptable_asset_system.h"
	"audio/audio_system.h"
	"physics/physics_system.h"
CACHE STRING "The list of classes that should be exposed to Lua" FORCE)
