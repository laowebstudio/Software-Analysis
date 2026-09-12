#include "ModelCanvas.h"

#include <QPainter>
#include <QMouseEvent>
#include <QJsonArray>
#include <QLineF>
#include <QtMath>

ModelCanvas::ModelCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(800, 520);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void ModelCanvas::setTool(Tool tool)
{
    m_tool = tool;
    m_startNode = -1;
    m_selectedNode = -1;
    update();
}

void ModelCanvas::clearModel()
{
    m_nodes.clear();
    m_members.clear();
    m_startNode = -1;
    m_selectedNode = -1;
    emit modelChanged();
    update();
}

QPointF ModelCanvas::snapToGrid(const QPointF &p) const
{
    const double x = qRound(p.x() / m_grid) * m_grid;
    const double y = qRound(p.y() / m_grid) * m_grid;
    return QPointF(x, y);
}

int ModelCanvas::findNodeNear(const QPointF &p) const
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (QLineF(m_nodes[i], p).length() <= m_pickRadius)
            return i;
    }
    return -1;
}

int ModelCanvas::findOrCreateNode(const QPointF &p)
{
    const QPointF snapped = snapToGrid(p);
    const int existing = findNodeNear(snapped);
    if (existing >= 0)
        return existing;

    m_nodes.push_back(snapped);
    emit modelChanged();
    return m_nodes.size() - 1;
}

QPointF ModelCanvas::nodeScreenPoint(int index) const
{
    if (index < 0 || index >= m_nodes.size())
        return {};
    return m_nodes[index];
}

void ModelCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().base());

    // Grid
    QPen gridPen(palette().mid().color());
    gridPen.setWidth(1);
    painter.setPen(gridPen);
    for (int x = 0; x < width(); x += m_grid)
        painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += m_grid)
        painter.drawLine(0, y, width(), y);

    // Members
    for (const Member &member : m_members) {
        const QPointF a = nodeScreenPoint(member.a);
        const QPointF b = nodeScreenPoint(member.b);
        QPen memberPen;
        memberPen.setWidth(member.type == "Column" ? 6 : 4);
        painter.setPen(memberPen);
        painter.drawLine(a, b);
    }

    // Preview line
    if ((m_tool == Tool::Beam || m_tool == Tool::Column) && m_startNode >= 0) {
        QPen previewPen(palette().highlight().color());
        previewPen.setStyle(Qt::DashLine);
        previewPen.setWidth(2);
        painter.setPen(previewPen);
        painter.drawLine(nodeScreenPoint(m_startNode), snapToGrid(m_mousePos));
    }

    // Nodes
    for (int i = 0; i < m_nodes.size(); ++i) {
        const QPointF p = m_nodes[i];
        painter.setPen(Qt::NoPen);
        painter.setBrush(i == m_selectedNode ? palette().highlight() : palette().text());
        painter.drawEllipse(p, 5, 5);

        painter.setPen(palette().text().color());
        painter.drawText(p + QPointF(7, -7), QString::number(i + 1));
    }
}

void ModelCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    const QPointF p = event->position();

    if (m_tool == Tool::Node) {
        const int node = findOrCreateNode(p);
        m_selectedNode = node;
        emit statusMessage(QString("Node %1 at (%2, %3)")
                               .arg(node + 1)
                               .arg(m_nodes[node].x())
                               .arg(m_nodes[node].y()));
        update();
        return;
    }

    if (m_tool == Tool::Beam || m_tool == Tool::Column) {
        const int node = findOrCreateNode(p);
        if (m_startNode < 0) {
            m_startNode = node;
            emit statusMessage("Select end point");
        } else if (node != m_startNode) {
            Member member;
            member.a = m_startNode;
            member.b = node;
            member.type = (m_tool == Tool::Beam) ? "Beam" : "Column";
            m_members.push_back(member);
            m_startNode = node; // chain drawing
            emit modelChanged();
            emit statusMessage(member.type + " created");
        }
        update();
        return;
    }

    // Select tool
    m_selectedNode = findNodeNear(p);
    if (m_selectedNode >= 0)
        emit statusMessage(QString("Selected Node %1").arg(m_selectedNode + 1));
    else
        emit statusMessage("Ready");
    update();
}

void ModelCanvas::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->position();
    if (m_startNode >= 0)
        update();
}

QJsonObject ModelCanvas::toJson() const
{
    QJsonObject root;
    root["format"] = "SAPUDOM";
    root["version"] = "0.1";

    QJsonArray nodes;
    for (const QPointF &p : m_nodes) {
        QJsonObject n;
        n["x"] = p.x();
        n["y"] = p.y();
        n["z"] = 0.0;
        nodes.push_back(n);
    }
    root["nodes"] = nodes;

    QJsonArray members;
    for (const Member &m : m_members) {
        QJsonObject obj;
        obj["start"] = m.a;
        obj["end"] = m.b;
        obj["type"] = m.type;
        members.push_back(obj);
    }
    root["members"] = members;

    return root;
}

bool ModelCanvas::fromJson(const QJsonObject &obj)
{
    if (obj.value("format").toString() != "SAPUDOM")
        return false;

    QVector<QPointF> nodes;
    QVector<Member> members;

    for (const QJsonValue &v : obj.value("nodes").toArray()) {
        const QJsonObject n = v.toObject();
        nodes.push_back(QPointF(n.value("x").toDouble(), n.value("y").toDouble()));
    }

    for (const QJsonValue &v : obj.value("members").toArray()) {
        const QJsonObject m = v.toObject();
        Member member;
        member.a = m.value("start").toInt(-1);
        member.b = m.value("end").toInt(-1);
        member.type = m.value("type").toString();
        if (member.a < 0 || member.b < 0 || member.a >= nodes.size() || member.b >= nodes.size())
            return false;
        members.push_back(member);
    }

    m_nodes = nodes;
    m_members = members;
    m_startNode = -1;
    m_selectedNode = -1;
    emit modelChanged();
    update();
    return true;
}
