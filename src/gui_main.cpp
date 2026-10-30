#include "gui_model.h"
#include "config_controller.h"

#include <QApplication>
#include <QColor>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QPushButton>
#include <QMessageBox>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <exception>
#include <string>

namespace {
const char* roadName(int road) {
    static constexpr const char* names[SimModel::kRoads] = {
        "North", "East", "South", "West"
    };
    return road >= 0 && road < SimModel::kRoads ? names[road] : "";
}

QColor signalColor(SimModel::Signal signal) {
    switch (signal) {
        case SimModel::Signal::Green: return QColor(40, 180, 60);
        case SimModel::Signal::Yellow: return QColor(240, 200, 30);
        case SimModel::Signal::Red: return QColor(210, 40, 40);
    }
    return QColor(210, 40, 40);
}

class IntersectionView final : public QWidget {
public:
    explicit IntersectionView(SimModel& model, QWidget* parent = nullptr)
        : QWidget(parent), model_(model) {
        setMinimumSize(600, 500);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(235, 235, 235));

        painter.fillRect(QRect(0, 210, 600, 80), QColor(70, 70, 70));
        painter.fillRect(QRect(260, 0, 80, 500), QColor(70, 70, 70));

        const QColor vehicleColor(30, 90, 200);
        for (int road = 0; road < SimModel::kRoads; ++road) {
            const int count = static_cast<int>(model_.queueLength(road));
            const int shown = count > 8 ? 8 : count;
            for (int i = 0; i < shown; ++i) {
                const int distance = 10 + i * 22;
                QRect vehicle;
                switch (road) {
                    case 0:
                        vehicle = QRect(265, 210 - distance - 18, 30, 18);
                        break;
                    case 1:
                        vehicle = QRect(340 + distance, 215, 18, 30);
                        break;
                    case 2:
                        vehicle = QRect(305, 290 + distance, 30, 18);
                        break;
                    default:
                        vehicle = QRect(260 - distance - 18, 255, 18, 30);
                        break;
                }
                painter.fillRect(vehicle, vehicleColor);
            }
        }

        painter.setPen(Qt::black);
        const QPoint labels[SimModel::kRoads] = {
            QPoint(165, 24), QPoint(390, 200),
            QPoint(390, 475), QPoint(160, 315)
        };
        for (int road = 0; road < SimModel::kRoads; ++road) {
            painter.drawText(
                labels[road],
                QString("%1 queue: %2")
                    .arg(roadName(road))
                    .arg(static_cast<qulonglong>(model_.queueLength(road)))
            );
        }

        const QPoint lights[SimModel::kRoads] = {
            QPoint(245, 195), QPoint(355, 195),
            QPoint(355, 305), QPoint(245, 305)
        };
        for (int road = 0; road < SimModel::kRoads; ++road) {
            painter.setBrush(signalColor(model_.signal(road)));
            painter.setPen(Qt::black);
            painter.drawEllipse(lights[road], 10, 10);
        }
    }

private:
    SimModel& model_;
};

