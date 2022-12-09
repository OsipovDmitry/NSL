#ifndef COLOR_H
#define COLOR_H

#include <array>
#include <QColor>

namespace model
{

struct Color : public std::array<uint8_t, 3>
{
    Color() : std::array<uint8_t, 3>({0, 0, 0}) {}
    Color(const std::array<uint8_t, 3>& another) : std::array<uint8_t, 3>(another) {}

    operator QColor() const  { return QColor(at(0), at(1), at(2)); }

    static QColor toQColor(const Color& color) { return QColor(color[0], color[1], color[2]); }
    static Color fromQColor(const QColor& color) { return Color({color.red(), color.green(), color.blue()}); }
};

}

#endif // COLOR_H
