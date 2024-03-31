#include<bits/stdc++.h>
using namespace std;

struct Node
{
    char data;
    unsigned freq;
    string code;
    Node *left, *right;

    Node()
    {
        left = right = NULL;
    }
};

struct compare{
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

class huffman
{
    public:
    string fileName;
    string filePath = "C:\\Users\\gella\\OneDrive\\Codes\\C++ Codes\\DS\\DSA Projects\\";
    string compressedFileName = "compressedfile.huf";
    string decompressedFileName = "decompressedfile.txt";
    vector<Node*> arr;
    Node *root;
    priority_queue<Node*,vector<Node*>,compare> pq;

    void createArr()
    {
        for(int i = 0; i < 128; i++)
        {
            arr.push_back(new Node());
            arr[i]->data = i;
            arr[i]->freq = 0;
        }
    }

    void createMinHeap()
    {
        char ch;
        ifstream file;
        file.open(filePath+fileName);

        while(file.get(ch))
        {
            arr[ch]->freq ++;
        }

        file.close();

        for(int i=0;i<128;i++)
        {
            if(arr[i]->freq != 0)
            {
                pq.push(arr[i]);
            }
        }
    }

    void createTree()
    {
        priority_queue<Node*,vector<Node*>,compare> tempPq(pq);
        while(tempPq.size() != 1)
        {
            Node *left = tempPq.top();
            tempPq.pop();
            Node *right = tempPq.top();
            tempPq.pop();

            root = new Node();
            root->freq = left->freq + right->freq;

            root->left = left;
            root->right = right;
            tempPq.push(root);
        }
    }

    void createCodes(Node *r, string str)
    {
        if(r->left == NULL && r->right == NULL)
        {
            r->code = str;
            return ;
        }

        createCodes(r->left, str + "0");
        createCodes(r->right, str + "1");
    }

    int binToDec(string str)
    {
        int res = 0;
        for(auto i: str)
        {
            res = res * 2 + i - '0';
        }
        return res;
    }

    void saveEncodedFile()
    {
        ifstream file;
        file.open(filePath + fileName , ios::in);
        fstream fileOutput;
        fileOutput.open(filePath + compressedFileName,ios::out | ios::binary);

        string in = "";
        string s = "";

        in += (char)pq.size();
        priority_queue<Node*,vector<Node*>,compare> tempPq(pq);
        while(!tempPq.empty())
        {
            Node *curr = tempPq.top();
            in += curr->data;

            // Padding the bits
            s.assign(127-curr->code.length(), '0');
            s += '1';
            s += curr->code;

            for (int i = 0; i < 128; i+=8)
            {
                string temp = s.substr(i, 8);
                in += (char)binToDec(temp);
            }
            tempPq.pop();
        }
        s.clear();

        //Saving the codes of every character appearing in the input file
        char ch;
        while(file.get(ch))
        {
            s += arr[ch]->code;

            while(s.length() > 8)
            {
                in += (char)binToDec(s.substr(0, 8));
                s = s.substr(8);
            }
        }

        int count = 8 - s.length();
        if(s.length() < 8)
        {
            s.append(count, '0');
        }
        in += (char)binToDec(s);

        in += (char)count;

        fileOutput.write(in.c_str(),in.size());
        file.close();
        fileOutput.close();
    }

    string decToBin(int num)
    {
        string temp = "", res = "";
        while(num > 0)
        {
            temp += (num % 2 + '0');
            num /= 2;
        }
        res.append(8 - temp.length(),'0');
        for(int i=temp.length()-1; i>= 0; i--)
        {
            res += temp[i];
        }

        return res;
    }

    void buildTree(char a_code, string& path)
    {
        Node *curr = root;
        for(int i=0;i<path.length();i++)
        {
            if(path[i] == '0')
            {
                if(curr->left == NULL)
                {
                    curr->left = new Node();
                }
                curr = curr->left;
            }
            else if(path[i] == '1')
            {
                if(curr->right == NULL)
                {
                    curr->right = new Node();
                }
                curr = curr->right;
            }
        }
        curr->data = a_code;
    }

    void getTree()
    {
        fstream file;
        file.open(filePath+compressedFileName, ios::in | ios::binary);
        unsigned char size;
        file.read(reinterpret_cast<char*>(&size), 1);

        root = new Node();

        for(int i=0;i<size;i++)
        {
            char aCode;
            unsigned char hCodec[16];
            file.read(&aCode,1);
            file.read(reinterpret_cast<char*>(hCodec),16);

            string hCodeStr = "";
            for(int i=0;i<16;i++)
            {
                hCodeStr += decToBin(hCodec[i]);
            }

            int j = 0;
            while (hCodeStr[j] == '0')
            {
                j ++;
            }
            hCodeStr = hCodeStr.substr(j+1);
            buildTree(aCode,hCodeStr);
        }
        file.close();
    }

    void saveDecodedFile()
    {
        fstream file;
        file.open(filePath+compressedFileName,ios::in | ios::binary);
        fstream ofile;
        ofile.open(filePath+decompressedFileName,ios::out);

        unsigned char size;
        file.read(reinterpret_cast<char*>(&size), 1);

        file.seekg(-1,ios::end);
        char count0;
        file.read(&count0, 1);
        file.seekg(1 + 17 * size, ios::beg);

        vector<unsigned char> text;
        unsigned char textseg;
        file.read(reinterpret_cast<char*>(&textseg), 1);

        while(!file.eof())
        {
            text.push_back(textseg);
            file.read(reinterpret_cast<char*>(&textseg), 1);
        }

        Node *curr = root;
        string path;
        for(int i=0;i<text.size()-1;i++)
        {
            path = decToBin(text[i]);
            if(i == text.size() - 2)
            {
                path = path.substr(0, 8 - count0);
            }

            for(int j=0;j<path.size();j++)
            {
                if(path[j] == '0')
                    curr = curr->left;
                else
                    curr = curr->right;

                if(curr->left == NULL && curr->right == NULL)
                {
                    ofile.put(curr->data);
                    curr = root;
                }
            }
        }
        file.close();
        ofile.close();
    }

    void compress()
    {
        cout<<"Enter the file you want to compress: ";
        cin>>fileName;
        cout<<"The compressed file is stored in compressedfile.huf\n";
        createArr();
        createMinHeap();
        createTree();
        createCodes(root,"");
        saveEncodedFile();
    }

    void decompress()
    {
        cout<<"Your decompressed file is stored in decompressedfile.txt\n";
        getTree();
        saveDecodedFile();
    }

};


int main()
{      
    huffman huff;
    huff.compress();

    cout<<"Do you want to decompress and check ? if yes press y or n: ";
    char ch;
    cin>>ch;
    if(ch == 'y')
        huff.decompress();

}


