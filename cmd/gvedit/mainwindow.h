
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_settings.h"

class MdiChild;
class CFrmSettings;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    QMdiArea *mdiArea;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget *window);
    void settings();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);

    QSignalMapper *windowMapper;



    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *graphMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *graphToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QAction *settingsAct;
    QAction *layoutAct;


    CFrmSettings* frmSettings;
};

#endif
