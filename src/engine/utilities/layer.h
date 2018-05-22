#pragma once

#include <foundation/utils/type_definitions.h>
#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    class SortingLayer;

    class LayerMask
    {
    public:
      LayerMask() = default;
      LayerMask(const SortingLayer& layer);

      static LayerMask FromLayers(std::initializer_list<SortingLayer> list);

      void AddLayer(const SortingLayer& layer);
      void RemoveLayer(const SortingLayer& layer);
      bool ContainsLayer(const SortingLayer& layer);

      bool operator==(const LayerMask& rhs)
      {
        return layer_mask_ == rhs.layer_mask_;
      };

      LayerMask& operator&=(const LayerMask& rhs)
      {
        layer_mask_ &= rhs;
        return *this;
      };

      LayerMask& operator^=(const LayerMask& rhs)
      {
        layer_mask_ ^= rhs;
        return *this;
      };

      LayerMask& operator|=(const LayerMask& rhs)
      {
        layer_mask_ |= rhs;
        return *this;
      };

      LayerMask operator&(const LayerMask& rhs) const
      {
        return LayerMask(layer_mask_ & rhs);
      };

      LayerMask operator~() const
      {
        return LayerMask(~layer_mask_);
      };

      LayerMask operator^(const LayerMask& rhs) const
      {
        return LayerMask(layer_mask_ ^ rhs);
      };

      LayerMask operator|(const LayerMask& rhs) const
      {
        return LayerMask(layer_mask_ | rhs);
      };


      operator size_t() const { return layer_mask_; };

    private:
      LayerMask(size_t mask) :
        layer_mask_(mask)
      {};

      size_t layer_mask_ = PS_SIZE_MAX;
    };

    class SortingLayer
    {
    public:
      SortingLayer();

      static const uint kLayerCount = 64u;
      static SortingLayer LayerById(size_t id);
      static SortingLayer LayerByName(const foundation::String& name);

      operator size_t() const { return layer_id_; };

      foundation::String GetName() const;
      void SetName(const foundation::String& name);
      void initialize();

    private:
      SortingLayer(size_t id, foundation::String name);

      size_t layer_id_;

      static foundation::Vector<foundation::String> layer_names_;
    };
  }
}

template<> struct eastl::hash<sulphur::engine::SortingLayer>
{
public:
  size_t operator()(const sulphur::engine::SortingLayer& v) const
  {
    return eastl::hash<size_t>()(v);
  }
};

template<> struct eastl::hash<sulphur::engine::LayerMask>
{
public:
  size_t operator()(const sulphur::engine::LayerMask& v) const
  {
    return eastl::hash<size_t>()(v);
  }
};