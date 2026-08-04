#pragma once

#include <QDockWidget>
#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPixmap>
#include <QPointF>
#include <QPointer>
#include <QPushButton>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <vector>
#include <algorithm>

#include "gateitem.h"

struct Connection {
    QPointer<GateItem> source;
    QPointer<GateItem> target;
    int inputSlot = -1;
    QGraphicsLineItem* line = nullptr;
};

class DashboardWidget : public QMainWindow {
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget* parent = nullptr);

    void setUsername(const QString& username);
    void clearCanvas();

signals:
    void logoutRequested();

private slots:
    void toggleInputA();
    void toggleInputB();
    void updateSelectedGate();
    void connectSelectedItems();
    void disconnectSelectedConnections();
    void deleteSelectedItem();
    void handleLogoutAction();

private:
    void createToolbar();
    void createCanvas();
    void createInspector();
    void refreshCircuit();
    void refreshTruthTable();
    void updateWirePositions();
    bool addConnection(GateItem* source, GateItem* target, int inputSlot);
    void clearConnections();
    void addSceneItem(const QString& type, const QPointF& scenePos);
    void addPrebuiltCircuit(const QString& type, const QPointF& scenePos);
    void removeItemWithConnections(GateItem* item);
    QPixmap createGateIcon(const QString& type, int size = 48);
    void applyDashboardPalette();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    QPointer<GateItem> m_selectedItem;
    QLabel* m_statusLabel;
    QLabel* m_userLabel;
    QPushButton* m_logoutButton;
    QPushButton* m_connectButton;
    QPushButton* m_disconnectButton;
    QPushButton* m_deleteButton;
    QListWidget* m_gatePalette;
    QListWidget* m_prebuiltPalette;
    QTableWidget* m_truthTable;
    std::vector<Connection> m_connections;
    std::vector<AnchorItem*> m_pendingConnectionAnchors;
    bool m_refreshingCircuit;
    bool m_updatingSelection;
};