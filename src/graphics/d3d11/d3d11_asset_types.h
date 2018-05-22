#pragma once
#include <glm/glm.hpp>
#include <wrl.h>
#include <foundation/utils/color.h>

struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;

struct ID3D11VertexShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;

namespace sulphur
{
  namespace graphics
  {
    struct D3D11Vertex
    {
      glm::vec3 pos;
      glm::vec3 normal;
      glm::vec3 tangent;
      glm::vec2 tex;
      foundation::Color color;
    };

    struct D3D11Asset {};

    struct D3D11Mesh : public D3D11Asset
    {
      uint index_count;
      Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
      Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    };

    struct D3D11Texture : public D3D11Asset
    {
      Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource_view;

      Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
      Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
    };

    template<typename T>
    struct D3D11ShaderProgram : public D3D11Asset
    {
      Microsoft::WRL::ComPtr<T> shader_program;
    };

    struct D3D11Shader : public D3D11Asset
    {
      D3D11ShaderProgram<ID3D11VertexShader>* vertex_shader;
      D3D11ShaderProgram<ID3D11HullShader>* hull_shader;
      D3D11ShaderProgram<ID3D11DomainShader>* domain_shader;
      D3D11ShaderProgram<ID3D11GeometryShader>* geometry_shader;
      D3D11ShaderProgram<ID3D11PixelShader>* pixel_shader;
    };
  }
}