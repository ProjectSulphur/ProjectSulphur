#pragma once
#pragma once

#include "physics/platform_physics_body.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

#include <glm/vec3.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::PhysicsManifold
    * @brief Class that contains all information for contacts, used in collision callbacks
    * @author Benjamin Waanders
    */
    class PhysicsManifold
    {
    public:

      /**
      * @struct sulphur::physics::PhysicsManifold::ContactPoint
      * @brief Contact point structure POD
      */
      struct ContactPoint
      {
        /**
        * @brief The constructor for the POD struct
        */
        ContactPoint(glm::vec3 point, glm::vec3 normal, float seperation)
          : point_(point), normal_(normal), seperation_(seperation)
        {}

        glm::vec3 point_; //!< Contact point from A onto B
        glm::vec3 normal_; //!< Contact normal from A onto B 
        float seperation_; //!< Seperation distance over the normal
      };

      /**
      * @Brief the default constructor
      */
      PhysicsManifold();
      
      /**
      * @Brief the default deconstructor
      */
      ~PhysicsManifold();

      /**
      * @brief Constructor that takes both bodies A and B
      * @param[in] body_a (sulphur::physics::PhysicsBody*)
      * @param[in] body_b (sulphur::physics::PhysicsBody*)
      */
      PhysicsManifold(
        PhysicsBody* body_a,
        PhysicsBody* body_b);

      /**
      * @brief Getter for the contact points
      * @return (sulphur::foundation::Vector<sulphur::physics::PhysicsManifold::ContactPoint>& the contact points
      */
      const foundation::Vector<ContactPoint>& get_contact_points() const;

      /**
      * @brief Adds a contact point to the manifold
      * @param[in] contact_point (sulphur::physics::PhysicsManifold::ContactPoint)
      */
      void AddContactPoint(const ContactPoint& contact_point);

      /**
      * @brief Getter for the B body
      * @return (sulphur::physics::PhysicsBody*) Body B
      */
      PhysicsBody* body_a();

      /**
      * @brief Getter for the A body
      * @return (sulphur::physics::PhysicsBody*) Body A
      */
      PhysicsBody* body_b();

    private:
      PhysicsBody* body_a_; //!< Body A
      PhysicsBody* body_b_; //!< Body B
      
      foundation::Vector<ContactPoint> contact_points_; //!< The collection of contact points
    };
  }
}
