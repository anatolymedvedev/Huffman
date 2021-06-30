#include <iostream>
#include <fstream>
#include <cstddef>
#include <list>
#include <vector>
#include <map>

using namespace std;

class Node
{
public:
    char sym;           //символ
    int freq;           //частота появления символа
    Node *left, *right; //левый и правый потомок

    Node();             
    Node(char , int );
    Node (Node *, Node *);
    ~Node();
};

Node:: Node()
{
    sym = '\0';
    freq = 0;
    left = NULL;
    right = NULL;
}

Node::Node(char _sym, int _freq)
{
    sym = _sym;
    freq = _freq;
    left = NULL;
    right = NULL;
}

Node::Node(Node *_left, Node *_right)
{
    freq = _left->freq + _right->freq;    // сумма частот
    left = _left;
    right = _right;
}

Node:: ~Node()
{
    delete[] left;
    delete[] right;
}

bool comp(const Node *c1, const Node *c2)
{
    return c1->freq < c2->freq;
}

// леворекурсивный обход и построение кодов
vector<bool> code;
map<char, vector<bool>> table_code;
void treeGo(Node *head){
    if(head->left != nullptr)
    {
        code.push_back(0);
        treeGo(head->left);
    }
    if(head->right != nullptr)
    {
        code.push_back(1);
        treeGo(head->right);
    }
    if (head->left == nullptr && head->right == nullptr)
    {
        table_code[head->sym] = code;
    }
    if(!code.empty())
    {
        code.pop_back();
    }
}

int main()
{
    ifstream file("input.txt");
    if (!file)
    {
        cerr << "Crash!1" << endl;;
    }
    ofstream result("output.txt", ios::out | ios::binary);
    if(!result)
    {
        cerr << "Crash!2" << endl;
    }

    map<char, int> table;
    char s;
    //подсчитываем частоты символов
    while ((s = file.get()) && !file.eof())                     
    {															
		table[s]++;												
	}
    
    list<Node*> list;
    map<char, int>:: iterator iter;
    //создаем список, идем по таблице
    for(iter = table.begin(); iter !=table.end(); ++iter)       
    {
        Node *p = new Node(iter->first, iter->second);      //передаем сивол и его частоту
        list.push_back(p);
    }

    //строим дерево из нашего списка
    while(list.size() != 1) 
    {
        list.sort(comp);
        Node *first_node = list.front();
        list.pop_front();
        Node *second_node = list.front();
        list.pop_front();
        Node *new_node = new Node(first_node, second_node);
        list.push_back(new_node);
    }

    Node *root = list.front();  //корень
    treeGo(root);               //обход дерева леворекурсивный, строим коды и кладем их в мапу

    //сначала записываем значение по умолчанию (0), а потом когда узнаем кол-во значащих бит в последнем байте перезапишем
    int last_byte;
    result << last_byte;
    result << " ";
    //записываем  сколько элементов хранится в мапе, а потом сами символы и их частоты
    int count = table.size();
    result << count;
    for(iter = table.begin(); iter != table.end(); ++iter)
    {
        result << iter->first;
        result << iter->second;
    }

    file.clear();  
    file.seekg(0); //возвращает указатель в начало файла

    //записываем наш текст в файл
    char tx = 0;
    count = 0;                              //следит за упаковкой 8 бит(1 байта)
    while ((s = file.get()) && !file.eof())
    {
        vector <bool> x = table_code.at(s);
        for (size_t i = 0; i < x.size(); i++)
        {
            tx = tx | x[i] << (7 - count);
            count++;
            if (count==8)
            {
                count = 0;
                result << tx;
                tx = 0;
            }
        }
    }

    if (count)
    {
        result << tx;
    }

    //ставим указатель в начало файла и перезаписываем количество значащих бит в последний байт
    result.clear();
    result.seekp(0);
    last_byte = count;
    result << last_byte;

    file.close();
    result.close();
    return 0;
}
