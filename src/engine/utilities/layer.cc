#include "engine/utilities/layer.h"
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    LayerMask::LayerMask(const SortingLayer& layer) :
      layer_mask_(1ull << layer)
    {}

    //--------------------------------------------------------------------------------
    LayerMask LayerMask::FromLayers(std::initializer_list<SortingLayer> list)
    {
      size_t mask = 0;
      for (const SortingLayer& layer : list)
      {
        mask |= (1ull << layer);
      }
      return LayerMask(mask);
    }

    //--------------------------------------------------------------------------------
    LayerMask LayerMask::ExludeLayer(const SortingLayer& layer)
    {
      return LayerMask(~(1ull << layer));
    }

    //--------------------------------------------------------------------------------
    LayerMask LayerMask::ExcludeLayers(std::initializer_list<SortingLayer> exclude_list)
    {
      size_t mask = eastl::numeric_limits<size_t>::max();
      for (const SortingLayer& layer : exclude_list)
      {
        mask &= ~(1ull << layer);
      }
      return LayerMask(mask);
    }

    //--------------------------------------------------------------------------------
    void LayerMask::AddLayer(const SortingLayer& layer)
    {
      layer_mask_ |= (1ull << layer);
    }

    //--------------------------------------------------------------------------------
    void LayerMask::RemoveLayer(const SortingLayer& layer)
    {
      layer_mask_ &= ~(1ull << layer);
    }

    //--------------------------------------------------------------------------------
    bool LayerMask::ContainsLayer(const SortingLayer& layer)
    {
      const size_t mask = (1ull << layer);
      return (layer_mask_ & mask) == mask;
    }

    foundation::Vector<foundation::String> SortingLayer::layer_names_;
    
    //--------------------------------------------------------------------------------
    SortingLayer::SortingLayer() :
      layer_id_(0)
    {
      Initialize();
    }

    //--------------------------------------------------------------------------------
    SortingLayer::SortingLayer(size_t id) :
      layer_id_(id)
    {
      if (layer_id_ > 63)
      {
        PS_LOG(Warning, "The layer ID used exceeds the maximum layer ID (63), clamping to layer 63");
        layer_id_ = 63;
      }
    }

    //--------------------------------------------------------------------------------
    SortingLayer SortingLayer::LayerById(size_t id)
    {
      Initialize();
      return SortingLayer(id);
    }

    //--------------------------------------------------------------------------------
    SortingLayer SortingLayer::LayerByName(const foundation::String& name)
    {
      Initialize();

      for (size_t id = 0; id < layer_names_.size(); ++id)
      {
        if (layer_names_[id] == name)
        {
          return SortingLayer(id);
        }
      }

#ifdef _DEBUG
      PS_LOG(Warning, "Could not find a layer with this name! Returning the default layer...\n");
#endif

      return SortingLayer();
    }

    //--------------------------------------------------------------------------------
    SortingLayer SortingLayer::FindUnusedLayer(const foundation::String& name)
    {
      Initialize();

      for (size_t id = 0; id < layer_names_.size(); ++id)
      {
        if (layer_names_[id] == "")
        {
          SortingLayer layer(id);
          if (name != "")
          {
            layer.SetName(name);
          }
          return layer;
        }
      }

#ifdef _DEBUG
      PS_LOG(Warning, "Could not find an unused layer! Returning the default layer...\n");
#endif

      return SortingLayer();
    }

    //--------------------------------------------------------------------------------
    foundation::String SortingLayer::GetName() const
    {
      return layer_names_[layer_id_];
    }

    //--------------------------------------------------------------------------------
    void SortingLayer::SetName(const foundation::String& name)
    {
      if (layer_id_ == 0)
        return;

      layer_names_[layer_id_] = name;
    }

    //--------------------------------------------------------------------------------
    void SortingLayer::Initialize()
    {
      if (layer_names_.size() == 0)
      {
        layer_names_.resize(64);
        layer_names_[0] = "default_layer";
      }
    }
  }
}