#pragma once
#include "foundation/utils/type_definitions.h"
#include "foundation/containers/vector.h"
#include "foundation/containers/string.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL // glm::lerp(vec3, vec3, t)
#include <glm/gtx/compatibility.hpp>

namespace sulphur
{
  namespace foundation
  {

#define PS_RGB_TO_FLOAT 0.0039215686274509803921568627451
#define PS_FLOAT_TO_RGB 255.0

    struct Color
    {
      union
      {
        float rgba[4];

        struct
        {
          // Red, Green, Blue, Alpha
          float r, g, b, a;
        };

        // All color values as a vector
        glm::vec4 v;
      };

      //--------------------------------------------------------------------------
      Color()
        : v()
      {}

      //--------------------------------------------------------------------------
      Color(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha)
      {}

      //--------------------------------------------------------------------------
      Color(glm::vec3 color, float alpha = 1.0f)
        : r(color.x), g(color.y), b(color.z), a(alpha)
      {}

      //--------------------------------------------------------------------------
      Color(glm::vec4 color)
        : v(color)
      {}

      //--------------------------------------------------------------------------
      Color(const Color& other):
        v(other.v)
      {}

      //--------------------------------------------------------------------------
      operator int() const
      {
        return (int(PS_FLOAT_TO_RGB * r) << 24 |
          int(PS_FLOAT_TO_RGB * g) << 16 |
          int(PS_FLOAT_TO_RGB * b) << 8 |
          int(PS_FLOAT_TO_RGB * a));
      };

      //--------------------------------------------------------------------------
      operator glm::vec4() const { return glm::vec4(r, g, b, a); };

      //--------------------------------------------------------------------------
      // Formated as RGBA
      Color(int color) :
        r(float(PS_RGB_TO_FLOAT * ((color & 0xff000000) >> 24))),
        g(float(PS_RGB_TO_FLOAT * ((color & 0xff0000) >> 16))),
        b(float(PS_RGB_TO_FLOAT * ((color & 0xff00) >> 8))),
        a(float(PS_RGB_TO_FLOAT * (color & 0xff)))
      {}

      //--------------------------------------------------------------------------
      Color(foundation::String hex);

      //--------------------------------------------------------------------------
      static inline Color FromByte(byte red = 0u, byte green = 0u, byte blue = 0u, byte alpha = 255u)
      {
        return Color(float(PS_RGB_TO_FLOAT * red), float(PS_RGB_TO_FLOAT * green), float(PS_RGB_TO_FLOAT * blue), float(PS_RGB_TO_FLOAT * alpha));
      }

      //--------------------------------------------------------------------------
      static inline Color FromHSV(float h, float s, float v)
      {
        Color color;
        color.a = 1.0f;

        if (s == 0.0f)
        {
          color.r = v;
          color.g = v;
          color.b = v;
        }
        else
        {
          int i;
          float f, p, q, t;

          if (h == 360.0f)
          {
            h = 0.0f;
          }
          else
          {
            h /= 60.0f;
          }

          i = (int)trunc(h);
          f = h - i;

          p = v * (1.0f - s);
          q = v * (1.0f - (s * f));
          t = v * (1.0f - (s * (1.0f - f)));

          switch (i)
          {
          case 0:
            color.r = v;
            color.g = t;
            color.b = p;
            break;

          case 1:
            color.r = q;
            color.g = v;
            color.b = p;
            break;

          case 2:
            color.r = p;
            color.g = v;
            color.b = t;
            break;

          case 3:
            color.r = p;
            color.g = q;
            color.b = v;
            break;

          case 4:
            color.r = t;
            color.g = p;
            color.b = v;
            break;

          default:
            color.r = v;
            color.g = p;
            color.b = q;
            break;
          }

        }

        return color;
      }

      //--------------------------------------------------------------------------
      static inline Color FromARGB(ulong argb)
      {
        return Color(float(PS_RGB_TO_FLOAT * ((argb & 0xff0000) >> 16))
          , float(PS_RGB_TO_FLOAT * ((argb & 0xff00) >> 8))
          , float(PS_RGB_TO_FLOAT * (argb & 0xff))
          , float(PS_RGB_TO_FLOAT * ((argb & 0xff000000) >> 24)));
      }

      //--------------------------------------------------------------------------
      glm::u8vec4 ToByteColor() const
      {
        return glm::u8vec4(static_cast<byte>(PS_FLOAT_TO_RGB * r),
          static_cast<byte>(PS_FLOAT_TO_RGB * g),
          static_cast<byte>(PS_FLOAT_TO_RGB * b),
          static_cast<byte>(PS_FLOAT_TO_RGB * a));
      }

