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
    ifstream file("output.txt", ios::out | ios::binary);
    if (!file)
    {
        cerr << "Crash!1" << '\n';
    }
	ofstream result("result.txt");
    if(!result)
    {
        cerr << "Crash!2" << '\n';
    }
    
    int last_byte;
    file >> last_byte;  //считываем кол-во значащих битов последнего байта

    int count;
    file >> count;      // считываем размер мапы

    char s;
    int sym_freq;
    map<char, int > table;
    // заполняем мапу
    while (count > 0)
    {
		s = file.get();
		file >> sym_freq;
        --count;
        table[s] = sym_freq;
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

    //записываем наш текст в раскодированный файл
    char tx;
    Node *head = root;
    tx = file.get();
    count = 0;
    while (!file.eof())
    {
		bool bit = tx & (1 << (7 - count));
		if (bit) 
        {
            head = head->right;
        }
		else 
        {
            head = head->left;
        }
		if (head->left == nullptr && head->right == nullptr)
        {
			result << head->sym;
			head = root;
		}
		count++;
        
		if (count == 8)
        {
			count = 0;
			tx = file.get();
		}
        if (file.peek() == EOF)
        {
            while(last_byte != count)
            {
                bit = tx & (1 << (7 - count));
                if (bit)
                {
                    head = head->right;
                }
                else
                {
                    head = head->left;
                }
                if(head->right == nullptr && head->left == nullptr)
                {
                    result <<head->sym;
                    head = root;
                }
                count++;
            }
            break;
        }
	}

    file.close();
    result.close();
    return 0;
}
