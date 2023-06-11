#ifndef HEXWALKMAIN_H
#define HEXWALKMAIN_H

#include <QMainWindow>
#include "../src/qhexedit.h"


#include "searchdialog.h"
#include "advancedsearchdialog.h"
#include "entropydialog.h"
#include "binanalysisdialog.h"
#include "converterdialog.h"
#include "hashdialog.h"
#include "diffdialog.h"
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QUndoStack;
class QLabel;
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

namespace Ui {
class HexWalkMain;
}

class HexWalkMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit HexWalkMain(QWidget *parent = nullptr);
    ~HexWalkMain();
    void loadFile(const QString &fileName);


protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private slots:
    void about();
    void dataChanged();
    void open();
    void openRecent();
    void findNext();
    bool save();
    bool saveAs();
    void saveSelectionToReadableFile();
    void saveToReadableFile();
    void setAddress(qint64 address);
    void setOverwriteMode(bool mode);
    void setSize(qint64 size);
    void showSearchDialog();
    void showAdvancedSearchDialog();
    void showEntropyDialog();
    void showBinaryDialog();
    void showDiffDialog();
    void showConverterDialog();
    void showHashDialog();
    void toggleOverwriteMode();
    void gotoAddress();

private:
    void init();
    void adjustForCurrentFile(const QString& filePath);
    void updateRecentActionList();
    QList<QAction*> recentFileActionList;
    QString curFile;
    QFile file;
    QFile hexfile;
    QString diffFile;
    bool isUntitled;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QMenu *analysisMenu;
    QMenu *toolsMenu;
    QMenu *recentFilesMenu;
    QString currentFilePath;
    QToolBar *infoToolBar;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *analysisToolBar;

    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *saveReadable;
    QAction *closeAct;
    QAction *exitAct;
    QAction *binaryAct;
    QAction *overwriteAct;
    QAction *hashAct;
    QAction *diffAct;

    QAction *undoAct;
    QAction *redoAct;
    QAction *saveSelectionReadable;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *findAct;
    QAction *findNextAct;
    QAction *advancedFindAct;
    QAction *entropyAct;
    QAction *converterAct;
    void createActions();
    void createMenus();
    void createStatusBar();
    void createToolBars();
    void setCurrentFile(const QString &fileName);
    void readSettings();
    void writeSettings();
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    Ui::HexWalkMain *ui;
    QHexEdit *hexEdit;

    EntropyDialog *entropyDialog;
    ConverterDialog *converterDialog;
    HashDialog * hashDialog;
    SearchDialog *searchDialog;
    binanalysisdialog * analysisDialog;
    AdvancedSearchDialog *advancedSearchDialog;
    DiffDialog *diffDialog;
    QLabel *lbAddress, *lbAddressName;
    QLabel *lbOverwriteMode, *lbOverwriteModeName;
    QLabel *lbSize, *lbSizeName;
    QLabel * gotoLbl;
    QLineEdit * gotoText;
};

#endif // HEXWALKMAIN_H
