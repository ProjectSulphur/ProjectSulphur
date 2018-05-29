#include "foundation/utils/color.h"

#include <sstream>

namespace sulphur
{
  namespace foundation
  {
    Color const Color::kRed = Color::FromByte(221u, 0u, 0u);
    Color const Color::kGreen = Color::FromByte(136u, 255u, 0u);
    Color const Color::kLime = Color::FromByte(123u, 203u, 79u);
    Color const Color::kBlue = Color::FromByte(34u, 34u, 255u);
    Color const Color::kWhite = Color::FromByte(255u, 255u, 255u);
    Color const Color::kHalfDutchWhite = Color::FromByte(255u, 244u, 214u);
    Color const Color::kBlack = Color::FromByte(0u, 0u, 0u);
    Color const Color::kYellow = Color::FromByte(255u, 238u, 0u);
    Color const Color::kOrange = Color::FromByte(255u, 165u, 0u);
    Color const Color::kPurple = Color::FromByte(160u, 32u, 240u);
    Color const Color::kCyan = Color::FromByte(0u, 255u, 255u);
    Color const Color::kMagenta = Color::FromByte(255u, 0u, 255u);
    Color const Color::kBrown = Color::FromByte(123u, 74u, 18u);
    Color const Color::kGrey = Color::FromByte(128u, 128u, 128u);
    Color const Color::kGray = Color::kGrey;
    Color const Color::kCornFlower = Color::FromByte(100u, 149u, 237u);

    Color const Color::kWhiteTransparent = Color::FromByte(255u, 255u, 255u, 0u);
    Color const Color::kBlackTransparent = Color::FromByte(0u, 0u, 0u, 0u);
    Color const Color::kGreenTransparent = Color::FromByte(0u, 255u, 0u, 50u);
    Color const Color::kRedTransparent = Color::FromByte(255u, 0u, 0u, 50u);
    Color const Color::kBlueTransparent = Color::FromByte(0u, 0u, 255u, 50u);

    //--------------------------------------------------------------------------
    Color::Color(foundation::String hex_string)
    {
      if (hex_string.length() == 6)
        hex_string.append("ff");
      
      std::stringstream ss;
      unsigned int temp;
      ss << std::hex << hex_string.c_str();
      ss >> temp;
      *this = Color(temp);
    }

    //--------------------------------------------------------------------------
    foundation::String Color::ToString() const
    {
      return "[r: " + foundation::to_string(r) + 
        ", g: " + foundation::to_string(g) + 
        ", b: " + foundation::to_string(b) + 
        ", a: " + foundation::to_string(a) + "]";
    }

    //--------------------------------------------------------------------------
    Color Color::Transparent(float opacity) const
    {
      return Color(r, g, b, byte(opacity * 255.0f));
    }
  }
}