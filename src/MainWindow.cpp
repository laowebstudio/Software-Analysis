#include "MainWindow.h"
#include "ModelCanvas.h"

#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_canvas(new ModelCanvas(this))
{
    setCentralWidget(m_canvas);
    resize(1180, 760);

    createMenusAndToolbar();
    statusBar()->showMessage("Ready");

    connect(m_canvas, &ModelCanvas::modelChanged, this, &MainWindow::setModified);
    connect(m_canvas, &ModelCanvas::statusMessage, statusBar(), &QStatusBar::showMessage);

    updateTitle();
}

void MainWindow::createMenusAndToolbar()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *newAction = fileMenu->addAction("&New Project");
    QAction *openAction = fileMenu->addAction("&Open...");
    QAction *saveAction = fileMenu->addAction("&Save");
    QAction *saveAsAction = fileMenu->addAction("Save &As...");
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction("&Quit");

    connect(newAction, &QAction::triggered, this, &MainWindow::newProject);
    connect(openAction, &QAction::triggered, this, &MainWindow::openProject);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveProject);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveProjectAs);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    QMenu *drawMenu = menuBar()->addMenu("&Draw");
    m_selectAction = drawMenu->addAction("Select");
    m_nodeAction = drawMenu->addAction("Node");
    m_beamAction = drawMenu->addAction("Beam");
    m_columnAction = drawMenu->addAction("Column");

    QActionGroup *tools = new QActionGroup(this);
    for (QAction *action : {m_selectAction, m_nodeAction, m_beamAction, m_columnAction}) {
        action->setCheckable(true);
        tools->addAction(action);
    }
    m_selectAction->setChecked(true);

    connect(m_selectAction, &QAction::triggered, this, &MainWindow::chooseSelect);
    connect(m_nodeAction, &QAction::triggered, this, &MainWindow::chooseNode);
    connect(m_beamAction, &QAction::triggered, this, &MainWindow::chooseBeam);
    connect(m_columnAction, &QAction::triggered, this, &MainWindow::chooseColumn);

    QToolBar *toolbar = addToolBar("Modeling");
    toolbar->setMovable(false);
    toolbar->addAction(newAction);
    toolbar->addAction(openAction);
    toolbar->addAction(saveAction);
    toolbar->addSeparator();
    toolbar->addAction(m_selectAction);
    toolbar->addAction(m_nodeAction);
    toolbar->addAction(m_beamAction);
    toolbar->addAction(m_columnAction);
}

void MainWindow::newProject()
{
    if (!maybeSave())
        return;

    m_canvas->clearModel();
    m_currentFile.clear();
    m_modified = false;
    updateTitle();
}

void MainWindow::openProject()
{
    if (!maybeSave())
        return;

    const QString fileName = QFileDialog::getOpenFileName(this, "Open SAPUDOM Project", {}, "SAPUDOM Project (*.sapudom);;JSON (*.json)");
    if (!fileName.isEmpty())
        loadFromFile(fileName);
}

void MainWindow::saveProject()
{
    if (m_currentFile.isEmpty())
        saveProjectAs();
    else
        saveToFile(m_currentFile);
}

void MainWindow::saveProjectAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save SAPUDOM Project", m_currentFile, "SAPUDOM Project (*.sapudom)");
    if (fileName.isEmpty())
        return;
    if (!fileName.endsWith(".sapudom", Qt::CaseInsensitive))
        fileName += ".sapudom";
    saveToFile(fileName);
}

bool MainWindow::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Save Error", "Cannot write project file.");
        return false;
    }

    file.write(QJsonDocument(m_canvas->toJson()).toJson(QJsonDocument::Indented));
    m_currentFile = fileName;
    m_modified = false;
    updateTitle();
    statusBar()->showMessage("Project saved", 2500);
    return true;
}

bool MainWindow::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Open Error", "Cannot open project file.");
        return false;
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject() || !m_canvas->fromJson(doc.object())) {
        QMessageBox::critical(this, "Open Error", "Invalid SAPUDOM project file.");
        return false;
    }

    m_currentFile = fileName;
    m_modified = false;
    updateTitle();
    statusBar()->showMessage("Project opened", 2500);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!m_modified)
        return true;

    const auto result = QMessageBox::warning(this, "SAPUDOM", "The project has unsaved changes.", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (result == QMessageBox::Save) {
        saveProject();
        return !m_modified;
    }
    return result == QMessageBox::Discard;
}

void MainWindow::setModified()
{
    m_modified = true;
    updateTitle();
}

void MainWindow::updateTitle()
{
    const QString name = m_currentFile.isEmpty() ? "Untitled.sapudom" : QFileInfo(m_currentFile).fileName();
    setWindowTitle(QString("%1%2 — SAPUDOM Structural Analysis V0.1").arg(name, m_modified ? " *" : ""));
}

void MainWindow::chooseSelect() { m_canvas->setTool(ModelCanvas::Tool::Select); statusBar()->showMessage("Select tool"); }
void MainWindow::chooseNode() { m_canvas->setTool(ModelCanvas::Tool::Node); statusBar()->showMessage("Click grid to create node"); }
void MainWindow::chooseBeam() { m_canvas->setTool(ModelCanvas::Tool::Beam); statusBar()->showMessage("Click start and end points for beam"); }
void MainWindow::chooseColumn() { m_canvas->setTool(ModelCanvas::Tool::Column); statusBar()->showMessage("Click start and end points for column"); }
