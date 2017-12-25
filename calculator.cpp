#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <memory.h>
#include <stdlib.h>

using namespace std;
typedef struct RowCode *Row;
typedef struct errorMessage *Error;
typedef struct MoveIn *Move;

//保存错误信息
struct errorMessage
{
    string data;
    Error next;
};

//保存一行代码
struct RowCode
{
    string data;
    Row next;
};

//判断左右符号是否匹配
struct Brackets
{
    int top;
};

//zicu栈
struct MoveIn
{
    string data;
    char symbol;
    Move next;
};

Brackets bk;
Error Head;
int temporary = 1;

Row initRow()
{
    Row head;
    head = new RowCode;
    head->next = NULL;
    return head;
}
Move initMoveIn()
{
    Move head = new MoveIn;
    head->next = NULL;
    return head;
}
void initError()
{
    Head = new errorMessage;
    Head->next = NULL;
}
//类型转换，int to string
string intTostr(float num)
{
    stringstream ss;
    string str;
    ss << num;
    ss >> str;
    return str;
}

//类型转换，string to int
float strToint(string data)
{
    stringstream ss;

    float num;
    ss << data;
    ss >> num;
    return num;
}
//插入错误信息
void ErrorInsert(string data)
{
    Error pre = Head;
    while (pre->next != NULL)
        pre = pre->next;

    Error err = new errorMessage;
    err->data = data;
    err->next = pre->next;
    pre->next = err;
}
//移进一个操作数
Move MoveInDate(string data, char Operator, Move Head)
{
    Move pre = Head;

    Move move = new MoveIn;
    move->data = data;
    move->symbol = data != ")" && data != "(" ? 'Y' : ' ';
    move->next = pre->next;
    pre->next = move;

    if (Operator != '#' && Operator != 'K')
    {
        Move o = new MoveIn;
        o->data = Operator;
        o->next = pre->next;
        pre->next = o;
    }

    if (Operator == '+' || Operator == '-' || Operator == '#' || Operator == ')')
    {
        move->symbol = 'X';
    }
    return pre;
}
//规约
void BracketsProtocol(Move move)
{
    Move value = NULL;
    Move left = NULL;
    while (move != NULL)
    {
        if (move->data == ")")
        {
            value = move->next;
            left = value->next;
            if (value->symbol == 'X' && left->data == "(")
            {
                move->data = value->data;
                move->symbol = 'X';
                move->next = left->next;
                delete value;
                delete left;
            }
        }
        move = move->next;
    }
}
void protocol(Move move, char Operator, Move pre)
{
    if (Operator != '+' && Operator != '-' && Operator != '/' && Operator != '*' && Operator != '#' && Operator != ')')
        return;

    Move right = NULL;
    Move opera = NULL;
    Move left = NULL;

    if (Operator == '#')
    {
        right = move;
        move = pre;
    }
    else
        right = move->next;

    if (right->next == NULL)
        return;
    else
        opera = right->next;

    if (opera->next == NULL)
        return;
    else
        left = opera->next;

    if (opera->data == "+" || opera->data == "-")
    {
        if (right->symbol == 'X' && left->symbol == 'X')
        {
            cout << "(" << opera->data << "," << left->data << "," << right->data << ", t" << temporary
                 << ")" << endl;
            // left->data = opera->data == "+" ? intTostr(strToint(left->data) + strToint(right->data))
            //                                 : intTostr(strToint(left->data) - strToint(right->data));
            left->data = "t";
            left->data += intTostr(temporary);
            temporary++;
            move->next = left;
            BracketsProtocol(move);
            protocol(move, ')', move);
            delete opera;
            delete right;
        }
    }
    else if (opera->data == "/" || opera->data == "*" || Operator == ')')
    {
        if ((right->symbol == 'X' || right->symbol == 'Y') && (left->symbol == 'X' || left->symbol == 'Y'))
        {
            cout << "(" << opera->data << "," << left->data << "," << right->data << ", t" << temporary
                 << ")" << endl;
            // left->data = opera->data == "/" ? intTostr(strToint(left->data) / strToint(right->data))
            //                                 : intTostr(strToint(left->data) * strToint(right->data));
            left->data = "t";
            left->data += intTostr(temporary);
            temporary++;
            left->symbol = 'X';
            move->next = left;
            BracketsProtocol(move);
            protocol(move, ')', move);
            delete opera;
            delete right;
        }
    }
}

//添加一行代码
void RowInsert(Row &head, string data)
{
    Row pre = head;
    while (pre->next != NULL)
        pre = pre->next;

    Row row = new RowCode;
    row->data = data;
    row->next = pre->next;
    pre->next = row;
}

