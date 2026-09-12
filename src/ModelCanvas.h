#pragma once

#include <QWidget>
#include <QPointF>
#include <QVector>
#include <QJsonObject>

class ModelCanvas : public QWidget
{
    Q_OBJECT
public:
    enum class Tool { Select, Node, Beam, Column };

    explicit ModelCanvas(QWidget *parent = nullptr);

    void setTool(Tool tool);
    Tool tool() const { return m_tool; }

    void clearModel();
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &obj);

signals:
    void modelChanged();
    void statusMessage(const QString &message);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    struct Member {
        int a = -1;
        int b = -1;
        QString type;
    };

    QPointF snapToGrid(const QPointF &p) const;
    int findOrCreateNode(const QPointF &p);
    int findNodeNear(const QPointF &p) const;
    QPointF nodeScreenPoint(int index) const;

    Tool m_tool = Tool::Select;
    QVector<QPointF> m_nodes;
    QVector<Member> m_members;

    int m_startNode = -1;
    int m_selectedNode = -1;
    QPointF m_mousePos;

    const int m_grid = 40;
    const double m_pickRadius = 10.0;
};
