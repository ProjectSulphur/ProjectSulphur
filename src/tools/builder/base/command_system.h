#pragma once
#include "tools/builder/base/commands_base.h"
#include "tools/builder/pipelines/scene_loader.h"
#include <foundation/utils/type_definitions.h>

namespace sulphur 
{
  namespace builder 
  {
    class ModelPipeline;
    class MaterialPipeline;
    class MeshPipeline;
    class ShaderPipeline;
    class TexturePipeline;
    class SkeletonPipeline;
    class AnimationPipeline;
    class ScriptPipeline;
    class AudioPipeline;

    /**
    * @brief error return codes for command system
    */
    enum struct CommandErr : uint16_t
    {
      kInvalidSyntax, //!< wrong syntax used. gets thrown when unique flag is used multiple times.
      kInvalidFlag, //!< wrong flag used. gets thrown if a given flag is not supported with a command.
      kMissingFlag,
      kUnknownCommand,//!< command not found or invalid command input
      kExitCommand, //!< exit command called aplication closes
      kNoError //!< everything okay
    };

    /**
    *@class sulphur::builder::CommandSystem
    *@brief class that manages command line input and registering of commands.  
    *@author Stan Pepels
    */
    class CommandSystem
    {
    public:
      /**
       * @brief Function pointer to a function that handles error messages.
       */
      using ErrorHandler = void(*)(const CommandErr& message);


      /**
      *@brief constructor
      *@param[in] model_pipeline (sulphur::builder::ModelPipeline*) a model pipeline
      *@param[in] mesh_pipeline (sulphur::builder::MeshPipeline*) a mesh pipeline
      *@param[in] material_pipeline (sulphur::builder::MaterialPipeline*) a material pipeline
      *@param[in] texture_pipeline (sulphur::builder::TexturePipeline*) a texture pipeline
      *@param[in] shader_pipeline (sulphur::builder::ShaderPipeline*) a shader pipeline
      *@param[in] skeleton_pipeline (sulphur::builder::SkeletonPipeline*) a skeleton pipeline
      *@param[in] animation_pipeline (sulphur::builder::AnimationPipeline*) an animation pipeline
      *@param[in] script_pipeline (sulphur::builder::ScriptPipeline*) a script pipeline
      *@param[in] audio_pipeline (sulphur::builder::AudioPipeline*) an audio pipeline
      *@param[in] scene_loader (sulphur::builder::SceneLoader*) a scene loader
      */
      CommandSystem(ModelPipeline* model_pipeline,
        MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
        TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline,
        SkeletonPipeline* skeleton_pipeline, AnimationPipeline* animation_pipeline,
        ScriptPipeline* script_pipeline, AudioPipeline* audio_pipeline, 
        SceneLoader* scene_loader);

      /**
      * @brief destructor
      */
      ~CommandSystem();

      /**
      *@brief set error handler for handling error codes.
      *@param[in] handler (sulphur::builder::ErrorHandler) the handler.
      */
      void set_error_handler(ErrorHandler handler);

      /**
      *@brief register command of type T with arguments Args...
      *@param[in] key (const char*) key to access the command with.
      *@param[in] args <Args...> parameter pack to pass in the constructor of command of type T.
      *@return (unsigned int) unique id used to unregister command. user is responsible for keeping it around.
      */
      template<typename T, typename... Args>
      uint RegisterCommand(const char* key, Args... args);

      /**
      *@brief unregister a command from the command list.
      *@param[in] command_id (unsigned int) command to unregister. value is the id returned when this command was registered  with sulphur::builder::CommandSystem::RegisterCommand.
      *@return (bool) true if removed false if command does not exist.
      *@see sulphur::builder::CommandSystem::RegisterCommand
      */
      bool UnregisterCommand(unsigned int command_id);

      /**
      *@brief execute the next line inputted in the console.
      */
      CommandErr ExecuteNext();

      /**
      *@brief execute single command line.
      */
      CommandErr ExecuteCommandLine(const char* command_line);

      /**
      *@brief print the descriptions of all registered commands.
      */
      void PrintDescriptions() const;
    private:
      ModelPipeline* model_pipeline_;         //!< a model pipeline passed by the constructor
      MeshPipeline* mesh_pipeline_;           //!< a mesh pipeline passed by the constructor
      MaterialPipeline* material_pipeline_;   //!< a material pipeline passed by the constructor
      TexturePipeline* texture_pipeline_;     //!< a texture pipeline passed by the constructor
      ShaderPipeline* shader_pipeline_;       //!< a shader pipeline passed by the constructor
      SkeletonPipeline* skeleton_pipeline_;   //!< a skeleton pipeline passed by the constructor
      AnimationPipeline* animation_pipeline_; //!< an animation pipeline passed by the constructor
      ScriptPipeline* script_pipeline_;       //!< a script pipeline passed by the constructor
      AudioPipeline* audio_pipeline_;         //!< an audio pipeline passed by the constructor
      SceneLoader* scene_loader_;             //!< a scene loader passed by the constructor

      ErrorHandler error_handler_;
      foundation::Vector<ICommand*> registered_commands_;
      foundation::Vector<unsigned int> command_ids_;

      unsigned int num_commands_;

      /**
      *@brief handle errors using either the default handler or the handler givin by sulphur::builder::CommandSystem::set_error_handler
      *@param[in] error (sulphur::builder::CommandErr) error to be processed
      *@see sulphur::builder::CommandSystem::set_error_handler
      */
      void HandleError(const CommandErr& error);

      /**
      *@brief validate syntax and parse command line to distinguish flags and parameters
      *@param[in] command_line (const char*) input to be parsed
      *@param[in] syntax (sulphur::builder::Syntax&) struct describing the syntax for the command
      *@param[out] cmd_input (sulphur::builder::CommandInput*) struct for querying the parsed flags and arguments. This gets passed to the command's sulphur::builder::Command::Run function
      *@see sulphur::builder::Command::Run
      */
      CommandErr ValidateSyntax(const char* command_line, Syntax& syntax, CommandInput* cmd_input);

      /**
      *@brief default error handler
      *@param[in] message (const sulphur::builder::CommandErr&) error message to be handled
      */
      static void DefaultErrorHandler(const CommandErr& message);

      const foundation::String help_command; //!< string for printing the description of all available commands
    };

    template<typename T, typename... Args>
    inline uint CommandSystem::RegisterCommand(const char* key, Args... args)
    {
      static_assert(std::is_base_of<ICommand, T>::value,
                    "command is not derived from Command class");

      registered_commands_.push_back(new T(key, args...));
      registered_commands_.back()->SetPipelines(model_pipeline_,
        mesh_pipeline_,
        material_pipeline_,
        texture_pipeline_,
        shader_pipeline_,
        skeleton_pipeline_,
        animation_pipeline_,
        script_pipeline_,
        audio_pipeline_,
        scene_loader_);

      command_ids_.push_back(num_commands_);
      ++num_commands_;
      return command_ids_.back();
    }
  }
}
