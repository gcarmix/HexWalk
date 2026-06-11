/*
 * Copyright (C) 2025 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HexWalk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <QApplication>
#include <QIcon>
#include <QSettings>
#include <QStyleFactory>

#include "hexwalkmain.h"
#include "theme.h"
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(hexwalk);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    app.setApplicationName("HexWalk");
    app.setOrganizationName("HexWalk");
    app.setWindowIcon(QIcon(":images/hexwalk.ico"));
    // set style
    app.setStyle(QStyleFactory::create("Fusion"));
    // increase font size for better reading
    QString fontPath1=":/fonts/Roboto.ttf";
    QString fontPath2=":/fonts/Courier.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath1);
    QFontDatabase::addApplicationFont(fontPath2);
    //if (fontId != -1)
    //{
        QFont font("Roboto");
        font.setPointSize(font.pointSize()-2);
        app.setFont(font);
    //}
    /*QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize()+2);
    app.setFont(defaultFont);*/
    // apply the palette for the saved theme (defaults to dark).
    // Must use the same store key as HexWalkMain ("hexwalk","hexwalk").
    QSettings appSettings("hexwalk", "hexwalk");
    QString theme = appSettings.value("Theme", Theme::kDark).toString();
    app.setPalette(Theme::paletteFor(theme));


    // Identify locale and load translation if available
    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("hexwalk_") + locale);
    app.installTranslator(&translator);

    QCommandLineParser parser;
    parser.addPositionalArgument("file", "File to open");
    parser.addHelpOption();
    parser.process(app);

    /*MainWindow *mainWin = new MainWindow;
    if(!parser.positionalArguments().isEmpty())
        mainWin->loadFile(parser.positionalArguments().at(0));
    mainWin->show();*/
    HexWalkMain *hexwalkWin = new HexWalkMain;
    if(!parser.positionalArguments().isEmpty())
        hexwalkWin->loadFile(parser.positionalArguments().at(0));
    hexwalkWin->show();

    return app.exec();
}
