#pragma once
#include "d3d11_asset_types.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/string.h>

#include <dxgiformat.h>
#include <wrl.h>
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct IDXGIAdapter;
struct ID3D11RenderTargetView;
struct ID3D11Resource;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11InputLayout;
struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;

enum DXGI_MODE_SCANLINE_ORDER : int;
enum DXGI_MODE_SCALING : int;

enum D3D11_BIND_FLAG : int;
enum D3D11_USAGE : int;
enum D3D11_CPU_ACCESS_FLAG : int;

namespace sulphur
{
  namespace graphics
  {
    enum struct TopologyType;
  }

  namespace engine
  {
    enum struct TextureFormat;
  }
}

namespace sulphur
{
  namespace graphics
  {
    struct DeviceInfo
    {
      size_t dedicatedVRAM, usedVRAM;
      std::wstring gpuDescription;
    };

    struct Rational
    {
      uint Numerator;
      uint Denominator;
    };

    struct DisplayMode
    {
      uint Width;
      uint Height;
      Rational RefreshRate;
      DXGI_FORMAT Format;
      DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
      DXGI_MODE_SCALING Scaling;
    };

    class D3D11Device
    {
    public:
      void Create();

      void CreateSwapChain(void* hWnd, const glm::ivec2& screen_size, bool vsync,
        uint refresh_rate);

      void CreateBackBuffer();
      Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetBackBuffer();

      void CreateTexture(uint width, uint height, engine::TextureFormat format, const byte* data,
        Microsoft::WRL::ComPtr<ID3D11Texture2D>& out_tex,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& out_view,
        bool allow_dsv, bool allow_rtv);
      void CreateDepthBufferFromTexture(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& out_dsv);
      void CreateRenderTargetFromTexture(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& out_rtv);

      void ClearDepthBuffer(const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& dsv);
      void ClearBackBuffer(const foundation::Color& clear_color);
      void ClearRenderTarget(const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
        const foundation::Color& clear_color);

      void CreateObjectBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& out_buffer,
        size_t source_size);
      void UpdateObjectBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
        const void* source);

      void SetViewport(float x, float y, float width, float height);
      void SetPrimitiveTopology(TopologyType topology_type);

      void CreateDepthStencilState();
      void CreateRasterizerState();
      void CreateBlendState();

      void CreateSamplers();

      void CreateDefaultShaders();

      bool CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& out_buffer, const void* data,
        size_t size, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage,
        D3D11_CPU_ACCESS_FLAG cpuFlags, uint miscFlags);

      void CopyResource(const Microsoft::WRL::ComPtr<ID3D11Resource>& destination,
        const Microsoft::WRL::ComPtr<ID3D11Resource>& source);

      void CreateVertexShader(const void* src_data, size_t src_size,
        Microsoft::WRL::ComPtr<ID3D11VertexShader>& out_shader);
      void CreateHullShader(const void* src_data, size_t src_size,
        Microsoft::WRL::ComPtr<ID3D11HullShader>& out_shader);
      void CreateDomainShader(const void* src_data, size_t src_size,
        Microsoft::WRL::ComPtr<ID3D11DomainShader>& out_shader);
      void CreateGeometryShader(const void* src_data, size_t src_size,
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>& out_shader);
      void CreatePixelShader(const void* src_data, size_t src_size,
        Microsoft::WRL::ComPtr<ID3D11PixelShader>& out_shader);

      void SetRenderTargets(uint num_render_targets, ID3D11RenderTargetView* const* render_targets,
        const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& dsv);
      void SetTexture(uint register_id, const D3D11Texture* texture);
      void SetMesh(const D3D11Mesh* mesh);
      void SetShader(const D3D11Shader* shader);
      void SetStencilRef(uint value);

      void Draw(uint index_count = 0, uint index_offset = 0);
      void Present(bool vsync);

      const DeviceInfo& device_info() const { return device_info_; };

      ID3D11Device* GetDevice();
      ID3D11DeviceContext* GetDeviceContext();

    private:
      DeviceInfo device_info_;
      foundation::Vector<DisplayMode> displayModes;

      Microsoft::WRL::ComPtr<ID3D11Device> device_;
      Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context_;

      Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
      Microsoft::WRL::ComPtr<IDXGIAdapter> adapter_;

      Microsoft::WRL::ComPtr<ID3D11RenderTargetView> back_buffer_;

      Microsoft::WRL::ComPtr<ID3D11RasterizerState> default_rasterizer_state_;
      Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_depth_stencil_state_;
      Microsoft::WRL::ComPtr<ID3D11BlendState> default_blend_state_;

      Microsoft::WRL::ComPtr<ID3D11SamplerState> point_sampler_;
      Microsoft::WRL::ComPtr<ID3D11SamplerState> linear_sampler_;

      Microsoft::WRL::ComPtr<ID3D11InputLayout> default_input_layout_;

      Microsoft::WRL::ComPtr<ID3D10Blob> default_vertex_shader_blob_;
      Microsoft::WRL::ComPtr<ID3D10Blob> default_pixel_shader_blob_;

      Microsoft::WRL::ComPtr<ID3D11VertexShader> default_vertex_shader_;
      Microsoft::WRL::ComPtr<ID3D11PixelShader> default_pixel_shader_;

      Microsoft::WRL::ComPtr<ID3D11Buffer> default_index_buffer_;
      Microsoft::WRL::ComPtr<ID3D11Buffer> default_vertex_buffer_;

      uint GetSizeFromFormat(DXGI_FORMAT format) const;
    };
  }
}