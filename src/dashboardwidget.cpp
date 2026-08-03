#include "dashboardwidget.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QHeaderView>
#include <QListWidgetItem>
#include <QMimeData>
#include <QMessageBox>
#include <QPalette>
#include <QPainter>
#include <QPen>
#include <QShortcut>
#include <QSize>
#include <QStatusBar>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QWheelEvent>


DashboardWidget::DashboardWidget(QWidget* parent)
    : QMainWindow(parent),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this)),
      m_selectedItem(nullptr),
      m_statusLabel(new QLabel("Drag inputs, gates, or outputs from the palette", this)),
    m_userLabel(new QLabel(this)),
    m_logoutButton(new QPushButton("Logout", this)),
      m_connectButton(new QPushButton("Connect selected", this)),
      m_disconnectButton(new QPushButton("Disconnect wires", this)),
      m_deleteButton(new QPushButton("Delete", this)),
      m_gatePalette(new QListWidget(this)),
      m_prebuiltPalette(new QListWidget(this)),
      m_truthTable(new QTableWidget(this)),
      m_refreshingCircuit(false),
      m_updatingSelection(false) {
    setWindowTitle("Jhatkaa - Digital Logic Gate Simulator");
    resize(1280, 800);

    applyDashboardPalette();
    createToolbar();
    createCanvas();
    createInspector();

    m_gatePalette->setSelectionMode(QAbstractItemView::SingleSelection);
    m_gatePalette->setDragEnabled(true);
    m_gatePalette->setDragDropMode(QAbstractItemView::DragOnly);
    m_prebuiltPalette->setSelectionMode(QAbstractItemView::SingleSelection);
    m_prebuiltPalette->setDragEnabled(true);
    m_prebuiltPalette->setDragDropMode(QAbstractItemView::DragOnly);

    m_view->viewport()->setAcceptDrops(true);
    m_view->viewport()->installEventFilter(this);

    connect(m_gatePalette, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        addSceneItem(item->text(), QPointF(100, 100));
    });
    connect(m_prebuiltPalette, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        addPrebuiltCircuit(item->text(), QPointF(120, 140));
    });
    connect(m_connectButton, &QPushButton::clicked, this, &DashboardWidget::connectSelectedItems);
    connect(m_disconnectButton, &QPushButton::clicked, this, &DashboardWidget::disconnectSelectedConnections);
    connect(m_deleteButton, &QPushButton::clicked, this, &DashboardWidget::deleteSelectedItem);
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &DashboardWidget::updateSelectedGate);

    auto* connectShortcut = new QShortcut(QKeySequence(Qt::Key_C), this);
    connect(connectShortcut, &QShortcut::activated, this, &DashboardWidget::connectSelectedItems);

    auto* deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteShortcut, &QShortcut::activated, this, &DashboardWidget::deleteSelectedItem);

    m_connectButton->setEnabled(false);
    m_disconnectButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
}

