/**
 * @file        database_basic.h
 * @author      vacajk (wang_li_123@qq.com)
 * @brief       database with widget
 * @version     0.1
 * @date        2019-09-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _DATABASE_BASIC_H_
#define _DATABASE_BASIC_H_

#include "database_samples_db.h"

#include <QWidget>

typedef uint8_t u8;
typedef uint32_t u32;

namespace Ui {
class database_basic;
}

class database_basic : public QWidget
{
    Q_OBJECT

public:
    explicit database_basic(QString table_name, QList<QString> column_hidden_list=QList<QString>(), QWidget *parent = 0);
    ~database_basic();

    database_samples_db *Database_Db = nullptr;

    typedef enum
    {
        TABLE_POSITION_TOP_LEFT,
        TABLE_POSITION_TOP_RIGHT,
        TABLE_POSITION_BOTTOM_LEFT,
        TABLE_POSITION_BOTTOM_RIGHT,
        TABLE_POSITION_CUSTOM,
        TABLE_POSITION_COUNT,
    } TABLE_POSITION_e;

    void database_open(QString database_name);

    void table_set_postion(TABLE_POSITION_e pos, int row=0, int col=0, QItemSelectionModel::SelectionFlag sel_flag=QItemSelectionModel::Select);
    void table_select();

signals:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void databaseOpened();

public slots:
    void on_db_open_clicked();
    void on_db_submit_clicked();
    void on_db_revert_clicked();

    void on_db_rec_delete_clicked();
    void on_db_rec_insert_clicked();
    void on_db_rec_append_clicked();

    void on_column_sort_name_currentIndexChanged(QString index);
    void on_column_sort_ascend_clicked();
    void on_column_sort_descend_clicked();

private slots:
    void slot_currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void slot_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::database_basic *ui;

    QString database_file_name;
    QString database_table_name;
    QList<QString> table_column_hidden_list;
};

#endif // _DATABASE_BASIC_H_
