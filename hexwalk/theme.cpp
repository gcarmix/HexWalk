/*
 * Copyright (C) 2026 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#include "theme.h"

#include <QImage>
#include <QPixmap>

namespace Theme {

static QPalette darkPalette()
{
    QPalette p;
    p.setColor(QPalette::Window, QColor(53, 53, 53));
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    p.setColor(QPalette::Base, QColor(42, 42, 42));
    p.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    p.setColor(QPalette::ToolTipBase, Qt::white);
    p.setColor(QPalette::ToolTipText, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    p.setColor(QPalette::Dark, QColor(35, 35, 35));
    p.setColor(QPalette::Shadow, QColor(20, 20, 20));
    p.setColor(QPalette::Button, QColor(53, 53, 53));
    p.setColor(QPalette::ButtonText, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    p.setColor(QPalette::BrightText, Qt::red);
    p.setColor(QPalette::Link, QColor(42, 130, 218));
    p.setColor(QPalette::Highlight, QColor(42, 130, 218));
    p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    return p;
}

static QPalette lightPalette()
{
    QPalette p;
    p.setColor(QPalette::Window, QColor(240, 240, 240));
    p.setColor(QPalette::WindowText, Qt::black);
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
    p.setColor(QPalette::Base, QColor(255, 255, 255));
    p.setColor(QPalette::AlternateBase, QColor(233, 233, 233));
    p.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    p.setColor(QPalette::ToolTipText, Qt::black);
    p.setColor(QPalette::Text, Qt::black);
    p.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
    p.setColor(QPalette::Dark, QColor(160, 160, 160));
    p.setColor(QPalette::Shadow, QColor(105, 105, 105));
    p.setColor(QPalette::Button, QColor(240, 240, 240));
    p.setColor(QPalette::ButtonText, Qt::black);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
    p.setColor(QPalette::BrightText, Qt::red);
    p.setColor(QPalette::Link, QColor(0, 122, 204));
    p.setColor(QPalette::Highlight, QColor(0, 120, 215));
    p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(190, 190, 190));
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(120, 120, 120));
    return p;
}

QPalette paletteFor(const QString& name)
{
    if (name.compare(kLight, Qt::CaseInsensitive) == 0)
        return lightPalette();
    return darkPalette();
}

HexColors hexDefaultsFor(const QString& name)
{
    HexColors c;
    if (name.compare(kLight, Qt::CaseInsensitive) == 0) {
        c.highlighting = QColor("#ffe0b2");
        c.addressArea  = QColor("#d0d0d0");
        c.selection    = QColor("#90caf9");
        c.addressFont  = QColor("#202020");
        c.asciiArea    = QColor("#ececec");
        c.asciiFont    = QColor("#1b5e20");
        c.hexFont      = QColor("#1b5e20");
    } else {
        c.highlighting = QColor("#540c00");
        c.addressArea  = QColor("#545454");
        c.selection    = QColor("#0998c7");
        c.addressFont  = QColor("#f0f0f0");
        c.asciiArea    = QColor("#424242");
        c.asciiFont    = QColor("#00ff5e");
        c.hexFont      = QColor("#00ff5e");
    }
    return c;
}

// Tint for monochrome glyphs on the light theme: dark when enabled, and still
// clearly dark (just a touch lighter) when disabled. The disabled colour must
// be well below ~150: the style would otherwise composite the normal glyph at
// ~50% over the light window and land on that washed-out grey, which is what
// made file-gated icons (entropy, binary, diff, ...) look unchanged.
static const QColor kLightGlyphColor(60, 60, 60);
static const QColor kLightGlyphDisabledColor(95, 95, 95);

// Recolour a glyph to a flat colour, preserving its alpha channel as the mask.
static QPixmap tintGlyph(const QImage& src, const QColor& color)
{
    QImage img = src; // copy; src is already ARGB32
    const int r = color.red();
    const int g = color.green();
    const int b = color.blue();
    for (int y = 0; y < img.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x)
            line[x] = qRgba(r, g, b, qAlpha(line[x]));
    }
    return QPixmap::fromImage(img);
}

QIcon themedIcon(const QString& path, const QString& name, bool monochrome)
{
    QPixmap pm(path);
    // Dark theme keeps the original light-grey glyphs; colourful icons
    // (open/save) are never recoloured. Only monochrome glyphs on the light
    // theme need work.
    if (pm.isNull() || !monochrome || name.compare(kLight, Qt::CaseInsensitive) != 0)
        return QIcon(pm);

    QImage img = pm.toImage().convertToFormat(QImage::Format_ARGB32);
    QIcon icon;
    // Provide explicit Normal and Disabled pixmaps so we control the disabled
    // look instead of letting the style fade the dark glyph to near-white.
    icon.addPixmap(tintGlyph(img, kLightGlyphColor), QIcon::Normal);
    icon.addPixmap(tintGlyph(img, kLightGlyphDisabledColor), QIcon::Disabled);
    return icon;
}

} // namespace Theme