void DashboardWidget::applyDashboardPalette() {
    QFont font(QStringLiteral("Segoe UI"), 10);
    setFont(font);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#F0F0F0"));
    palette.setColor(QPalette::WindowText, QColor("#000000"));
    palette.setColor(QPalette::Base, QColor("#FFFFFF"));
    palette.setColor(QPalette::AlternateBase, QColor("#F0F0F0"));
    palette.setColor(QPalette::Text, QColor("#000000"));
    palette.setColor(QPalette::Button, QColor("#E1E1E1"));
    palette.setColor(QPalette::ButtonText, QColor("#000000"));
    palette.setColor(QPalette::Highlight, QColor("#0078D7"));
    palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    palette.setColor(QPalette::Light, QColor("#FFFFFF"));
    palette.setColor(QPalette::Dark, QColor("#ADADAD"));
    palette.setColor(QPalette::Mid, QColor("#D0D0D0"));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#A0A0A0"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#A0A0A0"));

    setPalette(palette);
    setAutoFillBackground(true);
    setStyleSheet(R"(
        QWidget, QMainWindow, QDockWidget, QToolBar, QStatusBar, QFrame {
            background-color: #F0F0F0;
            color: #000000;
        }
        QLabel {
            background-color: transparent;
            color: #000000;
            border: none;
            padding: 0;
        }
        QListWidget, QTableWidget, QLineEdit, QPlainTextEdit, QTextEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background-color: #FFFFFF;
            color: #000000;
            border: 1px solid #ADADAD;
        }
        QListWidget::item {
            background-color: #FFFFFF;
            color: #000000;
            padding: 4px;
        }
        QListWidget::item:selected {
            background-color: #E5F1FB;
            color: #000000;
        }
        QPushButton {
            background-color: #E1E1E1;
            color: #000000;
            border: 1px solid #ADADAD;
            padding: 6px 10px;
        }
        QPushButton:hover {
            background-color: #E5F1FB;
        }
        QPushButton:pressed {
            background-color: #CCE4F7;
        }
        QHeaderView::section {
            background-color: #F0F0F0;
            color: #000000;
            border: 1px solid #ADADAD;
            padding: 4px;
        }
        QScrollBar:vertical, QScrollBar:horizontal {
            background-color: #F0F0F0;
            border: 1px solid #ADADAD;
        }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
            background-color: #E1E1E1;
            border: 1px solid #ADADAD;
        }
        QScrollBar::handle:vertical:hover, QScrollBar::handle:horizontal:hover {
            background-color: #E5F1FB;
        }
        QScrollBar::handle:vertical:pressed, QScrollBar::handle:horizontal:pressed {
            background-color: #CCE4F7;
        }
    )");
}

void DashboardWidget::createToolbar() {
    auto* toolbar = addToolBar("Tools");
    toolbar->setMovable(false);
}

void DashboardWidget::createCanvas() {
    auto* content = new QWidget(this);
    auto* layout = new QHBoxLayout(content);

    auto* palettePanel = new QWidget(this);
    palettePanel->setMinimumWidth(288);
    auto* paletteLayout = new QVBoxLayout(palettePanel);
    paletteLayout->addWidget(new QLabel("Click items to add them onto canvas"));
    paletteLayout->addWidget(m_gatePalette);
    paletteLayout->addSpacing(18);
    paletteLayout->addWidget(new QLabel("Pre-built library"));
    paletteLayout->addWidget(m_prebuiltPalette);
    paletteLayout->addStretch();

    m_gatePalette->setMinimumWidth(264);
    m_gatePalette->setFixedHeight(460);
    m_gatePalette->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_gatePalette->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_prebuiltPalette->setFixedHeight(150);
    m_gatePalette->setIconSize(QSize(48, 48));
    m_prebuiltPalette->setIconSize(QSize(48, 48));

    const QStringList gateTypes = {"INPUT", "OUTPUT", "AND", "OR", "NOT", "XOR", "XNOR", "NAND", "NOR"};
    for (const QString& gateType : gateTypes) {
        auto* item = new QListWidgetItem(m_gatePalette);
        item->setText(gateType);
        item->setIcon(createGateIcon(gateType, 48));
    }

    const QStringList prebuiltTypes = {"HALF ADDER", "FULL ADDER"};
    for (const QString& prebuiltType : prebuiltTypes) {
        auto* item = new QListWidgetItem(m_prebuiltPalette);
        item->setText(prebuiltType);
        item->setIcon(createGateIcon(prebuiltType, 48));
    }

    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(0, 0, 4000, 3000);
    m_view->setBackgroundBrush(QColor("#FFFFFF"));
    m_view->setStyleSheet("QGraphicsView { background-color: #FFFFFF; border: 1px solid #ADADAD; }");
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setMinimumSize(700, 600);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    layout->addWidget(palettePanel, 1);
    layout->addWidget(m_view, 4);
    setCentralWidget(content);
}

