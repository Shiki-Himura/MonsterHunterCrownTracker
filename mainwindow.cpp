#include "mainwindow.h"
#include "crownoverlaywidget.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QSettings>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CreateMonsterGrid();
}

void MainWindow::CreateMonsterGrid()
{
    int monsterAmount = 26;
    QGridLayout *grid = new QGridLayout(this);
    ui->centralwidget->setLayout(grid);
    QPixmap crownLeft(":/icons/MHWilds_Icons/small_crown.png");
    QPixmap crownRight(":/icons/MHWilds_Icons/big_crown.png");

    QSettings settings("Shiki", "CrownTracker");

    for (int i = 0; i <= monsterAmount; i++)
    {
        CrownOverlayWidget *widget = new CrownOverlayWidget(this);
        widget->setIndex(i);
        widget->setMainIcon(QPixmap(QString(":/icons/MHWilds_Icons/monster (%1).png").arg(i)));
        widget->setCrownIcons(crownLeft, crownRight);

        // Load saved state from settings
        bool leftActive = settings.value(QString("monster_%1_left").arg(i), false).toBool();
        bool rightActive = settings.value(QString("monster_%1_right").arg(i), false).toBool();
        widget->setCrownState(leftActive, rightActive);

        // Connect the signal to save the state
        connect(widget, &CrownOverlayWidget::crownStateChanged, this,
                [](int index, bool leftActive, bool rightActive)
                {
                    QSettings settings("Shiki", "CrownTracker");
                    settings.setValue(QString("monster_%1_left").arg(index), leftActive);
                    settings.setValue(QString("monster_%1_right").arg(index), rightActive);
                });

        grid->addWidget(widget, i / 6, i % 6);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionReset_Crowns_triggered()
{
    QSettings settings("Shiki", "CrownTracker");
    for (int i = 0; i <= 26; i++)
    {
        settings.remove(QString("monster_%1_left").arg(i));
        settings.remove(QString("monster_%1_right").arg(i));

        // Reset the crown state in the UI
        CrownOverlayWidget *widget = qobject_cast<CrownOverlayWidget *>(ui->centralwidget->layout()->itemAt(i)->widget());
        widget->setCrownState(false, false);
    }

    CreateMonsterGrid();
}

