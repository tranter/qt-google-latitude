#ifndef SELECTADDRESSDLG_H
#define SELECTADDRESSDLG_H

#include <QDialog>
#include <QVariantList>

namespace Ui {
class SelectAddressDlg;
}

class SelectAddressDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectAddressDlg(QWidget *parent = 0);
    ~SelectAddressDlg();

    void initData(const QVariantList& list);
    const QVariant& getCoordinate();
    
private slots:
    void onAcceptedButton();
    void onCellDoubleClicked(int row,int column);

private:
    Ui::SelectAddressDlg *ui;
    QVariantList m_list;
    int m_selectedRow;
};

#endif // SELECTADDRESSDLG_H
