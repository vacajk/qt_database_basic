/**
 * @file        database_samples_db.cpp
 * @author      vacajk (wang_li_123@qq.com)
 * @brief       database with SQL
 * @version     0.1
 * @date        2019-09-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "database_samples_db.h"

#include <QMessageBox>

typedef struct
{
    int     Column_Index;
    int     Column_Width;
    QString DB_Header;
    QString Tbl_Header;
} TABLE_VIEW_HEADER_t;

static TABLE_VIEW_HEADER_t s_TblView_Header[] =
{
    {  0, 80,   QString("SampleNo"),            QString("编号")},
    {  1, 80,   QString("DeviceID"),            QString("设备ID")},
    {  2, 80,   QString("DeviceStatus"),        QString("状态")},
    {  3, 80,   QString("DeviceErrorCode"),     QString("错误码")},
    {  4, 80,   QString("DeviceWorkMode"),      QString("工作模式")},
};

#define STATUS_TABLE_VIEW_HEADER_COLUMN_NUM (sizeof(s_TblView_Header)/sizeof(TABLE_VIEW_HEADER_t))

database_samples_db::database_samples_db(QWidget *parent) : QWidget(parent)
{
}

database_samples_db::~database_samples_db()
{

}

int database_samples_db::get_table_column_width(int index)
{
    int width = 40;

    QString table_header = TblModel->headerData(index, Qt::Horizontal).toString();
    for (int j=0; j<(int)STATUS_TABLE_VIEW_HEADER_COLUMN_NUM; j++)
    {
        if(table_header == s_TblView_Header[j].Tbl_Header)
        {
            width = s_TblView_Header[j].Column_Width;
            break;
        }
    }
    return width;
}

QStringList database_samples_db::get_table_headers()
{
    QStringList tbl_header_list;

    for(int i=0; i<TblModel->columnCount(); i++)
    {
        QString table_header = TblModel->headerData(i, Qt::Horizontal).toString();
        for (int j=0; j<(int)STATUS_TABLE_VIEW_HEADER_COLUMN_NUM; j++)
        {
            if(table_header == s_TblView_Header[j].Tbl_Header)
            {
                if(!TableView->isColumnHidden(TblModel->fieldIndex(s_TblView_Header[j].DB_Header)))
                {
                    tbl_header_list.append(s_TblView_Header[j].Tbl_Header);
                }
            }
        }
    }

    return tbl_header_list;
}

int database_samples_db::get_table_header_index(QString header)
{
    for(int i=0; i<TblModel->columnCount(); i++)
    {
        QString table_header = TblModel->headerData(i, Qt::Horizontal).toString();
        if(table_header == header)
        {
            return i;
        }
    }
    return -1;
}

QSqlError database_samples_db::create_custom_sample_table()
{
    QSqlQuery q(DB);
    if (!q.exec(QLatin1String("create table sampledata("
                              "SampleNo integer primary key autoincrement unique,"
                              "DeviceID integer not null,"
                              "DeviceStatus integer,"
                              "DeviceErrorCode integer,"
                              "DeviceWorkMode integer"
                              ")")))
        return q.lastError();

    return QSqlError();
}

QSqlError database_samples_db::init_database(INIT_TYPE_e init_type,
                                           QString database_name, QString table_name,
                                           QTableView *table_view, QList<QString> column_hidden_list)
{
    QSqlError ret;
    TableView = table_view;
    Database_Name = database_name;
    Table_name = table_name;

    //we must set connection name if we want to one more database at the same time
    QString connection_name = database_name + table_name;

    //open database
    DB = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    switch(init_type)
    {
    case INIT_TYPE_FILE:
        qDebug() << "Open and init a exist database:" << database_name;
        DB.setDatabaseName(database_name);
        if (!DB.open())
        {
            QMessageBox::warning(this, "错误", "打开数据库失败",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return DB.lastError();
        }
        break;

    case INIT_TYPE_MEMORY:
        qDebug() << "Create and init a memory database";
        DB.setDatabaseName(":memory:");
        if (!DB.open())
        {
            QMessageBox::warning(this, "错误", "打开数据库失败",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return DB.lastError();
        }
        else if(table_name == "sampledata")
        {
            ret = create_custom_sample_table();
            if(QSqlError::NoError != ret.type())
            {
                qCritical() << ret.text();
            }
        }
        else
            return QSqlError();
        break;

    case INIT_TYPE_NEW:
        qDebug() << "Open and init a new database:" << database_name;
        DB.setDatabaseName(database_name);
        if (!DB.open())
        {
            QMessageBox::warning(this, "错误", "打开数据库失败",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return DB.lastError();
        }
        if(table_name == "sampledata")
        {
            ret = create_custom_sample_table();
            if(QSqlError::NoError != ret.type())
            {
                qCritical() << ret.text();
            }
        }
        else
            return QSqlError();
        break;

    default:
        qCritical() << "Error init type!";
        break;
    }

    //open table
    TblModel=new QSqlTableModel(nullptr, DB);                                   //数据表
    TblModel->setTable(table_name);                                             //设置数据表
    TblModel->setEditStrategy(QSqlTableModel::OnManualSubmit);                  //数据保存方式，OnManualSubmit
    TblModel->setSort(TblModel->fieldIndex("SampleNo"), Qt::AscendingOrder);    //排序
    if (!TblModel->select())    //查询数据
    {
        QMessageBox::critical(this, "错误信息",
                              "打开数据表错误,错误信息\n"+TblModel->lastError().text(),
                              QMessageBox::Ok, QMessageBox::NoButton);
       return TblModel->lastError();
    }

    for (int i=0; i<(int)STATUS_TABLE_VIEW_HEADER_COLUMN_NUM; i++)
    {
        TblModel->setHeaderData(TblModel->fieldIndex(s_TblView_Header[i].DB_Header), Qt::Horizontal, s_TblView_Header[i].Tbl_Header);
    }

    SelModel = new QItemSelectionModel(TblModel);//关联选择模型

    TableView->setModel(TblModel);                                              //设置数据模型
    TableView->setSelectionModel(SelModel);                                     //设置选择模型

    foreach(QString column_hiden, column_hidden_list)
    {
        TableView->setColumnHidden(TblModel->fieldIndex(column_hiden), true);   //隐藏列
    }

    for(int i=0; i<TblModel->columnCount(); i++)
    {
        QString table_header = TblModel->headerData(i, Qt::Horizontal).toString();
        for (int j=0; j<(int)STATUS_TABLE_VIEW_HEADER_COLUMN_NUM; j++)
        {
            if(table_header == s_TblView_Header[j].Tbl_Header)
            {
                TableView->setColumnWidth(TblModel->fieldIndex(s_TblView_Header[j].DB_Header), s_TblView_Header[j].Column_Width);
            }
        }
    }

    return QSqlError();
}

QSqlError database_samples_db::sample_data_append(DATABASE_SAMPLE_DATA_t sample_data)
{
    QString query_title_str = QString("insert into %1").arg(Table_name);
    QString query_cmd_str = QString("("
                                    "DeviceID,"
                                    "DeviceStatus,"
                                    "DeviceErrorCode,"
                                    "DeviceWorkMode"
                                    ")"
                                    "values"
                                    "("
                                    "?, ?, ?, ?"
                                    ")");

    QSqlQuery q(DB);
    if (!q.prepare(query_title_str + query_cmd_str))
        return q.lastError();

    q.addBindValue(sample_data.DeviceID);
    q.addBindValue(sample_data.DeviceStatus);
    q.addBindValue(sample_data.DeviceErrorCode);
    q.addBindValue(sample_data.DeviceWorkMode);
    q.exec();

    return QSqlError();
}

QSqlError database_samples_db::sample_data_update_device_id_device_status(DATABASE_SAMPLE_DATA_t sample_data)
{
    QString query_title_str = QString("update %1 set ").arg(Table_name);
    QString query_cmd_str = QString("DeviceStatus = ? where DeviceID = ?");

    QSqlQuery q(DB);
    if (!q.prepare(query_title_str + query_cmd_str))
    {
        qDebug() << q.lastError().text();
        return q.lastError();
    }

    q.addBindValue(sample_data.DeviceStatus);
    q.addBindValue(sample_data.DeviceID);

    q.exec();
    return q.lastError();
}
