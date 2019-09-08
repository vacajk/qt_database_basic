/**
 * @file        database_samples_db.h
 * @author      vacajk (wang_li_123@qq.com)
 * @brief       database with SQL
 * @version     0.1
 * @date        2019-09-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _DATABASE_SAMPLES_DB_H
#define _DATABASE_SAMPLES_DB_H

#include <QWidget>
#include <QtSql>
#include <QTableView>
#include <QMutex>

typedef struct
{
    QString DeviceID;
    QString DeviceStatus;
    QString DeviceErrorCode;
    QString DeviceWorkMode;
} DATABASE_SAMPLE_DATA_t;

class database_samples_db : public QWidget
{
    Q_OBJECT

public:
    explicit database_samples_db(QWidget *parent = nullptr);
    ~database_samples_db();

    typedef enum
    {
        INIT_TYPE_FILE,
        INIT_TYPE_MEMORY,
        INIT_TYPE_NEW,
        INIT_TYPE_COUNT,
    } INIT_TYPE_e;

    QSqlDatabase         DB;                        //数据库连接
    QSqlTableModel      *TblModel   = nullptr;      //数据模型
    QItemSelectionModel *SelModel   = nullptr;      //选择模型
    QTableView          *TableView  = nullptr;

    QSqlError init_database(INIT_TYPE_e init_type,
                            QString database_name, QString table_name,
                            QTableView *table_view, QList<QString> column_hidden_list=QList<QString>());

    QSqlError sample_data_append(DATABASE_SAMPLE_DATA_t sample_data);
    QSqlError sample_data_update_device_id_device_status(DATABASE_SAMPLE_DATA_t sample_data);

    int get_table_column_width(int index);
    QStringList get_table_headers();
    int get_table_header_index(QString header);

private:
    QMutex db_mutex;
    QString Database_Name;
    QString Table_name;

    QSqlError create_custom_sample_table();
};

#endif // database_samples_db_H
