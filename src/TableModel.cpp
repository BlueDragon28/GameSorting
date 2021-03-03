#include "TableModel.h"
#include <QSqlQuery>

TableModel::TableModel(QSqlDatabase& db, QObject* parent) :
    QSqlRelationalTableModel(parent),
    m_db(db),
    m_isTableCreated(false),
    m_isTableChanged(false)
{}

void TableModel::setTableName(const QString& tableName)
{
    if (m_isTableCreated)
    {
        QStringList tablesName = m_db.tables();
        for (QStringList::const_iterator it = tablesName.cbegin(); it != tablesName.cend(); it++)
        {
            if (tableName.compare(*it, Qt::CaseInsensitive) == 0)
                return;
        }
        QSqlQuery query;
        query.exec(QString("ALTER TABLE %1 RENAME TO %2;").arg(m_tableName, tableName));
        m_tableName = tableName;
        m_isTableChanged = true;
        updateTableContent();
    }
    else
    {
        QStringList tablesName = m_db.tables();
        for (QStringList::const_iterator it = tablesName.cbegin(); it != tablesName.cend(); it++)
        {
            if (tableName.compare(*it, Qt::CaseInsensitive) == 0)
                return;
        }
        m_tableName = tableName;
        QSqlQuery query;
        query.exec(QString(
            "CREATE TABLE %1 ( "
            "GameID INTEGER PRIMARY KEY, "
            "Name TEXT DEFAULT \"Default Game\", "
            "Type INTEGER DEFAULT \"Default Type\", "
            "Rate INTEGER DEFAULT 0);").arg(m_tableName));
        m_isTableCreated = true;
        m_isTableReady = true;
        m_isTableChanged = true;
        updateTableContent();
    }
}

void TableModel::updateTableContent()
{
    if (m_isTableReady && m_isTableChanged)
    {
        QSqlQuery query;
        QString selectStatement = QString("SELECT Name, Type, Rate FROM %1").arg(m_tableName);
        query.exec(selectStatement);
        setQuery(query);
        m_isTableChanged = false;
    }
}