#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QListWidgetItem;
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void addTask();
    void removeTask();
    void clearTasks();
    void editTask(QListWidgetItem *item);

private:
    bool taskExists(const QString &text, const QListWidgetItem *ignoredItem = nullptr) const;
    void updateWindowTitle();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
