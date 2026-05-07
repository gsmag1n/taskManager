#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->btnAdd,    &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->btnRemove, &QPushButton::clicked, this, &MainWindow::onRemoveClicked);
    connect(ui->btnClear,  &QPushButton::clicked, this, &MainWindow::onClearClicked);

    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onItemDoubleClicked);

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddClicked()
{
    bool ok = false;
    const QString text = QInputDialog::getText(this,
                                               tr("Новая задача"),
                                               tr("Введите задачу"),
                                               QLineEdit::Normal,
                                               QString(),
                                               &ok);
    if (!ok) {
        return;
    }

    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Задача не может быть пустой"));
        return;
    }

    if (isDuplicate(trimmed)) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Такая задача уже есть в списке"));
        return;
    }

    ui->listWidget->addItem(trimmed);
    updateWindowTitle();
}

void MainWindow::onRemoveClicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Выберите задачу для удаления"));
        return;
    }

    const auto reply = QMessageBox::question(this,
                                             tr("Подтверждение"),
                                             tr("Удалить выбранную задачу?"),
                                             QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    const int row = ui->listWidget->row(item);
    delete item;

    const int count = ui->listWidget->count();
    if (count > 0) {
        const int next = (row < count) ? row : count - 1;
        ui->listWidget->setCurrentRow(next);
    }

    updateWindowTitle();
}

void MainWindow::onClearClicked()
{
    if (ui->listWidget->count() == 0) {
        QMessageBox::information(this, tr("Информация"),
                                 tr("Список уже пуст"));
        return;
    }

    const auto reply = QMessageBox::question(this,
                                             tr("Подтверждение"),
                                             tr("Удалить ВСЕ задачи?"),
                                             QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    ui->listWidget->clear();
    updateWindowTitle();
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    bool ok = false;
    const QString text = QInputDialog::getText(this,
                                               tr("Редактирование задачи"),
                                               tr("Измените текст задачи"),
                                               QLineEdit::Normal,
                                               item->text(),
                                               &ok);
    if (!ok) {
        return;
    }

    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Задача не может быть пустой"));
        return;
    }

    if (trimmed == item->text()) {
        return;
    }

    if (isDuplicate(trimmed)) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Такая задача уже есть в списке"));
        return;
    }

    item->setText(trimmed);
}

void MainWindow::updateWindowTitle()
{
    const int count = ui->listWidget->count();
    setWindowTitle(tr("Менеджер задач — задач: %1").arg(count));
}

bool MainWindow::isDuplicate(const QString &text) const
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        if (ui->listWidget->item(i)->text() == text) {
            return true;
        }
    }
    return false;
}
