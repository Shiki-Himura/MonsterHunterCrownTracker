#include "crownoverlaywidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QSettings>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setFixedSize(780,685);

    CrownOverlayWidget::CreateMonsterNameList();
    CreateMonsterGrid();
}

void MainWindow::CreateMonsterGrid()
{
    QWidget *gridWidget = CrownOverlayWidget::CreateMonsterGrid(this);
    ui->centralwidget->setLayout(new QVBoxLayout);
    ui->centralwidget->layout()->addWidget(gridWidget);
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
    }
    // Remove the old grid widget
    if (ui->centralwidget->layout()->count() > 0) {
        QLayoutItem* item = ui->centralwidget->layout()->takeAt(0);
        if (item->widget()) {
            delete item->widget(); // This will delete all child widgets too
        }
        delete item;
    }

    CreateMonsterGrid();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Crown Tracker",
        "Title:        Crown Tracker\n"
        "Version:   0.2\n"
        "Creator:   Shiki\n\n"
        "Purpose:\n"
        "This application helps you track the crowns in Monster Hunter Wilds.\n\n"
        "For more information, feedback, or suggestions\n"
        "Contact me on Discord:\n\n"
        "shiki_himura\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}
