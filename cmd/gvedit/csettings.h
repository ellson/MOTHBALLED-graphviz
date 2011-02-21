
#ifndef CSETTINGS_H
#define CSETTINGS_H

class MdiChild;
#include <QDialog>
#include <QString>
#include "ui_settings.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gvc.h"
#include "gvio.h"


class CFrmSettings : public QDialog
{
        Q_OBJECT
public:
    CFrmSettings();
    int runSettings(MdiChild* m);
    int showSettings(MdiChild* m);
    int cur;
    int drawGraph();
    MdiChild* getActiveWindow();
    QString graphData;
private slots:
    void outputSlot();
    void addSlot();
    void helpSlot();
    void cancelSlot();
    void okSlot();
    void newSlot();
    void openSlot();
    void saveSlot();
private:
    //Actions
    Agraph_t* graph;
    MdiChild* activeWindow;
    GVC_t* gvc;    
    QAction* outputAct;
    QAction* addAct;
    QAction* helpAct;
    QAction* cancelAct;
    QAction* okAct;
    QAction* newAct;
    QAction* openAct;
    QAction* saveAct;
    //METHODS
    QString buildOutputFile(QString _fileName);
    void addAttribute(QString _scope,QString _name,QString _value);
    bool loadLayouts();
    bool loadRenderers();
    void refreshContent();
    void saveContent();
    void setActiveWindow(MdiChild* m);
    bool loadGraph(MdiChild* m);
    bool createLayout();
    bool renderLayout();


};

#endif
