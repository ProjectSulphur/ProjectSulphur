#include "engine/core/entity_system.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::FreeCamSystem : sulphur::engine::IOwnerSystem 
    *   <sulphur::engine::FreeCamSystem>
    * @brief Creates a basic controllable free cam as main-camera
    * @remarks Intended for use with the sulphur-editor only
    * @author Maarten ten Velden
    */
    class FreeCamSystem : public IOwnerSystem<FreeCamSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      FreeCamSystem();

      /**
      * @see sulphur::engine::IOwnerSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @see sulphur::engine::IOwnerSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @brief Setter for the movement sensitivity value
      * @details The movement sensitivity is the value with which input (range -1 to 1) is
      *   multiplied before applying movement.
      * @param[in] sensitivity (const glm::vec3&) The new value
      */
      void set_move_sensitivity(const glm::vec3& sensitivity);
      /**
      * @brief Setter for the look sensitivity value
      * @details The look sensitivity is the value with which input (range - 1 to 1) is
      *   multiplied before applying rotation(in euler angles)
      * @param[in] sensitivity (const glm::vec2&) The new value
      */
      void set_look_sensitivity(const glm::vec2& sensitivity);
      /**
      * @brief Setter for the mouse speed value
      * @details The mouse speed is the value with which the mouse offset (range -1 to 1) is
      *   multiplied before input accumulation
      * @param[in] speed (float) The new value
      */
      void set_mouse_speed(float speed);

      /**
      * @brief Getter for the movement sensitivity value
      * @details The movement sensitivity is the value with which input (range -1 to 1) is
      *   multiplied before applying movement.
      * @return (glm::vec3) The current value
      */
      glm::vec3 move_sensitivity();
      /**
      * @brief Getter for the look sensitivity value
      * @details The look sensitivity is the value with which input (range - 1 to 1) is
      *   multiplied before applying rotation(in euler angles)
      * @return (glm::vec2) The current value
      */
      glm::vec2 look_sensitivity();
      /**
      * @brief Getter for the mouse speed value
      * @details The mouse speed is the value with which the mouse offset (range -1 to 1) is
      *   multiplied before input accumulation
      * @return (float) The current value
      */
      float mouse_speed();

    private:
      /**
      * @brief Process input and move the free cam accordingly
      */
      void UpdateMovement();
      /**
      * @brief Process input and rotate the free cam accordingly
      */
      void UpdateLook();

      Input* input_; //!< The input system of the current application
      World* world_; //!< The world in which this free cam lives

      Entity entity_; //!< The free cam entity
      
      glm::vec3 move_sensitivity_; //!< The value with which input (range -1 to 1) is multiplied before applying movement
      glm::vec2 look_sensitivity_; //!< The value with which input (range -1 to 1) is multiplied before applying rotation (in euler angles)
      float mouse_speed_; //!< The value with which the mouse offset (range -1 to 1) is multiplied before input accumulation

    };
  }
}