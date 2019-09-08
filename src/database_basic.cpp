/**
 * @file        database_basic.cpp
 * @author      vacajk (wang_li_123@qq.com)
 * @brief       database with widget
 * @version     0.1
 * @date        2019-09-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "database_basic.h"
#include "ui_database_basic.h"

#include <QFileDialog>
#include <QMessageBox>

database_basic::database_basic(QString table_name, QList<QString> column_hidden_list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::database_basic),
    database_table_name(table_name),
    table_column_hidden_list(column_hidden_list)
{
    ui->setupUi(this);

    if(nullptr != parent)
    {
        this->setGeometry(parent->geometry());
        ui->table_view->setGeometry(ui->table_view->x(), ui->table_view->y(),
                                    parent->geometry().width(), parent->geometry().height());
    }

    database_open(QString("%1.db3").arg(table_name));
}

database_basic::~database_basic()
{
    delete ui;
}

void database_basic::database_open(QString database_name)
{
    database_file_name = database_name;
    on_db_open_clicked();
}

void database_basic::on_db_open_clicked()
{
    QSqlError ret;

    //打开数据表
    if(database_file_name.isEmpty())
    {
        database_file_name = QFileDialog::getOpenFileName(this, "选择数据库文件","",
                                                                  "SQL Lite数据库(*.db *.db3)");
    }

    //选择SQL Lite数据库文件
    if (database_file_name.isEmpty())
    {
        qCritical() << "File is empty." << database_file_name;
        return;
    }

    //打开数据库
    Database_Db = new database_samples_db();

    database_samples_db::INIT_TYPE_e init_type;
    if(QFile::exists(database_file_name))
    {
        init_type = database_samples_db::INIT_TYPE_FILE;
    }
    else
    {
        init_type = database_samples_db::INIT_TYPE_NEW;
    }

    ret = Database_Db->init_database(init_type,
                                   database_file_name, database_table_name,
                                   ui->table_view, table_column_hidden_list);
    if(QSqlError::NoError != ret.type())
    {
        qCritical() << ret.text();
        qCritical() << "Init database failed.";
        return;
    }

    //打开数据表
    //Database_Db->SelModel当前项变化时触发currentChanged信号
    connect(Database_Db->SelModel, &QItemSelectionModel::currentChanged,
            this, &database_basic::slot_currentChanged);
    //选择行变化时
    connect(Database_Db->SelModel, &QItemSelectionModel::currentRowChanged,
            this, &database_basic::slot_currentRowChanged);

    //更新界面组件的使能状态
    ui->db_open->setEnabled(false);
    ui->db_rec_delete->setEnabled(false);
    ui->db_rec_insert->setEnabled(false);
    ui->db_rec_append->setEnabled(true);

    //更新排序列表
    ui->column_sort_name->addItems(Database_Db->get_table_headers());
    ui->column_sort_name->setEnabled(true);
    ui->column_sort_ascend->setEnabled(true);
    ui->column_sort_descend->setEnabled(true);

    //emit open signal
    emit databaseOpened();
}

void database_basic::on_db_submit_clicked()
{
    //保存修改
    bool res = Database_Db->TblModel->submitAll();

    if (!res)
        QMessageBox::information(this, "消息", "数据保存错误,错误信息\n"+Database_Db->TblModel->lastError().text(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
    else
    {
        ui->db_submit->setEnabled(false);
        ui->db_revert->setEnabled(false);
    }
}

void database_basic::on_db_revert_clicked()
{
    //取消修改
    Database_Db->TblModel->revertAll();

    ui->db_submit->setEnabled(false);
    ui->db_revert->setEnabled(false);
}

void database_basic::on_db_rec_delete_clicked()
{
    //删除当前行记录
    QModelIndex curIndex=Database_Db->SelModel->currentIndex(); //获取当前选择单元格的模型索引
    Database_Db->TblModel->removeRow(curIndex.row());
}

void database_basic::on_db_rec_insert_clicked()
{
    //插入记录
    QModelIndex curIndex=ui->table_view->currentIndex();

    Database_Db->TblModel->insertRow(curIndex.row(),QModelIndex());

    Database_Db->SelModel->clearSelection();    //清除已有选择
    Database_Db->SelModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);
}

void database_basic::on_db_rec_append_clicked()
{
    //添加记录
    Database_Db->TblModel->insertRow(Database_Db->TblModel->rowCount(),QModelIndex());          //在末尾添加一个记录

    QModelIndex curIndex=Database_Db->TblModel->index(Database_Db->TblModel->rowCount()-1,1);   //创建最后一行的ModelIndex
    Database_Db->SelModel->clearSelection();                                                    //清空选择项
    Database_Db->SelModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);               //设置刚插入的行为当前选择行

    int currow=curIndex.row();                                                                  //获得当前行
    Database_Db->TblModel->setData(Database_Db->TblModel->index(currow,1), 0xff);               //自动生成编号, DeviceID
}

void database_basic::on_column_sort_name_currentIndexChanged(QString index)
{
    qDebug() << "Table Model Sort With Column:" << index;

    //选择字段进行排序
    if(ui->column_sort_ascend->isChecked())
    {
        on_column_sort_ascend_clicked();
    }
    else if(ui->column_sort_descend->isChecked())
    {
        on_column_sort_descend_clicked();
    }
    else
    {
        qCritical() << "Table Model Sort Order ERROR.";
    }
}

void database_basic::on_column_sort_ascend_clicked()
{
    qDebug() << "Table Model Sort Using Ascend Order";

    //升序
    QString header = ui->column_sort_name->currentText();
    int header_index = Database_Db->get_table_header_index(header);

    Database_Db->TblModel->setSort(header_index, Qt::AscendingOrder);
    table_select();

    table_set_postion(TABLE_POSITION_CUSTOM, 0, header_index, QItemSelectionModel::Columns);
}

void database_basic::on_column_sort_descend_clicked()
{
    qDebug() << "Table Model Sort Using Descend Order";

    //降序
    QString header = ui->column_sort_name->currentText();
    int header_index = Database_Db->get_table_header_index(header);

    Database_Db->TblModel->setSort(header_index, Qt::DescendingOrder);
    table_select();

    table_set_postion(TABLE_POSITION_CUSTOM, 0, header_index, QItemSelectionModel::Columns);
}

void database_basic::slot_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    //有未保存修改时可用
    ui->db_submit->setEnabled(Database_Db->TblModel->isDirty());
    ui->db_revert->setEnabled(Database_Db->TblModel->isDirty());

    emit currentChanged(current, previous);
}

void database_basic::slot_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    //行切换时的状态控制
    ui->db_rec_delete->setEnabled(current.isValid());
    ui->db_rec_insert->setEnabled(current.isValid());

    emit currentRowChanged(current, previous);
}

void database_basic::table_set_postion(TABLE_POSITION_e pos, int row, int column, QItemSelectionModel::SelectionFlag sel_flag)
{
    QModelIndex curIndex;

    switch(pos)
    {
    case TABLE_POSITION_TOP_LEFT:
        row = 0;
        column = 0;
        break;
    case TABLE_POSITION_TOP_RIGHT:
        row = 0;
        column = Database_Db->TblModel->columnCount();
        break;
    case TABLE_POSITION_BOTTOM_LEFT:
        row = Database_Db->TblModel->rowCount();
        column = 0;
        break;
    case TABLE_POSITION_BOTTOM_RIGHT:
        row = Database_Db->TblModel->rowCount();
        column = Database_Db->TblModel->columnCount();
        break;
    case TABLE_POSITION_CUSTOM:
        break;
    default:
        qCritical() << "Not Implemented!";
        break;
    }

    curIndex = Database_Db->TblModel->index(row, column);
    Database_Db->SelModel->setCurrentIndex(curIndex, QItemSelectionModel::Select | sel_flag);
}

void database_basic::table_select()
{
//    Database_Db->TblModel->setTable(database_table_name);
    if (!Database_Db->TblModel->select())
    {
        qCritical() << "Table Model Select Failed.";
        qCritical() << Database_Db->TblModel->lastError().text();
    }

}
