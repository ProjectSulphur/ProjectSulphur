#pragma once

#include <foundation/utils/type_definitions.h>
#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    class SortingLayer;

    /**
     * @class sulphur::engine::LayerMask
     * @brief Mask for sorting layers.
     * @author Timo van Hees
     */
    class LayerMask
    {
    public:
      /**
       * @brief Default constructor. 
       * @remark No layers masked. 
       */
      LayerMask() = default;
      /**
       * @brief Constructor.
       * @param[in] layer (const sulphur::engine::SortingLayer&) 
       * Layer to include in the mask.
       */
      LayerMask(const SortingLayer& layer);

      /**
       * @brief Creates a layer mask from a list of sorting layers.
       * @param[in] list (std::initializer_list <sulphur::engine::SortingLayer>) 
       * List of sorting layers to include in the mask. 
       * @return (sulphur::engine::LayerMask) The layer mask that has been created.
       */
      static LayerMask FromLayers(std::initializer_list<SortingLayer> list);
      /**
      * @brief Creates a layer mask that excludes a single sorting layer.
      * @param[in] layer (const sulphur::engine::SortingLayer&) The layer to exclude.
      * @return (sulphur::engine::LayerMask) The layer mask that has been created.
      */
      static LayerMask ExludeLayer(const SortingLayer& layer);/**
       * @brief Creates a layer mask that excludes a list of sorting layers.
       * @param[in] exclude_list (std::initializer_list <sulphur::engine::SortingLayer>) 
       * List of sorting layers to exclude in the mask. 
       * @return (sulphur::engine::LayerMask) The layer mask that has been created.
       */
      static LayerMask ExcludeLayers(std::initializer_list<SortingLayer> exclude_list);

      /**
       * @brief Include a layer in the mask.
       * @param[in] layer (const sulphur::engine::SortingLayer&) layer to include. 
       */
      void AddLayer(const SortingLayer& layer);
      /**
      * @brief Exclude a layer in the mask.
      * @param[in] layer (const sulphur::engine::SortingLayer&) layer to exclude.
      */
      void RemoveLayer(const SortingLayer& layer);
      /**
       * @brief Checks if a sorting layer is included in the mask.
       * @param[in] layer (const sulphur::engine::SortingLayer&) layer to check.
       * @return If the mask includes the sorting layer.
       */
      bool ContainsLayer(const SortingLayer& layer);

      /**
       * @brief Equals operator.
       */
      bool operator==(const LayerMask& rhs) const
      {
        return layer_mask_ == rhs.layer_mask_;
      };

      /**
      * @brief Bitwise and assignment operator.
      */
      LayerMask& operator&=(const LayerMask& rhs)
      {
        layer_mask_ &= rhs;
        return *this;
      };

      /**
      * @brief Bitwise XOR assignment operator.
      */
      LayerMask& operator^=(const LayerMask& rhs)
      {
        layer_mask_ ^= rhs;
        return *this;
      };

      /**
      * @brief Bitwise or assignment operator.
      */
      LayerMask& operator|=(const LayerMask& rhs)
      {
        layer_mask_ |= rhs;
        return *this;
      };

      /**
      * @brief Bitwise and operator.
      */
      LayerMask operator&(const LayerMask& rhs) const
      {
        return LayerMask(layer_mask_ & rhs);
      };


      /**
      * @brief Bitwise negation operator.
      */
      LayerMask operator~() const
      {
        return LayerMask(~layer_mask_);
      };

      /**
      * @brief Bitwise XOR operator.
      */
      LayerMask operator^(const LayerMask& rhs) const
      {
        return LayerMask(layer_mask_ ^ rhs);
      };

      /**
      * @brief Bitwise or operator.
      */
      LayerMask operator|(const LayerMask& rhs) const
      {
        return LayerMask(layer_mask_ | rhs);
      };

      /**
      * @brief Implicit conversion to size_t.
      */
      operator size_t() const { return layer_mask_; };

    private:
      LayerMask(size_t mask) :
        layer_mask_(mask)
      {};

      size_t layer_mask_ = PS_SIZE_MAX; //!< The layer mask.
    };

    /**
     * @class sulphur::engine::SortingLayer
     * @brief Layer used in culling operations.
     * @author Timo van Hees
     */
    class SortingLayer
    {
    public:
      /**
      * @brief Default constructor.
      * @remark Creates default layer.
      */
      SortingLayer();

      static const uint kLayerCount = 64u;  //!< Maximum number of layers.

      /**
       * @brief Finds a layer by id.
       * @param[in] id (size_t) The id of the layer to find.
       * @return (sulphur::engine::SortingLayer) The sorting layer of id.
       */
      static SortingLayer LayerById(size_t id);
      /**
      * @brief Finds a layer by name.
      * @param[in] name (const sulphur::foundation::String&) The name of the layer to find.
      * @return (sulphur::engine::SortingLayer) The sorting layer of name.
      * @remark If no layer with this name can be found, returns the default layers.
      */
      static SortingLayer LayerByName(const foundation::String& name);
      /**
      * @brief Finds a layer that's not being used.
      * @param[in] name (const sulphur::foundation::String&) Name to assign to the layer if found.
      * @return (sulphur::engine::SortingLayer) An unused sorting layer.
      * @remark If no layer with this name can be found, returns the default layers.
      */
      static SortingLayer FindUnusedLayer(const foundation::String& name = "");

      /**
       * @brief Implicit conversion operator to size_t.
       */
      operator size_t() const { return layer_id_; };

      /**
       * @brief Get the name of this layer.
       * @return (sulphur::foundation::String) The name of this layer.
       */
      foundation::String GetName() const;
      /**
       * @brief Change the name of this layer.
       * @param[in] name (const sulphur::foundation::String&) The new name.
       * @remark Also changes the name of all other sorting layers with the same id.
       */
      void SetName(const foundation::String& name);

      /**
       * @brief Creates storage for sorting layers and creates default layer.
       */
      static void Initialize();

    private:
      /**
       * @brief Constructor.
       * @param[in] id (size_t) The id of the sorting layer.
       */
      SortingLayer(size_t id);

      size_t layer_id_; //!< The id of this layer.

      static foundation::Vector<foundation::String> layer_names_; //!< The names of all layers.
    };
  }
}