#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>


using namespace std;

class tNode			//узел дерева Хаффмана
{
public:
	char symbol;			//символ узла
	int num;				//количество вхождений
	tNode *left, *right;	//потомки

	tNode()
	{

	}

	tNode(int num, char symbol)
	{
		this->symbol = symbol;
		this->num = num;
	}

	tNode(tNode *left, tNode *right)
	{
		this->left = left;
		this->right = right;
		this->symbol = false;
		this->num = left->num + right->num;
	}

	tNode(tNode *old)
	{
		this->left = old->left;
		this->right = old->right;
		this->num = old->num;
		this->symbol = old->symbol;
	}

	void Print()
	{
		cout << this->num << " ";
		cout << this->symbol << endl;
	}

};

void map_alphabet(map<char, int> &alphabet, char* filename);
bool compare_nodes(tNode *first, tNode *second);
tNode* create_tree(map<char, int> &alphabet, list<tNode*> &tree_to_file);
void create_table(tNode *root, map<char, vector<bool>> &table, vector<bool> &cur_code);
void encode_file(map<char, vector<bool>> &table, tNode *root, list<tNode*> &tree_to_file);
void decode_file(char* filename);

int main()
{
	setlocale(LC_ALL, "Russian");

	int choice = 5;


	
		cout << endl << "Введите 1, чтобы заархивировать файл" << endl << "Введите 2, чтобы разархивировать файл" << endl;
		cin >> choice;

		switch (choice)
		{
		case 1:
		{
			cout << "Введите название файла: ";
			char *str = new char[128];
			scanf("%s", str);
			map<char, int> alphabet;		//количество вхождений каждого символа

			map_alphabet(alphabet, str);

			list<tNode*> tree_to_file;
			tNode *root = create_tree(alphabet, tree_to_file);			//создаем дерево Хаффмана и определяем его корень

			map<char, vector<bool>> table;					//таблица Хаффмана
			vector<bool> cur_code;

			create_table(root, table, cur_code);


			encode_file(table, root, tree_to_file);
			cout << "Файл заархивирован в encoded.bin" << endl;
			

		}
		break;

		case 2:
		{
			cout << "Введите название файла: ";
			char *str = new char[128];
			scanf("%s", str);
			decode_file(str);
			cout << "Файл разархивирован в decoded.txt" << endl;
		}
		break;
		}
	


	
	//--------------------------------------------------------------------------------------
	
	

	cout<<"\n";
	system("pause");
	return 0;
}

