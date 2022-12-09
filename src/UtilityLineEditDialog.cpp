#include "UtilityLineEditDialog.h"
#include "UtilityLineEdit.h"
#include "TableModel_UtilityInterface.h"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qpushbutton.h>

UtilityLineEditDialog::UtilityLineEditDialog(UtilityTableName tableName, long long int itemID, TableModel* tableModel, SqlUtilityTable& utilityTable, QSqlDatabase& db, QWidget* parent) :
    QDialog(parent),
    m_tableName(tableName),
    m_utilityTable(utilityTable),
    m_db(db),
    m_itemID(itemID),
    m_tableModel(tableModel),
    m_utilityLineEdit(nullptr)
{
    buildInterface();

    // When accepted or rejected, close the dialog.
    connect(this, &UtilityLineEditDialog::accepted, this, &UtilityLineEditDialog::hideAndDelete);
    connect(this, &UtilityLineEditDialog::rejected, this, &UtilityLineEditDialog::hideAndDelete);
}

UtilityLineEditDialog::~UtilityLineEditDialog()
{}

void UtilityLineEditDialog::buildInterface()
{
    // Building the interface with a line edit and a apply and cancel button.
    QVBoxLayout* mainLayout = new QVBoxLayout();

    m_utilityLineEdit = new UtilityLineEdit(m_tableName, m_utilityTable, m_db, this);
    setEditorData(); // Set the existing utility into the editor.
    mainLayout->addWidget(m_utilityLineEdit);

    // Seperator line between content and bottom buttons.
    QFrame* horizontalLine = new QFrame(this);
    horizontalLine->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(horizontalLine);

    // The cancel and apply buttons section.
    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
    QPushButton* applyButton = new QPushButton(tr("Apply"), this);

    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(applyButton);

    applyButton->setDefault(true); // Set the apply button when the user press enter.

    // The connection of the buttons.
    connect(cancelButton, &QPushButton::clicked, this, &UtilityLineEditDialog::reject);
    connect(applyButton, &QPushButton::clicked, this, &UtilityLineEditDialog::applyData);

    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
}

void UtilityLineEditDialog::hideAndDelete()
{
    // Hide and delete the the dialog.
    hide();
    emit deleteLater();
}

void UtilityLineEditDialog::applyData()
{
    // Apply utility to the specified data.
    if (m_itemID <= 0)
        return;

    QStringList utilityList = 
        m_utilityLineEdit->text().split(',', Qt::SkipEmptyParts);
    QList<long long int> utilityIDs;
    for (const QString& item : utilityList)
    {
        const QString trimmedUtility = item.trimmed();
        if (!trimmedUtility.isEmpty())
        {
            utilityIDs.append(
                m_utilityTable.addItem(m_tableName, trimmedUtility));
        }
    }
    m_tableModel->utilityInterface()->
        updateItemUtility(m_itemID, m_tableName, 
            QVariant::fromValue(utilityIDs));

    // Delete dialog.
    hideAndDelete();
}

void UtilityLineEditDialog::setEditorData()
{
    if (m_itemID <= 0)
    {
        return;
    }

    m_utilityLineEdit->setText(
        m_utilityTable.retrieveUtilityDataFromItem(
            m_itemID, 
            m_tableName,
            m_tableModel->utilityInterface()->tableName(m_tableName)));
}