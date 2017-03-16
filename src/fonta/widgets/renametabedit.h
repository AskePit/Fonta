#ifndef RENAMETABEDIT_H
#define RENAMETABEDIT_H

#include <QLineEdit>

class QTabWidget;

namespace fonta {

class WorkArea;

class RenameTabEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit RenameTabEdit(QTabWidget* tabWidget, WorkArea* workArea, QWidget *parent = 0);
    virtual ~RenameTabEdit(){}

signals:
    void applied();

private slots:
    void keyPressEvent(QKeyEvent* event);
    void focusOutEvent(QFocusEvent* e) { (void)e; deleteLater(); }
private:
    QTabWidget* m_tabWidget;
    WorkArea* m_workArea;

    void apply();
};

} // namespace fonta

#endif // RENAMETABEDIT_H