//判断是否输入非终结符
bool end(string data)
{
    int size = data.size();
    for (int i = 0; i < size; i++)
        if (data[i] == '#')
            return false;
    return true;
}
//添加错误信息到错误信息栈
void addError(int line, int i, char data)
{
    string str = "Line ";
    str += intTostr(line);
    str += ":";
    str += intTostr(i + 1);
    str += " ";
    str += data;
    str += " is undefined";

    ErrorInsert(str);
}
//取出一个操作数
int access(string data, int i, int size, int line, Move M)
{
    string num = "";
    bool pointFlag = true;
    bool zeroFlag = false;
    if (data[i] == '0')
    {
        num += data[i++];
        zeroFlag = true;
    }

    for (; i < size; i++)
    {
        if (data[i] == '+' || data[i] == '-' || data[i] == '/' || data[i] == '*' || data[i] == '#')
        {
            // cout << num << endl;
            if (data[i - 1] == '.')
                addError(line, i - 1, data[i - 1]);

            Move pre = MoveInDate(num, data[i], M);
            protocol(pre->next, data[i], pre);
            return i;
        }
        else if (data[i] == ')')
        {
            // cout << num << endl;
            if (data[i - 1] == '.')
                addError(line, i - 1, data[i - 1]);

            Move pre = MoveInDate(num, data[i], M);
            protocol(pre->next, data[i], pre);
            bk.top--;
            return i;
        }
        else if (data[i] >= '0' && data[i] <= '9' && !zeroFlag)
        {
            num += data[i];
        }
        else if (data[i] == '.')
        {
            if (pointFlag)
            {
                num += data[i];
                pointFlag = false;
            }
            else
            {
                addError(line, i, data[i]);
            }
            zeroFlag = false;
        }
        else
        {
            addError(line, i, data[i]);
        }
    }
    return i;
}
void dealWith(string data, int line, Move M)
{
    data += "#";
    int size = data.size();
    int cursor = 0;

    while (size != cursor)
    {
        if (data[cursor] == '#')
        {
            string errorstring = "Line ";
            errorstring += intTostr(line);
            if (bk.top > -1)
            {
                errorstring += " ( is not equipped";
                ErrorInsert(errorstring);
            }
            else if (bk.top < -1)
            {
                errorstring += " ) is not equipped";
                ErrorInsert(errorstring);
            }
            return;
        }

        else if (data[cursor] == ' ')
            cursor++;

        if (data[cursor] >= '0' && data[cursor] <= '9')
        {
            cursor = access(data, cursor, size, line, M) + 1;
            // cout << cursor << endl;
        }
        else if (data[cursor] == '(')
        {
            MoveInDate("(", 'K', M);
            bk.top++;
            cursor++;
        }
        else if (data[cursor] == ')')
        {
            MoveInDate(")", 'K', M);
            cursor++;
            bk.top--;
        }
        else if (data[cursor] == '+' || data[cursor] == '-' || data[cursor] == '/' || data[cursor] == '*')
        {
            string str = "";
            if (data[cursor - 1] == ')')
            {
                str += data[cursor];
                MoveInDate(str, 'K', M);
                cursor++;
            }
            else if (data[cursor] == '-')
            {
                str += data[cursor++];
                while (data[cursor] >= '0' && data[cursor] <= '9')
                {
                    str += data[cursor];
                    cursor++;
                }
                cout << "(@," << str << ", NULL, t" << temporary
                     << ")" << endl;

                str = "t";
                str += intTostr(temporary);
                temporary++;
                MoveInDate(str, '+', M);
                protocol(M->next, data[cursor], M);
                cursor++;
            }
        }
        else
        {
            addError(line, cursor, data[cursor]);
            cursor++;
        }
    }
}
int main()
{
    string str;
    int line = 1;
    bk.top = -1;
    initError();
    Row R = initRow();
    Move M = initMoveIn();
    do
    {
        cin >> str;
        RowInsert(R, str);

    } while (end(str));

    Row pre = R->next;

    while (pre != NULL)
    {
        dealWith(pre->data, line, M);
        pre = pre->next;
        line++;
    }

    Move move = M->next;
    protocol(move, '#', M);
    // while (move->data == ")")
    // {
    //     BracketsProtocol(move);
    //     protocol(move, '+', M->next);
    // }

    // move = M->next;
    // while (move != NULL)
    // {

    //     cout << endl
    //          << move->data;
    //     move = move->next;
    // }
    Error err = Head->next;
    while (err != NULL)
    {
        cout << endl
             << err->data;
        err = err->next;
    }
    return 0;
}