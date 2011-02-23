#include "csettings.h"
#include "qmessagebox.h"
#include "qfiledialog.h"
#include <QtGui>
#include "mdichild.h"


QString stripFileExtension(QString fileName)
{
    int idx;
    for (idx=fileName.length();idx >=0 ; idx --)
    {
	if(fileName.mid(idx,1)==".")
	    break;
    }
    return fileName.left(idx);
}


char* graph_reader( char * str, int num, FILE * stream ) //helper function to load / parse graphs from tstring
{
    CFrmSettings* s=reinterpret_cast<CFrmSettings*>(stream); //as ugly as it gets :)
    if (s->cur >= strlen(s->graphData.toUtf8().constData()))
	return NULL;
    strcpy(str,(char*)s->graphData.mid(s->cur,num).toUtf8().constData());
    s->cur=s->cur+num;
    return str;

}


CFrmSettings::CFrmSettings()
{

    aginit();
    this->gvc=gvContext();
    Ui_Dialog tempDia;
    tempDia.setupUi(this);
    graph=NULL;

    connect(WIDGET(QPushButton,pbAdd),SIGNAL(clicked()),this,SLOT(addSlot()));
    connect(WIDGET(QPushButton,pbNew),SIGNAL(clicked()),this,SLOT(newSlot()));
    connect(WIDGET(QPushButton,pbOpen),SIGNAL(clicked()),this,SLOT(openSlot()));
    connect(WIDGET(QPushButton,pbSave),SIGNAL(clicked()),this,SLOT(saveSlot()));
    connect(WIDGET(QPushButton,btnOK),SIGNAL(clicked()),this,SLOT(okSlot()));
    connect(WIDGET(QPushButton,pbOut),SIGNAL(clicked()),this,SLOT(outputSlot()));
}

void CFrmSettings::outputSlot()
{
    QString _filter="Output File(*."+WIDGET(QComboBox,cbExtension)->currentText()+")";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Graph As.."),"/",_filter);
 if (!fileName.isEmpty())
     WIDGET(QLineEdit,leOutput)->setText(fileName);
}
void CFrmSettings::addSlot()
{
    QString _scope=WIDGET (QComboBox,cbScope)->currentText();
    QString _name=WIDGET  (QComboBox,cbName)->currentText();
    QString _value=WIDGET(QLineEdit,leValue)->text();

    if (_value.trimmed().length() == 0)
	QMessageBox::warning(this, tr("GvEdit"),tr("Please enter a value for selected attribute!"),QMessageBox::Ok,QMessageBox::Ok);
    else
    {
	QString str=_scope+"["+_name+"=\"";
	if(WIDGET (QTextEdit,teAttributes)->toPlainText().contains(str))
	{
	    QMessageBox::warning(this, tr("GvEdit"),tr("Attribute is already defined!"),QMessageBox::Ok,QMessageBox::Ok);
	    return;
	}
	else
	{
	    str = str + _value+"\"]";
	    WIDGET (QTextEdit,teAttributes)->setPlainText(WIDGET (QTextEdit,teAttributes)->toPlainText()+str+"\n");

	}
    }
}
void CFrmSettings::helpSlot(){}
void CFrmSettings::cancelSlot(){}
void CFrmSettings::okSlot()
{
    saveContent();
    this->done(drawGraph());
}
void CFrmSettings::newSlot()
{
    WIDGET (QTextEdit,teAttributes)->setPlainText(tr(""));
}
void CFrmSettings::openSlot()
{
 QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/",tr("Text file (*.*)"));
 if (!fileName.isEmpty())
 {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    WIDGET (QTextEdit,teAttributes)->setPlainText(in.readAll());
 }

}
void CFrmSettings::saveSlot(){

    if(WIDGET (QTextEdit,teAttributes)->toPlainText().trimmed().length()==0)
    {
	QMessageBox::warning(this, tr("GvEdit"),tr("Nothing to save!"),QMessageBox::Ok,QMessageBox::Ok);
	return;


    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                "/",
                                                 tr("Text File(*.*)"));
 if (!fileName.isEmpty())
 {

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << WIDGET (QTextEdit,teAttributes)->toPlainText();
    return;
 }

}