void DashboardWidget::createInspector() {
    auto* inspector = new QWidget(this);
    auto* inspectorLayout = new QVBoxLayout(inspector);

    auto* clearButton = new QPushButton("Clear", this);
    connect(clearButton, &QPushButton::clicked, this, [this]() {
        clearConnections();
        m_scene->clear();
        m_selectedItem = nullptr;
        m_statusLabel->setText("Canvas cleared");
        refreshTruthTable();
    });

    inspectorLayout->addWidget(new QLabel("Selected item controls"));
    inspectorLayout->addWidget(clearButton);
    inspectorLayout->addWidget(m_connectButton);
    inspectorLayout->addWidget(m_disconnectButton);
    inspectorLayout->addWidget(m_deleteButton);
    inspectorLayout->addWidget(new QLabel("Truth Table / State"));
    inspectorLayout->addWidget(m_truthTable);
    inspectorLayout->addWidget(m_statusLabel);
    inspectorLayout->addStretch();

    m_truthTable->setColumnCount(3);
    m_truthTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_truthTable->setHorizontalHeaderLabels({"A", "B", "Output"});
    m_truthTable->horizontalHeader()->setStretchLastSection(true);
    m_truthTable->verticalHeader()->setVisible(false);
    m_truthTable->setMinimumHeight(220);

    auto* inspectorDock = new QDockWidget("Inspector", this);
    inspectorDock->setWidget(inspector);
    inspectorDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    inspectorDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);
    inspectorDock->show();
    inspectorDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    auto* anchorHintLabel = new QLabel("Ctrl+click anchors to select them", this);
    auto* connectHintLabel = new QLabel("C = connect selected", this);
    auto* deleteHintLabel = new QLabel("Del = delete", this);
    m_userLabel->setText("Welcome.");
    connect(m_logoutButton, &QPushButton::clicked, this, &DashboardWidget::handleLogoutAction);
    statusBar()->addWidget(m_userLabel);
    statusBar()->addWidget(m_logoutButton);
    statusBar()->addPermanentWidget(anchorHintLabel);
    statusBar()->addPermanentWidget(connectHintLabel);
    statusBar()->addPermanentWidget(deleteHintLabel);
}

