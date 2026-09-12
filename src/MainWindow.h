#pragma once

#include <QMainWindow>

class ModelCanvas;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void setModified();
    void chooseSelect();
    void chooseNode();
    void chooseBeam();
    void chooseColumn();

private:
    bool maybeSave();
    bool saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);
    void createMenusAndToolbar();
    void updateTitle();

    ModelCanvas *m_canvas = nullptr;
    QString m_currentFile;
    bool m_modified = false;

    QAction *m_selectAction = nullptr;
    QAction *m_nodeAction = nullptr;
    QAction *m_beamAction = nullptr;
    QAction *m_columnAction = nullptr;
};