class MainWindow final : public QMainWindow {
public:
    MainWindow() {
        setWindowTitle("Traffic Management System");

        auto* central = new QWidget(this);
        auto* layout = new QHBoxLayout(central);
        auto* view = new IntersectionView(model_, central);
        auto* controls = new QVBoxLayout();
        auto* startButton = new QPushButton("Start", central);
        pauseButton_ = new QPushButton("Pause", central);
        auto* resetButton = new QPushButton("Reset", central);
        statusLabel_ = new QLabel(central);
        statisticsLabel_ = new QLabel(central);
        configFeedbackLabel_ = new QLabel(central);
        configFeedbackLabel_->setWordWrap(true);

        auto* configLayout = new QFormLayout();
        tickMsInput_ = new QLineEdit(central);
        greenTicksInput_ = new QLineEdit(central);
        yellowTicksInput_ = new QLineEdit(central);
        arrivalIntervalInput_ = new QLineEdit(central);
        maxQueueInput_ = new QLineEdit(central);

        updateConfigurationFields(model_.config());

        configLayout->addRow("Timer interval (ms):", tickMsInput_);
        configLayout->addRow("Green duration (ticks):", greenTicksInput_);
        configLayout->addRow("Yellow duration (ticks):", yellowTicksInput_);
        configLayout->addRow("Arrival interval (ticks):", arrivalIntervalInput_);
        configLayout->addRow("Maximum queue size:", maxQueueInput_);
        auto* applyConfigButton = new QPushButton("Apply Configuration", central);

        controls->addWidget(startButton);
        controls->addWidget(pauseButton_);
        controls->addWidget(resetButton);
        controls->addSpacing(15);
        controls->addLayout(configLayout);
        controls->addWidget(applyConfigButton);
        controls->addWidget(configFeedbackLabel_);
        controls->addSpacing(15);
        controls->addWidget(statusLabel_);
        controls->addWidget(statisticsLabel_);
        controls->addStretch();
        layout->addWidget(view, 1);
        layout->addLayout(controls);
        setCentralWidget(central);

        connect(startButton, &QPushButton::clicked, this, [this]() {
            model_.start();
            refresh();
        });
        connect(pauseButton_, &QPushButton::clicked, this, [this]() {
            model_.togglePause();
            refresh();
        });
        connect(resetButton, &QPushButton::clicked, this, [this]() {
            try {
                model_.reset();
                refresh();
            } catch (const std::exception& error) {
                showSimulationError(error);
            }
        });

        connect(applyConfigButton, &QPushButton::clicked, this, [this]() {
            applyConfiguration();
        });

        timer_ = new QTimer(this);
        timer_->setInterval(model_.config().tickMs);
        connect(timer_, &QTimer::timeout, this, [this]() {
            try {
                if (model_.tick()) {
                    refresh();
                }
            } catch (const std::exception& error) {
                timer_->stop();
                showSimulationError(error);
            }
        });
        timer_->start();

        view_ = view;
        refresh();
        resize(1100, 560);
    }

private:
    void applyConfiguration() {
        if (model_.state() == SimModel::State::Paused) {
            configFeedbackLabel_->setText(
                "Resume and reset the simulation before applying settings."
            );
            return;
        }
        if (model_.state() != SimModel::State::Stopped ||
            model_.tickCount() != 0) {
            configFeedbackLabel_->setText(
                "Stop and reset the simulation before applying settings."
            );
            return;
        }

        RawConfig raw;
        raw.tickMs = tickMsInput_->text().toStdString();
        raw.greenTicks = greenTicksInput_->text().toStdString();
        raw.yellowTicks = yellowTicksInput_->text().toStdString();
        raw.arrivalInterval = arrivalIntervalInput_->text().toStdString();
        raw.maxQueue = maxQueueInput_->text().toStdString();

        std::string feedback;
        if (!configController_.submit(raw, feedback)) {
            configFeedbackLabel_->setText(QString::fromStdString(feedback));
            return;
        }

        const SimConfig previousConfig = model_.config();
        if (!model_.setConfig(configController_.current())) {
            std::string ignoredFeedback;
            configController_.submit(
                RawConfig{
                    std::to_string(previousConfig.tickMs),
                    std::to_string(previousConfig.greenTicks),
                    std::to_string(previousConfig.yellowTicks),
                    std::to_string(previousConfig.arrivalInterval),
                    std::to_string(previousConfig.maxQueue)
                },
                ignoredFeedback
            );
            configFeedbackLabel_->setText(
                "The simulation rejected the configuration."
            );
            return;
        }

        timer_->setInterval(model_.config().tickMs);
        updateConfigurationFields(model_.config());
        configFeedbackLabel_->setText(QString::fromStdString(feedback));
        refresh();
    }

    void updateConfigurationFields(const SimConfig& config) {
        tickMsInput_->setText(QString::number(config.tickMs));
        greenTicksInput_->setText(QString::number(config.greenTicks));
        yellowTicksInput_->setText(QString::number(config.yellowTicks));
        arrivalIntervalInput_->setText(QString::number(config.arrivalInterval));
        maxQueueInput_->setText(QString::number(config.maxQueue));
    }

    void showSimulationError(const std::exception& error) {
        QMessageBox::critical(
            this,
            "Simulation error",
            QString("The simulation could not continue: %1")
                .arg(QString::fromLocal8Bit(error.what()))
        );
    }

    void refresh() {
        const char* state = "Stopped";
        if (model_.state() == SimModel::State::Running) {
            state = "Running";
        } else if (model_.state() == SimModel::State::Paused) {
            state = "Paused";
        }

        statusLabel_->setText(
            QString("State: %1\nLogical tick: %2")
                .arg(state)
                .arg(static_cast<qulonglong>(model_.tickCount()))
        );
        statisticsLabel_->setText(
            QString("Generated: %1\nProcessed: %2\nTotal waiting ticks: %3")
                .arg(static_cast<qulonglong>(model_.generatedCount()))
                .arg(static_cast<qulonglong>(model_.processedCount()))
                .arg(static_cast<qulonglong>(model_.totalWaitingTicks()))
        );
        pauseButton_->setText(
            model_.state() == SimModel::State::Paused ? "Resume" : "Pause"
        );
        view_->update();
    }

    ConfigController configController_;
    SimModel model_;
    QTimer* timer_ = nullptr;
    IntersectionView* view_ = nullptr;
    QPushButton* pauseButton_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* statisticsLabel_ = nullptr;
    QLabel* configFeedbackLabel_ = nullptr;
    QLineEdit* tickMsInput_ = nullptr;
    QLineEdit* greenTicksInput_ = nullptr;
    QLineEdit* yellowTicksInput_ = nullptr;
    QLineEdit* arrivalIntervalInput_ = nullptr;
    QLineEdit* maxQueueInput_ = nullptr;
};
}

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    MainWindow window;
    window.show();
    return application.exec();
}
