#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QListWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onClearClicked();
    void onItemDoubleClicked(QListWidgetItem *item);

private:
    void updateWindowTitle();
    bool isDuplicate(const QString &text) const;

    Ui::MainWindow *ui;
};

#endif
