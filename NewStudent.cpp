// 必须放在所有头文件最顶部！
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
// 提前手动定义 byte，解决 C++17 与 Windows 头文件的冲突
#define byte win_byte_override
#include <Winsock2.h>
#undef byte

#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <string>
#include <windows.h>

// 现在再引入 mysql.h，此时 SOCKET 类型已经被 Winsock2.h 提前定义好了
#include "mysql.h"

using namespace std;
// 剩下的原有代码保持不变

#include <windows.h>
#include <limits>   // 必须这个头文件
MYSQL* mysql;
const string host = "127.0.0.1";
const string user = "root";
const string pwd  = "root";    //改成你自己mysql密码
const string db   = "stu_db";

bool connectDB()
{
    mysql = mysql_init(nullptr);
    if (!mysql)
    {
        cout << "X 数据库初始化失败" << endl;
        return false;
    }
    // 重点！完整 mysql_real_connect 写法
    if (!mysql_real_connect(mysql, "127.0.0.1", "root", "root", "stu_db", 3306, nullptr, 0))
    {
        cout << "X 数据库连接失败：" << mysql_error(mysql) << endl;
        return false;
    }
    int charRet = mysql_set_character_set(mysql, "utf8mb4");

    if(charRet != 0)
    {
        cout << "字符集设置失败：" << mysql_error(mysql) << endl;
        mysql_close(mysql);
        return false;
    }
    cout << "✅ MySQL连接成功！数据库：stu_db" << endl;
    return true;
}


void menu()
{
    cout << "\n==== MySQL学生管理系统 ====" << endl;
    cout << "1 添加学生" << endl;
    cout << "2 查看全部学生" << endl;
     cout << "3 删除学生" << endl;
      cout << "4 修改学生" << endl;
    cout << "0 退出程序" << endl;
    cout << "请输入选择：";
}

void addStu()
{
    int id, age;
   char name[50];   // ✅ 改成char数组，不要string;
    cout << "输入学号：";
    cin >> id;
    cout << "输入姓名：";
   cin.ignore();   // 吃掉学号输入后的回车
   fgets(name, 50, stdin);
    cout << "输入年龄：";
    cin >> age;
char sql[256];


// ✅ 正确，用 << 拼接
cout << "✅ 添加成功id=" << id << endl;
cout << "✅ 添加成功name=" << name << endl;
cout << "✅ 添加成功age=" << age << endl;
    sprintf(sql, "insert into student(id,name,age) values(%d,'%s',%d)", id, name, age);
    int ret = mysql_query(mysql, sql);
    if (ret == 0)
    {
        cout << "✅ 添加成功" << endl;
    }
    else
    {
        cout << "❌ 添加失败：" << mysql_error(mysql) << endl;
    }
}

void showAll()
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    const char* sql = "select * from student";
    int ret = mysql_query(mysql, sql);
    if (ret != 0)
    {
        cout << "❌ 查询失败：" << mysql_error(mysql) << endl;
        return;
    }
    res = mysql_store_result(mysql);
    if (!res)
    {
        cout << "📭 没有学生数据" << endl;
        return;
    }
    cout << "\n学号\t姓名\t年龄" << endl;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        cout << row[0] << "\t" << row[1] << "\t" << row[2] << endl;
    }
    mysql_free_result(res);
}
 ///删除特定的人传id
void deleteStu()
{
    int id;   // ✅ 先定义id
    cout << "输入学号：";
    cin >> id;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // id是int数字，不加单引号，全部使用英文半角空格
    char sql[256];
    sprintf(sql, "DELETE FROM student WHERE id=%d", id);

    int ret = mysql_query(mysql, sql);
    if(ret != 0)
    {
        cout << "❌ 删除失败：" << mysql_error(mysql) << endl;
       // mysql_rollback(mysql);
    }
    else
    {
        // mysql_affected_rows 获取本次SQL影响行数
        if(mysql_affected_rows(mysql) == 0)
        {
            cout << "⚠️ 该学号不存在，没有数据被删除！" << endl;
        }
        else
        {
            mysql_commit(mysql); // 事务提交
            cout << "✅ 删除成功，事务已提交" << endl;
        }
    }
     }
///  更改学生编号
 void updateStudent(){

int id, age;
    char name[50];

    cout << "输入要修改的学号：";
    cin >> id;
    cout << "输入新姓名：";
    cin >> name;
    cout << "输入新年龄：";
    cin >> age;

    char sql[256];
    // 查询
    snprintf(sql, sizeof(sql), "select * from student where id = %d", id);
    int ret = mysql_query(mysql, sql);
    if (ret != 0)
    {
        cout << "❌ 查询失败：" << mysql_error(mysql) << endl;
        return;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    if (mysql_num_rows(res) == 0)
    {
        cout << "❌ 学号不存在，不能修改！" << endl;
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);

    // 更新
    snprintf(sql, sizeof(sql), "update student set name='%s',age=%d where id=%d", name, age, id);
    ret = mysql_query(mysql, sql);
    if (ret == 0)
    {
        cout << "✅ 修改成功" << endl;
    }
    else
    {
        cout << "❌ 修改失败：" << mysql_error(mysql) << endl;
    }





}

// 处理cin输入错误的工具函数
void clearCinError()
{
    // 重置错误状态位
    cin.clear();
    // 清空缓冲区，丢弃所有字符直到换行符
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
int main()
{
    cout << "==== 程序启动 ====" << endl;
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
cout << "编码设置完成" << endl;
    if (!connectDB())
    {
        system("pause"); // 加这一行！失败就停住，看文字
        return -1;
    }
    int choice;
    while (true)
    {
        menu();
        cin >> choice;
        if (!cin)
    {
        cout << "❌ 请输入数字！" << endl;
        clearCinError();
        continue;
    }
        switch (choice)
        {
            case 1:
                addStu();
                break;
            case 2:
                showAll();
                break;
            case 3:
            //删除特定
            deleteStu();
             break;
            case 4:
           updateStudent();
            break;
        
            case 0:
                mysql_close(mysql);
                cout << "程序退出" << endl;
                system("pause");
                return 0;
            default:
                cout << "输入错误，请重新选择！" << endl;
        }
    }
}