QString CFrmSettings::buildOutputFile(QString _fileName)
{
    return QString("sfsdfdf");
}

void CFrmSettings::addAttribute(QString _scope,QString _name,QString _value){}
bool CFrmSettings::loadGraph(MdiChild* m)
{
    cur=0;
    if(graph)
	agclose(graph);
    graphData.clear();
    graphData.append(m->toPlainText());
    setActiveWindow(m);
    return true;

}
bool CFrmSettings::createLayout()
{
    //first attach attributes to graph
    int _pos=graphData.indexOf(tr("{"));
    graphData.replace(_pos,1,"{"+WIDGET(QTextEdit,teAttributes)->toPlainText());

      /* Reset line number and file name;
       * If known, might want to use real name
       */
    agsetfile("<gvedit>");
    graph=agread_usergets(reinterpret_cast<FILE*>(this),(gets_f)graph_reader);
    if(!graph)
	return false;
    Agraph_t* G=this->graph;
    gvLayout (gvc, G, (char*)WIDGET(QComboBox,cbLayout)->currentText().toUtf8().constData()); /* library function */
    return true;
}
bool CFrmSettings::renderLayout()
{
    if(graph)
    {
	QString _fileName(WIDGET(QLineEdit,leOutput)->text());
	_fileName=stripFileExtension(_fileName);
	_fileName=_fileName+"."+WIDGET(QComboBox,cbExtension)->currentText();
	int rv=gvRenderFilename(gvc,graph,(char*)WIDGET(QComboBox,cbExtension)->currentText().toUtf8().constData(),(char*)_fileName.toUtf8().constData());
	this->getActiveWindow()->loadPreview(_fileName);
	if(rv)
	    this->getActiveWindow()->loadPreview(_fileName);
	return rv;

    }
    return false;
}



bool CFrmSettings::loadLayouts()
{
    return false;
}

bool CFrmSettings::loadRenderers()
{
    return false;
}

void CFrmSettings::refreshContent()
{

    WIDGET(QComboBox,cbLayout)->setCurrentIndex(activeWindow->layoutIdx);
    WIDGET(QComboBox,cbExtension)->setCurrentIndex(activeWindow->renderIdx);
    if(!activeWindow->outputFile.isEmpty())
	WIDGET(QLineEdit,leOutput)->setText(activeWindow->outputFile);
    else
	WIDGET(QLineEdit,leOutput)->setText(stripFileExtension(activeWindow->currentFile())+  "."+WIDGET(QComboBox,cbExtension)->currentText());

    WIDGET(QTextEdit,teAttributes)->setText(activeWindow->attributes);
    WIDGET(QCheckBox,chbPreview)->setChecked(activeWindow->preview);
    WIDGET(QCheckBox,chbCairo)->setChecked(activeWindow->applyCairo);

    WIDGET(QLineEdit,leValue)->setText("");

}

void CFrmSettings::saveContent()
{
    activeWindow->layoutIdx=WIDGET(QComboBox,cbLayout)->currentIndex();
    activeWindow->renderIdx=WIDGET(QComboBox,cbExtension)->currentIndex();

    activeWindow->outputFile=WIDGET(QLineEdit,leOutput)->text();
    activeWindow->attributes=WIDGET(QTextEdit,teAttributes)->toPlainText();
    activeWindow->preview= WIDGET(QCheckBox,chbPreview)->isChecked();
    activeWindow->applyCairo= WIDGET(QCheckBox,chbCairo)->isChecked();
}
int CFrmSettings::drawGraph()
{
	    createLayout();
	    renderLayout();
	    return QDialog::Accepted;

}
int CFrmSettings::runSettings(MdiChild* m)
{
    if ((m) && (m==getActiveWindow()))
    {
        if(this->loadGraph(m))
	    return drawGraph();
	else
            return QDialog::Rejected;
    }

    else
	return showSettings(m);

}
int CFrmSettings::showSettings(MdiChild* m)
{

        if(this->loadGraph(m))
	{
	    refreshContent();
	    return this->exec();
	}
	else
	    return QDialog::Rejected;
}

void CFrmSettings::setActiveWindow(MdiChild* m)
{
    this->activeWindow=m;

}
MdiChild* CFrmSettings::getActiveWindow()
{
    return activeWindow;
}