      //--------------------------------------------------------------------------
      foundation::Vector<byte> AsBytes() const
      {
        return foundation::Vector<byte>({ static_cast<byte>(PS_FLOAT_TO_RGB * r),
          static_cast<byte>(PS_FLOAT_TO_RGB * g),
          static_cast<byte>(PS_FLOAT_TO_RGB * b),
          static_cast<byte>(PS_FLOAT_TO_RGB * a) });
      }

      //--------------------------------------------------------------------------
      bool operator==(const int& other) const
      {
        return (*this == Color(other));
      };
      //--------------------------------------------------------------------------
      void operator=(const Color& other)
      {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
      };

      //--------------------------------------------------------------------------
      bool operator!=(const int& other) const
      {
        return !operator==(other);
      };

      //--------------------------------------------------------------------------
      bool operator==(const Color& other) const
      {
        return (v == other.v);
      };

      //--------------------------------------------------------------------------
      bool operator!=(const Color& other) const
      {
        return !operator==(other);
      };

      //--------------------------------------------------------------------------
      Color& operator*= (const Color& other)
      {
        v *= other.v;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator* (const Color& other) const
      {
        return Color(v * other.v);
      };

      //--------------------------------------------------------------------------
      Color& operator/= (const Color& other)
      {
        v /= other.v;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator/ (const Color& other) const
      {
        return Color(v / other.v);
      };

      //--------------------------------------------------------------------------
      Color& operator+= (const Color& other)
      {
        v += other.v;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator+ (const Color& other) const
      {
        return Color(v + other.v);
      };

      //--------------------------------------------------------------------------
      Color& operator-= (const Color& other)
      {
        v -= other.v;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator- (const Color& other) const
      {
        return Color(v - other.v);
      };

      //--------------------------------------------------------------------------
      Color& operator*= (const float f)
      {
        v *= f;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator* (const float f) const
      {
        return Color(v*f);
      };

      //--------------------------------------------------------------------------
      Color& operator/= (const float f)
      {
        v /= f;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator/ (const float f) const
      {
        return Color(v / f);
      };

      //--------------------------------------------------------------------------
      Color& operator+= (const float f)
      {
        v += f;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator+ (const float f) const
      {
        return Color(v + f);
      };

      //--------------------------------------------------------------------------
      Color& operator-= (const float f)
      {
        v -= f;
        return *this;
      };

      //--------------------------------------------------------------------------
      Color operator- (const float f) const
      {
        return Color(v - f);
      };

      //--------------------------------------------------------------------------
      /* Returns a linearly interpolated color between this color and 'other' based on t. */
      Color lerp(const Color& other, const float t) const
      {
        return Color(glm::lerp(v, other.v, t));
      };

      //--------------------------------------------------------------------------
      /* Returns a linearly interpolated color between color 'a' and 'b' based on t. */
      static Color lerp(const Color& a, const Color& b, const float t)
      {
        return Color(a.lerp(b.v, t));
      };

      //--------------------------------------------------------------------------
      /* Returns a structured string representation of this color. */
      foundation::String ToString() const;

      //--------------------------------------------------------------------------
      friend Color operator* (const float d, const Color& color)
      {
        return Color(d * color.v);
      };

      //--------------------------------------------------------------------------
      friend Color operator/ (const float d, const Color& color)
      {
        return Color(d / color.v);
      };

      //--------------------------------------------------------------------------
      friend Color operator+ (const float d, const Color& color)
      {
        return Color(d + color.v);
      };

      //--------------------------------------------------------------------------
      friend Color operator- (const float d, const Color& color)
      {
        return Color(d - color.v);
      };

      // Define some quick colors
      static const Color kRed;
      static const Color kGreen;
      static const Color kLime;
      static const Color kBlue;
      static const Color kWhite;
      static const Color kHalfDutchWhite;
      static const Color kBlack;
      static const Color kYellow;
      static const Color kOrange;
      static const Color kPurple;
      static const Color kCyan;
      static const Color kMagenta;
      static const Color kBrown;
      static const Color kGrey;
      static const Color kGray;
      static const Color kCornFlower;

      static const Color kWhiteTransparent;
      static const Color kBlackTransparent;

      Color Transparent(float opacity = 0.0f) const;
    };
  }
}