void map_alphabet(map<char, int> &alphabet, char *filename)
{
	setlocale(LC_ALL, "Russian");


	string fileName = filename;
	ifstream in(fileName);
	string line;


	if (in.is_open())
	{
		while (getline(in, line))
		{
			line += '\n';
			for (int i = 0; i < line.length(); i++)
			{
				/*
				pair< map< char, int >::iterator, bool > ret;
				ret = init_alphabet.insert(pair< char, int >(line[i], 1));
				if (!ret.second) ret.first->second++;
				*/
				alphabet[line[i]]++;
			}
			//cout << line;
		}

	}
	else
	{
		cout << "Failed to open file ";
		system("pause");
	}
	in.close();
	/*
	for (pair<char, int> item : alphabet)
	{
		//cout << item.first << " : " << item.second << endl; //Вывод ключей и значений
		alphabet.emplace(item.second, item.first);
	}
	*/
}
bool compare_nodes(tNode *first, tNode *second)
{
	return first->num < second->num;
}
tNode* create_tree(map<char, int> &alphabet, list<tNode*> &tree_to_file)
{
	setlocale(LC_ALL, "Russian");
	list<tNode*> tree;						//дерево хаффмана

	for (pair<char, int> item : alphabet)						//создаем много маленьких деревьев
	{
		tNode *new_node = new tNode(item.second, item.first);
		tree.push_back(new_node);
		tree_to_file.push_back(new_node);
	}

	while (tree.size() != 1)										//объединяем их в одно
	{
		tree.sort(compare_nodes);

		tNode *left = tree.front();
		tree.pop_front();
		tNode *right = tree.front();
		tree.pop_front();

		tNode *parent_node = new tNode(left, right);
		tree.push_back(parent_node);
	}
	return tree.front();
}
void create_table(tNode * root, map<char, vector<bool>> &table, vector<bool> &cur_code)
{
	if (root->left)//если есть левый потомок
	{
		cur_code.push_back(0);
		create_table(root->left, table, cur_code);
	}

	if (root->right)//если есть правый потомок
	{
		cur_code.push_back(1);
		create_table(root->right, table, cur_code);
	}

	if (root->symbol != 0)			//если дошли до листа, то записываем текущий код в таблицу
	{
		table[root->symbol] = cur_code;
	}
	if (cur_code.size() != 0) cur_code.pop_back();	//если идем вверх по дереву, то на родительских узлах убираем последнее число из текущего кода
}
void encode_file(map<char, vector<bool>> &table, tNode *root, list<tNode*> &tree_to_file)
{
	string fileName = "iamafile.txt";
	ifstream in(fileName);
	string line;

	int count = 0;

	if (in.is_open())
	{
		FILE *f;
		f = fopen("encoded.bin", "wb");//файл для записи

		short size = tree_to_file.size();
		fwrite(&size, 2, 1, f);
		for (int i = 0; i < size; i++)//записываем дерево хаффмана в закодируемый файл
		{
			fwrite(tree_to_file.front(), sizeof(tNode), 1, f);
			tree_to_file.pop_front();
		}

		bool *buf = new bool[8];
		for (int i = 0; i < 8; i++) buf[i] = 0;
		int count = 0;

		while (getline(in, line))
		{
			line += '\n';
			for (int i = 0; i < line.length(); i++)
			{
				//cout << table[line[i]].size() << endl;
				for (int j = 0; j < table[line[i]].size(); j++)
				{
					buf[count] = table[line[i]][j];
					count++;
					//code.push_back(table[line[i]][j]);
					if (count == 8)
					{
						unsigned char c = 0;
						for (int t = 0; t < 8; t++)
						{
							if (buf[t])
							{
								c = c | (1 << t);
							}
						}
						fwrite(&c, 1, 1, f);

						count = 0;
					}


				}

			}
			//cout << line;
		}
		if (count != 0)//случай, когда не добралось до числа кратного 8
		{
			for (int j = count; j < 8; j++)
			{
				buf[j] = 0;
			}
			unsigned char c = 0;
			for (int t = 0; t < 8; t++)
			{
				if (buf[t])
				{
					c = c | (1 << t);
				}
			}
			fwrite(&c, 1, 1, f);
		}
		fclose(f);

	}
	else
	{
		cout << "Failed to open file ";
		system("pause");
	}
	in.close();
}
void decode_file(char* filename)
{
	setlocale(LC_ALL, "Russian");
	list<tNode*> huff_tree;
	tNode *cur_node = new tNode(1, 'a');

	FILE *fp, *fout;
	fp = fopen(filename, "rb");
	fout = fopen("decoded.txt", "w");

	short size;
	fread(&size, 2, 1, fp);
	for (int i = 0; i < size; i++)
	{
		fread(cur_node, sizeof(tNode), 1, fp);
		tNode *new_node = new tNode(cur_node->num, cur_node->symbol);
		huff_tree.push_back(new_node);
	}

	while (huff_tree.size() != 1)										//объединяем их в одно
	{
		huff_tree.sort(compare_nodes);

		tNode *left = huff_tree.front();
		huff_tree.pop_front();
		tNode *right = huff_tree.front();
		huff_tree.pop_front();

		tNode *parent_node = new tNode(left, right);
		huff_tree.push_back(parent_node);
	}
	tNode *new_root = huff_tree.front();
	cur_node = new_root;

	long lSize;
	fseek(fp, 0L, SEEK_END);
	lSize = ftell(fp) - 2 - size * sizeof(tNode);
	rewind(fp);
	fseek(fp, 2 + size * sizeof(tNode), 0);
	char *buffer = new char[lSize + 1];

	if (1 != fread(buffer, lSize, 1, fp)) fclose(fp), free(buffer), fputs("entire read fails", stderr);


	for (int s = 0; s < lSize - 1; s++)
	{
		//cout << buffer[i];
		bool *mas1 = new bool[8];
		for (int i = 0; i < 8; i++)
		{
			mas1[i] = 0;
		}

		for (int i = 0; i < 8; i++)
		{
			if ((buffer[s] & (1 << i)) != 0)
			{
				mas1[i] = true;
			}

			if (cur_node->symbol == 0)
			{
				if (mas1[i] == true)
				{
					cur_node = cur_node->right;// new tNode(cur_node->left);
				}
				else
				{
					cur_node = cur_node->left;//new tNode(cur_node->right);
				}
			}
			else
			{
				//cout << cur_node->symbol;
				fwrite(&cur_node->symbol, 1, 1, fout);
				cur_node = new_root;
				if (mas1[i] == true)
				{
					cur_node = cur_node->right;// new tNode(cur_node->left);
				}
				else
				{
					cur_node = cur_node->left;//new tNode(cur_node->right);
				}
			}
		}
	}
	fclose(fp);
	fclose(fout);
}