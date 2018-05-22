SET(PhysicsAPISources "")
SET(PhysicsTarget "")

IF(PS_PHYSX)
	IMPORT_FOLDER(PhysXSources "physx")
	SET(PhysicsAPISources ${PhysicsAPISources} ${PhysXSources})
	SET(PhysicsTarget physx)
ELSEIF(PS_BULLET)
	IMPORT_FOLDER(BulletSources "bullet")
	SET(PhysicsAPISources ${PhysicsAPISources} ${BulletSources})
	SET(PhysicsTarget bullet3)
ELSEIF(PS_SULPHURPHYSICS)
	MESSAGE( SEND_ERROR "Sulphur (custom) physics currently not supported.")
ELSE()
	MESSAGE( SEND_ERROR "No/Unknown physics SDK requested.")
ENDIF()

IMPORT_FOLDER(PhysicsRootSources "")

SET(PhysicsSources 
	${PhysicsRootSources}
	${PhysicsAPISources}
)

ADD_LIBRARY(sulphur-physics STATIC ${PhysicsSources})
TARGET_LINK_LIBRARIES(sulphur-physics EASTL glm ${PhysicsTarget})