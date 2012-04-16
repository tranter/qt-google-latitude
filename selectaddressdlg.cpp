#include <QDateTime>

#include "selectaddressdlg.h"
#include "ui_selectaddressdlg.h"

SelectAddressDlg::SelectAddressDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectAddressDlg)
{
    ui->setupUi(this);
    m_selectedRow = -1;

    connect(ui->twList, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onCellDoubleClicked(int,int)));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onAcceptedButton()));
}

SelectAddressDlg::~SelectAddressDlg()
{
    delete ui;
}

void SelectAddressDlg::initData(const QVariantList& list)
{
    QTableWidgetItem* itemAddress;
    QTableWidgetItem* itemCountry;
    m_list = list;
    int len = m_list.size();

    ui->twList->setColumnCount(2);
    ui->twList->setColumnWidth(0,400);
    ui->twList->setColumnWidth(1,100);
    ui->twList->setRowCount(len);
    for (int i=0; i<len; i++) {
        itemAddress = new QTableWidgetItem(m_list[i].toMap()["address"].toString());
        ui->twList->setItem(i,0,itemAddress);

        itemCountry = new QTableWidgetItem(m_list[i].toMap()["AddressDetails"].toMap()["Country"].toMap()["CountryName"].toString());
        ui->twList->setItem(i,1,itemCountry);

        ui->twList->setRowHeight(i,20);
    }
    QTableWidgetSelectionRange range(0,0,0,1);
    ui->twList->setRangeSelected(range,true);
}
const QVariant& SelectAddressDlg::getCoordinate()
{
    return m_list[m_selectedRow];
}

void SelectAddressDlg::onAcceptedButton()
{
    m_selectedRow = ui->twList->selectedRanges()[0].topRow();
}

void SelectAddressDlg::onCellDoubleClicked(int row, int /*column*/)
{
    m_selectedRow = row;
    accept();
}
