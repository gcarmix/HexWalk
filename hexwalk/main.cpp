#include <QApplication>
#include <QIcon>
#include <QStyleFactory>

#include "hexwalkmain.h"
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(hexwalk);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setApplicationName("HexWalk");
    app.setOrganizationName("HexWalk");
    app.setWindowIcon(QIcon(":images/hexwalk.ico"));
    // set style
    app.setStyle(QStyleFactory::create("Fusion"));
    // increase font size for better reading
    QString fontPath1=":/fonts/Helvetica.ttf";
    QString fontPath2=":/fonts/Courier.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath1);
    QFontDatabase::addApplicationFont(fontPath2);
    //if (fontId != -1)
    //{
        QFont font("Helvetica");
        font.setPointSize(font.pointSize()-1);
        app.setFont(font);
    //}
    /*QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize()+2);
    app.setFont(defaultFont);*/
    // modify palette to dark
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    darkPalette.setColor(QPalette::Base,QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,Qt::black);
    darkPalette.setColor(QPalette::Text,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
    darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText,Qt::red);
    darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

    app.setPalette(darkPalette);


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