bool DashboardWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_view->viewport()) {
        if (event->type() == QEvent::DragEnter) {
            auto* dragEvent = static_cast<QDragEnterEvent*>(event);
            if (dragEvent->mimeData()->hasText()) {
                dragEvent->acceptProposedAction();
                return true;
            }
        }
        if (event->type() == QEvent::DragMove) {
            auto* dragEvent = static_cast<QDragMoveEvent*>(event);
            if (dragEvent->mimeData()->hasText()) {
                dragEvent->acceptProposedAction();
                return true;
            }
        }
        if (event->type() == QEvent::Drop) {
            auto* dropEvent = static_cast<QDropEvent*>(event);
            const QString type = dropEvent->mimeData()->text().trimmed();
            const QPointF scenePos = m_view->mapToScene(dropEvent->position().toPoint());
            addSceneItem(type, scenePos);
            dropEvent->acceptProposedAction();
            return true;
        }
        if (event->type() == QEvent::Wheel) {
            auto* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->modifiers() & Qt::ControlModifier) {
                const qreal factor = wheelEvent->angleDelta().y() > 0 ? 1.1 : 0.9;
                m_view->scale(factor, factor);
                return true;
            }
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            updateWirePositions();
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void DashboardWidget::refreshCircuit() {
    if (m_refreshingCircuit) {
        qDebug() << "refreshCircuit reentered";
        return;
    }

    m_refreshingCircuit = true;
    const QList<QGraphicsItem*> sceneItems = m_scene->items();
    QList<GateItem*> gates;
    for (QGraphicsItem* item : sceneItems) {
        auto* gate = qgraphicsitem_cast<GateItem*>(item);
        if (gate && gate->scene() && gate->scene() == m_scene) {
            gates.append(gate);
        }
    }

    for (int pass = 0; pass < 10; ++pass) {
        bool changed = false;
        for (GateItem* gate : gates) {
            if (!gate || gate->itemKind() == ItemKind::InputSource) {
                continue;
            }
            const bool previous = gate->output();
            gate->evaluate();
            if (gate->output() != previous) {
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
    }

    for (GateItem* gate : gates) {
        if (gate && gate->scene() && !gate->scene()->views().isEmpty()) {
            gate->update();
        }
    }

    updateWirePositions();
    refreshTruthTable();
    m_refreshingCircuit = false;
}

void DashboardWidget::refreshTruthTable() {
    if (m_updatingSelection) {
        return;
    }

    if (!m_selectedItem || !m_selectedItem->scene()) {
        m_truthTable->setRowCount(0);
        m_selectedItem = nullptr;
        return;
    }

    m_truthTable->clearContents();
    m_truthTable->setRowCount(0);
    if (m_selectedItem->itemKind() == ItemKind::InputSource) {
        m_truthTable->setRowCount(2);
        m_truthTable->setItem(0, 0, new QTableWidgetItem(""));
        m_truthTable->setItem(0, 1, new QTableWidgetItem(""));
        m_truthTable->setItem(0, 2, new QTableWidgetItem(m_selectedItem->output() ? "1" : "0"));
        m_truthTable->setItem(1, 0, new QTableWidgetItem(""));
        m_truthTable->setItem(1, 1, new QTableWidgetItem(""));
        m_truthTable->setItem(1, 2, new QTableWidgetItem(m_selectedItem->isConnected() ? "Connected" : "Disconnected"));
    } else if (m_selectedItem->itemKind() == ItemKind::OutputSink) {
        m_truthTable->setRowCount(2);
        m_truthTable->setItem(0, 0, new QTableWidgetItem(""));
        m_truthTable->setItem(0, 1, new QTableWidgetItem(""));
        m_truthTable->setItem(0, 2, new QTableWidgetItem(m_selectedItem->output() ? "1" : "0"));
        m_truthTable->setItem(1, 0, new QTableWidgetItem(""));
        m_truthTable->setItem(1, 1, new QTableWidgetItem(""));
        m_truthTable->setItem(1, 2, new QTableWidgetItem(m_selectedItem->isConnected() ? "Connected" : "Disconnected"));
    } else {
        const auto type = m_selectedItem->gateType();
        if (type == GateType::NOT) {
            m_truthTable->setRowCount(3);
            for (int row = 0; row < 2; ++row) {
                const bool a = row == 1;
                const bool output = LogicEngine::evaluateGate(type, a);
                m_truthTable->setItem(row, 0, new QTableWidgetItem(QString::number(a)));
                m_truthTable->setItem(row, 1, new QTableWidgetItem("-"));
                m_truthTable->setItem(row, 2, new QTableWidgetItem(output ? "1" : "0"));
            }
            m_truthTable->setItem(2, 0, new QTableWidgetItem(""));
            m_truthTable->setItem(2, 1, new QTableWidgetItem(""));
            m_truthTable->setItem(2, 2, new QTableWidgetItem(m_selectedItem->isConnected() ? "Connected" : "Disconnected"));
        } else {
            m_truthTable->setRowCount(5);
            static const bool rows[4][2] = {{false, false}, {false, true}, {true, false}, {true, true}};
            for (int row = 0; row < 4; ++row) {
                const bool a = rows[row][0];
                const bool b = rows[row][1];
                const bool output = LogicEngine::evaluateGate(type, a, b);
                m_truthTable->setItem(row, 0, new QTableWidgetItem(a ? "1" : "0"));
                m_truthTable->setItem(row, 1, new QTableWidgetItem(b ? "1" : "0"));
                m_truthTable->setItem(row, 2, new QTableWidgetItem(output ? "1" : "0"));
            }
            m_truthTable->setItem(4, 0, new QTableWidgetItem(""));
            m_truthTable->setItem(4, 1, new QTableWidgetItem(""));
            m_truthTable->setItem(4, 2, new QTableWidgetItem(m_selectedItem->isConnected() ? "Connected" : "Disconnected"));
        }
    }
    m_truthTable->resizeColumnsToContents();
}

void DashboardWidget::updateWirePositions() {
    for (Connection& connection : m_connections) {
        if (!connection.source || !connection.target || !connection.line) {
            continue;
        }
        if (!connection.source->scene() || !connection.target->scene() || connection.source->scene() != m_scene || connection.target->scene() != m_scene) {
            continue;
        }
        if (!connection.line->scene() || connection.line->scene() != m_scene) {
            continue;
        }
        const QPointF start = connection.source->outputAnchor();
        const QPointF end = connection.target->inputAnchor(connection.inputSlot);
        connection.line->setLine(QLineF(start, end));
    }
}

bool DashboardWidget::addConnection(GateItem* source, GateItem* target, int inputSlot) {
    if (!source || !target || source == target || inputSlot < 0 || inputSlot > 1) {
        return false;
    }

    if (target->itemKind() == ItemKind::OutputSink && inputSlot != 0) {
        m_statusLabel->setText("Output only has one input slot");
        return false;
    }

    if (target->itemKind() == ItemKind::Gate && target->gateType() == GateType::NOT && inputSlot != 0) {
        m_statusLabel->setText("NOT gate only has one input slot");
        return false;
    }

    if (source->itemKind() == ItemKind::OutputSink) {
        m_statusLabel->setText("Cannot use an output sink as a source");
        return false;
    }

    if ((inputSlot == 0 && target->hasInputSource(0)) || (inputSlot == 1 && target->hasInputSource(1))) {
        m_statusLabel->setText("That input already has a connection");
        return false;
    }

    for (const Connection& connection : m_connections) {
        if (connection.target == target && connection.inputSlot == inputSlot) {
            m_statusLabel->setText("That input already has a connection");
            return false;
        }
    }

    if (inputSlot == 0) {
        target->setInputSourceA(source);
    } else {
        target->setInputSourceB(source);
    }

    auto* line = m_scene->addLine(QLineF(source->outputAnchor(), target->inputAnchor(inputSlot)));
    line->setPen(QPen(Qt::darkCyan, 3));
    line->setZValue(-1);
    line->setFlag(QGraphicsItem::ItemIsSelectable, false);
    line->setAcceptedMouseButtons(Qt::NoButton);

    m_connections.push_back({source, target, inputSlot, line});
    source->setConnected(true);
    target->setConnected(true);
    m_statusLabel->setText("Wire added");
    updateWirePositions();
    refreshCircuit();
    return true;
}

void DashboardWidget::clearConnections() {
    for (const Connection& connection : std::as_const(m_connections)) {
        if (connection.line) {
            m_scene->removeItem(connection.line);
            delete connection.line;
        }
        if (connection.target) {
            if (connection.inputSlot == 0) {
                connection.target->setInputSourceA(nullptr);
            } else {
                connection.target->setInputSourceB(nullptr);
            }
        }
    }
    m_connections.clear();
    for (QGraphicsItem* graphicsItem : m_scene->items()) {
        if (auto* gate = qgraphicsitem_cast<GateItem*>(graphicsItem)) {
            gate->setConnected(false);
        }
    }
}

void DashboardWidget::removeItemWithConnections(GateItem* item) {
    std::vector<Connection> remaining;
    for (const Connection& connection : std::as_const(m_connections)) {
        if (connection.source == item || connection.target == item) {
            if (connection.line) {
                m_scene->removeItem(connection.line);
                delete connection.line;
            }
            if (connection.target) {
                if (connection.inputSlot == 0) {
                    connection.target->setInputSourceA(nullptr);
                } else {
                    connection.target->setInputSourceB(nullptr);
                }
            }
            continue;
        }
        remaining.push_back(connection);
    }
    m_connections = std::move(remaining);
    for (QGraphicsItem* graphicsItem : m_scene->items()) {
        if (auto* gate = qgraphicsitem_cast<GateItem*>(graphicsItem)) {
            gate->setConnected(false);
        }
    }
    for (const Connection& connection : std::as_const(m_connections)) {
        if (connection.source) {
            connection.source->setConnected(true);
        }
        if (connection.target) {
            connection.target->setConnected(true);
        }
    }
}

void DashboardWidget::disconnectSelectedConnections() {
    if (!m_selectedItem) {
        QMessageBox::information(this, "Jhatkaa", "Select an item to disconnect.");
        return;
    }

    std::vector<Connection> remaining;
    bool removed = false;
    for (const Connection& connection : std::as_const(m_connections)) {
        if (connection.source == m_selectedItem || connection.target == m_selectedItem) {
            if (connection.line) {
                m_scene->removeItem(connection.line);
                delete connection.line;
            }
            if (connection.target) {
                if (connection.inputSlot == 0) {
                    connection.target->setInputSourceA(nullptr);
                } else {
                    connection.target->setInputSourceB(nullptr);
                }
            }
            removed = true;
            continue;
        }
        remaining.push_back(connection);
    }

    m_connections = std::move(remaining);
    for (QGraphicsItem* graphicsItem : m_scene->items()) {
        if (auto* gate = qgraphicsitem_cast<GateItem*>(graphicsItem)) {
            gate->setConnected(false);
        }
    }
    for (const Connection& connection : std::as_const(m_connections)) {
        if (connection.source) {
            connection.source->setConnected(true);
        }
        if (connection.target) {
            connection.target->setConnected(true);
        }
    }

    m_statusLabel->setText(removed ? "Wires disconnected" : "No wires to disconnect");
    refreshCircuit();
}

void DashboardWidget::deleteSelectedItem() {
    if (!m_selectedItem) {
        QMessageBox::information(this, "Jhatkaa", "Select an item to delete.");
        return;
    }
    removeItemWithConnections(m_selectedItem);
    m_scene->removeItem(m_selectedItem);
    delete m_selectedItem;
    m_selectedItem = nullptr;
    m_statusLabel->setText("Item deleted");
    refreshCircuit();
}

void DashboardWidget::addSceneItem(const QString& type, const QPointF& scenePos) {
    const QString kind = type.trimmed().toUpper();
    GateItem* item = nullptr;
    if (kind == "INPUT") {
        item = new GateItem(ItemKind::InputSource);
    } else if (kind == "OUTPUT") {
        item = new GateItem(ItemKind::OutputSink);
    } else {
        GateType gateType = GateType::AND;
        if (kind == "OR") gateType = GateType::OR;
        else if (kind == "NOT") gateType = GateType::NOT;
        else if (kind == "XOR") gateType = GateType::XOR;
        else if (kind == "XNOR") gateType = GateType::XNOR;
        else if (kind == "NAND") gateType = GateType::NAND;
        else if (kind == "NOR") gateType = GateType::NOR;
        item = new GateItem(ItemKind::Gate, gateType);
    }
    item->setPos(scenePos);
    item->setSelected(true);
    m_scene->addItem(item);
    connect(item, &GateItem::toggled, this, [this]() {
        refreshCircuit();
    });
    m_selectedItem = item;
    refreshCircuit();
    m_statusLabel->setText(QString("Added %1").arg(type));
}

void DashboardWidget::addPrebuiltCircuit(const QString& type, const QPointF& scenePos) {
    const QString kind = type.trimmed().toUpper();
    auto addPrefabItem = [this](GateItem* item) {
        m_scene->addItem(item);
        connect(item, &GateItem::toggled, this, [this]() {
            refreshCircuit();
        });
    };

    if (kind == "HALF ADDER") {
        auto* inputA = new GateItem(ItemKind::InputSource);
        auto* inputB = new GateItem(ItemKind::InputSource);
        auto* xorGate = new GateItem(ItemKind::Gate, GateType::XOR);
        auto* andGate = new GateItem(ItemKind::Gate, GateType::AND);
        auto* sumOutput = new GateItem(ItemKind::OutputSink);
        auto* carryOutput = new GateItem(ItemKind::OutputSink);

        inputA->setPos(scenePos + QPointF(0, 0));
        inputB->setPos(scenePos + QPointF(0, 100));
        xorGate->setPos(scenePos + QPointF(180, 0));
        andGate->setPos(scenePos + QPointF(180, 100));
        sumOutput->setPos(scenePos + QPointF(360, 0));
        carryOutput->setPos(scenePos + QPointF(360, 100));

        addPrefabItem(inputA);
        addPrefabItem(inputB);
        addPrefabItem(xorGate);
        addPrefabItem(andGate);
        addPrefabItem(sumOutput);
        addPrefabItem(carryOutput);

        addConnection(inputA, xorGate, 0);
        addConnection(inputB, xorGate, 1);
        addConnection(inputA, andGate, 0);
        addConnection(inputB, andGate, 1);
        addConnection(xorGate, sumOutput, 0);
        addConnection(andGate, carryOutput, 0);

        m_selectedItem = sumOutput;
        m_statusLabel->setText("Added half adder");
    } else if (kind == "FULL ADDER") {
        auto* inputA = new GateItem(ItemKind::InputSource);
        auto* inputB = new GateItem(ItemKind::InputSource);
        auto* inputCin = new GateItem(ItemKind::InputSource);
        auto* xorGate1 = new GateItem(ItemKind::Gate, GateType::XOR);
        auto* xorGate2 = new GateItem(ItemKind::Gate, GateType::XOR);
        auto* andGate1 = new GateItem(ItemKind::Gate, GateType::AND);
        auto* andGate2 = new GateItem(ItemKind::Gate, GateType::AND);
        auto* orGate = new GateItem(ItemKind::Gate, GateType::OR);
        auto* sumOutput = new GateItem(ItemKind::OutputSink);
        auto* carryOutput = new GateItem(ItemKind::OutputSink);

        inputA->setPos(scenePos + QPointF(0, 0));
        inputB->setPos(scenePos + QPointF(0, 90));
        inputCin->setPos(scenePos + QPointF(0, 180));
        xorGate1->setPos(scenePos + QPointF(180, 0));
        xorGate2->setPos(scenePos + QPointF(360, 0));
        andGate1->setPos(scenePos + QPointF(360, 90));
        andGate2->setPos(scenePos + QPointF(360, 180));
        orGate->setPos(scenePos + QPointF(540, 90));
        sumOutput->setPos(scenePos + QPointF(720, 0));
        carryOutput->setPos(scenePos + QPointF(720, 90));

        addPrefabItem(inputA);
        addPrefabItem(inputB);
        addPrefabItem(inputCin);
        addPrefabItem(xorGate1);
        addPrefabItem(xorGate2);
        addPrefabItem(andGate1);
        addPrefabItem(andGate2);
        addPrefabItem(orGate);
        addPrefabItem(sumOutput);
        addPrefabItem(carryOutput);

        addConnection(inputA, xorGate1, 0);
        addConnection(inputB, xorGate1, 1);
        addConnection(xorGate1, xorGate2, 0);
        addConnection(inputCin, xorGate2, 1);
        addConnection(xorGate1, andGate1, 0);
        addConnection(inputCin, andGate1, 1);
        addConnection(inputA, andGate2, 0);
        addConnection(inputB, andGate2, 1);
        addConnection(andGate1, orGate, 0);
        addConnection(andGate2, orGate, 1);
        addConnection(xorGate2, sumOutput, 0);
        addConnection(orGate, carryOutput, 0);

        m_selectedItem = sumOutput;
        m_statusLabel->setText("Added full adder");
    } else {
        m_statusLabel->setText("Unknown pre-built circuit");
        return;
    }

    refreshCircuit();
}

void DashboardWidget::toggleInputA() {
    if (!m_selectedItem) {
        QMessageBox::information(this, "Jhatkaa", "Select an input item first.");
        return;
    }

    if (m_selectedItem->itemKind() == ItemKind::InputSource) {
        m_selectedItem->toggleValue();
        refreshCircuit();
        m_statusLabel->setText("Input toggled");
    } else {
        m_statusLabel->setText("Select an input source to toggle.");
    }
}

void DashboardWidget::toggleInputB() {
    Q_UNUSED(m_selectedItem);
}

void DashboardWidget::updateSelectedGate() {
    m_updatingSelection = true;
    const QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

    QList<AnchorItem*> selectedAnchors;
    QList<GateItem*> selectedGates;
    for (QGraphicsItem* item : selectedItems) {
        if (auto* anchor = qgraphicsitem_cast<AnchorItem*>(item)) {
            selectedAnchors.append(anchor);
        } else if (auto* gate = qgraphicsitem_cast<GateItem*>(item)) {
            selectedGates.append(gate);
        }
    }

    if (!selectedAnchors.isEmpty()) {
        m_selectedItem = selectedAnchors.last()->gate();
    } else if (!selectedGates.isEmpty()) {
        m_selectedItem = selectedGates.last();
    } else {
        m_selectedItem = nullptr;
    }

    const int selectedAnchorCount = selectedAnchors.size();
    const int selectedGateCount = selectedGates.size();
    if (selectedAnchorCount == 1 && m_selectedItem) {
        const QString label = m_selectedItem->itemKind() == ItemKind::Gate
            ? QString::fromStdString(LogicEngine::gateName(m_selectedItem->gateType()))
            : (m_selectedItem->itemKind() == ItemKind::InputSource ? "INPUT" : "OUTPUT");
        const QString connectionState = m_selectedItem->isConnected() ? "connected" : "disconnected";
        m_statusLabel->setText(QString("Selected connection node on %1 (%2)").arg(label, connectionState));
    } else if (selectedAnchorCount == 2) {
        m_statusLabel->setText("Two connection nodes selected. Press Connect selected to wire them.");
    } else if (selectedGateCount == 1 && m_selectedItem) {
        const QString label = m_selectedItem->itemKind() == ItemKind::Gate
            ? QString::fromStdString(LogicEngine::gateName(m_selectedItem->gateType()))
            : (m_selectedItem->itemKind() == ItemKind::InputSource ? "INPUT" : "OUTPUT");
        m_statusLabel->setText(QString("Selected %1").arg(label));
    } else if (selectedGateCount > 1) {
        m_statusLabel->setText(QString("%1 items selected").arg(selectedGateCount));
    } else {
        m_statusLabel->setText("Drag inputs, gates, or outputs from the palette");
    }

    m_connectButton->setEnabled(selectedAnchorCount == 2);
    m_disconnectButton->setEnabled(m_selectedItem != nullptr);
    m_deleteButton->setEnabled(m_selectedItem != nullptr);

    refreshTruthTable();
    m_updatingSelection = false;
}

void DashboardWidget::connectSelectedItems() {
    const QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    QList<AnchorItem*> selectedAnchors;
    for (QGraphicsItem* item : selectedItems) {
        if (auto* anchor = qgraphicsitem_cast<AnchorItem*>(item)) {
            selectedAnchors.append(anchor);
        }
    }

    if (selectedAnchors.size() != 2) {
        m_statusLabel->setText("Select two anchors to connect.");
        return;
    }

    AnchorItem* first = selectedAnchors.at(0);
    AnchorItem* second = selectedAnchors.at(1);
    GateItem* firstGate = first->gate();
    GateItem* secondGate = second->gate();

    if (!firstGate || !secondGate || firstGate == secondGate) {
        m_statusLabel->setText("Choose two different items.");
        return;
    }

    AnchorItem* sourceAnchor = nullptr;
    AnchorItem* targetAnchor = nullptr;
    if (first->role() == AnchorItem::AnchorRole::Output && second->role() != AnchorItem::AnchorRole::Output) {
        sourceAnchor = first;
        targetAnchor = second;
    } else if (second->role() == AnchorItem::AnchorRole::Output && first->role() != AnchorItem::AnchorRole::Output) {
        sourceAnchor = second;
        targetAnchor = first;
    } else {
        m_statusLabel->setText("Connect an output anchor to an input anchor.");
        return;
    }

    const int inputSlot = targetAnchor->inputSlot();
    if (!addConnection(sourceAnchor->gate(), targetAnchor->gate(), inputSlot)) {
        return;
    }

    sourceAnchor->setSelected(false);
    targetAnchor->setSelected(false);
    m_scene->clearSelection();
}

QPixmap DashboardWidget::createGateIcon(const QString& type, int size) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QColor("#FFFFFF"));
    painter.setPen(QPen(QColor("#ADADAD"), 2));
    painter.drawRoundedRect(4, 4, size - 8, size - 8, 8, 8);
    painter.setPen(QColor("#000000"));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, type);
    return pixmap;
}

void DashboardWidget::handleLogoutAction() {
    emit logoutRequested();
}

void DashboardWidget::setUsername(const QString& username) {
    if (username.trimmed().isEmpty()) {
        m_userLabel->setText("Welcome.");
        return;
    }

    m_userLabel->setText(QString("Welcome, %1").arg(username.trimmed()));
}