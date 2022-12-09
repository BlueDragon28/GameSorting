#ifndef GAMESORTING_UTILITYLINEEDITDIALOG_H_
#define GAMESORTING_UTILITYLINEEDITDIALOG_H_

#include "DataStruct.h"
#include "SqlUtilityTable.h"
#include "UtilityLineEdit.h"
#include "TableModel.h"
#include <qdialog.h>
#include <qevent.h>
#include <qsqldatabase.h>

/*
A edit utility dialog embedding a line edit to edit utility.
This is made to workaround a bug on KDE display manager.
*/
class UtilityLineEditDialog : public QDialog
{
    Q_OBJECT

public:
    UtilityLineEditDialog(UtilityTableName tableName, long long int itemID, TableModel* tableModel, SqlUtilityTable& utilityTable, QSqlDatabase& db, QWidget* parent = nullptr);
    ~UtilityLineEditDialog();

private:
    void buildInterface();
    void hideAndDelete();
    void applyData();
    void setEditorData();

    UtilityTableName m_tableName;
    SqlUtilityTable& m_utilityTable;
    QSqlDatabase& m_db;
    long long int m_itemID;
    TableModel* m_tableModel;
    UtilityLineEdit* m_utilityLineEdit;
};

#endif // GAMESORTING_UTILITYLINEEDITDIALOG_H_