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
#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QIcon>
#include <QPalette>
#include <QString>

namespace Theme {

// Recognised theme names persisted in QSettings.
constexpr const char* kDark  = "dark";
constexpr const char* kLight = "light";

// QPalette for the whole application (window/button/text colours, etc.).
QPalette paletteFor(const QString& name);

// Default colours for the qhexedit widget area for a given theme.
// These are restored when the user switches theme or hits "Restore Defaults".
struct HexColors {
    QColor highlighting;
    QColor addressArea;
    QColor selection;
    QColor addressFont;
    QColor asciiArea;
    QColor asciiFont;
    QColor hexFont;
};
HexColors hexDefaultsFor(const QString& name);

// Returns a theme-appropriate version of a toolbar/menu icon.
// The shipped icons are light-grey monochrome glyphs designed for the dark
// theme; on the light theme they look washed-out. For the light theme this
// re-tints monochrome glyphs to a dark colour (preserving their alpha) and
// supplies an explicit mid-grey Disabled pixmap, so even file-gated actions
// that start out disabled stay clearly visible instead of fading to near-white.
// Pass monochrome=false for genuinely colourful icons (open/save) to leave
// them untouched. For the dark theme the icon is always returned unchanged.
QIcon themedIcon(const QString& path, const QString& name, bool monochrome = true);

} // namespace Theme

#endif // THEME_H