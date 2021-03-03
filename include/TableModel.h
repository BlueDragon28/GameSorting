#ifndef GAMESORTING_TABLEMODEL_H_
#define GAMESORTING_TABLEMODEL_H_

#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QString>

class TableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QSqlDatabase& db, QObject* parent = nullptr);

    // Setting the table name to use.
    void setTableName(const QString& tableName);
    void updateTableContent();

private:
    QSqlDatabase& m_db;
    bool m_isTableReady, m_isTableCreated, m_isTableChanged;
    QString m_tableName;
};

#endif // GAMESORTING_TABLEMODEL_H_