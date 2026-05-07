#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAbstractItemView>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

namespace {
constexpr auto BaseWindowTitle = "Менеджер задач";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->btnRemove, &QPushButton::clicked, this, &MainWindow::removeTask);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::clearTasks);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::editTask);

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTask()
{
    bool ok = false;
    const QString text = QInputDialog::getText(this,
                                               "Новая задача",
                                               "Введите задачу",
                                               QLineEdit::Normal,
                                               QString(),
                                               &ok)
                             .trimmed();

    if (!ok) {
        return;
    }

    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Задача не может быть пустой");
        return;
    }

    if (taskExists(text)) {
        QMessageBox::warning(this, "Ошибка", "Такая задача уже есть в списке");
        return;
    }

    ui->listWidget->addItem(text);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    updateWindowTitle();
}

void MainWindow::removeTask()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Ошибка", "Выберите задачу для удаления");
        return;
    }

    const int reply = QMessageBox::question(this,
                                            "Подтверждение",
                                            "Удалить выбранную задачу?",
                                            QMessageBox::Yes | QMessageBox::No,
                                            QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    const int removedRow = ui->listWidget->row(item);
    delete ui->listWidget->takeItem(removedRow);

    if (ui->listWidget->count() == 0) {
        ui->listWidget->setCurrentRow(-1);
        ui->listWidget->clearSelection();
    } else if (removedRow < ui->listWidget->count()) {
        ui->listWidget->setCurrentRow(removedRow);
    } else {
        ui->listWidget->setCurrentRow(-1);
        ui->listWidget->clearSelection();
    }

    updateWindowTitle();
}

void MainWindow::clearTasks()
{
    if (ui->listWidget->count() == 0) {
        QMessageBox::information(this, "Информация", "Список уже пуст");
        return;
    }

    const int reply = QMessageBox::question(this,
                                            "Подтверждение",
                                            "Удалить ВСЕ задачи?",
                                            QMessageBox::Yes | QMessageBox::No,
                                            QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    ui->listWidget->clear();
    updateWindowTitle();
}

void MainWindow::editTask(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    bool ok = false;
    const QString text = QInputDialog::getText(this,
                                               "Редактирование задачи",
                                               "Введите задачу",
                                               QLineEdit::Normal,
                                               item->text(),
                                               &ok)
                             .trimmed();

    if (!ok) {
        return;
    }

    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Задача не может быть пустой");
        return;
    }

    if (taskExists(text, item)) {
        QMessageBox::warning(this, "Ошибка", "Такая задача уже есть в списке");
        return;
    }

    item->setText(text);
}

bool MainWindow::taskExists(const QString &text, const QListWidgetItem *ignoredItem) const
{
    for (int row = 0; row < ui->listWidget->count(); ++row) {
        const QListWidgetItem *item = ui->listWidget->item(row);
        if (item != ignoredItem && item->text().trimmed() == text) {
            return true;
        }
    }

    return false;
}

void MainWindow::updateWindowTitle()
{
    setWindowTitle(QString("%1 (%2)").arg(BaseWindowTitle).arg(ui->listWidget->count()));
}
