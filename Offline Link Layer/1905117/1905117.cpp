#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <bitset>
#include <bits/stdc++.h>
#include <windows.h>

using namespace std;

int totalbits;
vector<vector<int>>datablockWithcheckbits;
vector<int> index;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
int r = 1;

string errorCorrection2(string receivedBits)
{
    int errorBit = 0;
    int n = receivedBits.length();
    // Calculate the parity bits
    for (int i = 0; i < r; i++)
    {
        int parity = 0;
        int parityBitPosition = (1 << i) - 1;
        for (int j = parityBitPosition; j < n; j += (1 << (i + 1)))
        {
            for (int k = 0; k < (1 << i); k++)
            {
                int idx = j + k;
                if (idx < n)
                {
                    parity ^= (receivedBits[idx] - '0');
                }
            }
        }

        if (parity != 0)
        {
            errorBit += (1 << i);
        }
    }


    if (errorBit > 0 && errorBit<=n)
    {
        receivedBits[errorBit - 1] = (receivedBits[errorBit - 1] == '0') ? '1' : '0';
    }

    return receivedBits;
}
string padDataString(const string& dataString, int m)
{
    int paddingSize = m - (dataString.length() % m);
    if (paddingSize != m)
    {
        string padding(paddingSize, '~');
        return dataString + padding;
    }
    return dataString;
}


vector<string> createDataBlock(const string& paddedDataString, int m)
{
    vector<string> dataBlock;
    for (size_t i = 0; i < paddedDataString.length(); i += m)
    {
        string row = "";
        for (int j = 0; j < m; j++)
        {
            char c = paddedDataString[i + j];
            int asciiValue = static_cast<int>(c);
            row += bitset<8>(asciiValue).to_string();
        }
        dataBlock.push_back(row);
    }
    return dataBlock;
}

vector<int> generateHammingCode(
    string msgBits, int m, int r)
{
    vector<int> hammingCode(r + m);
    for (int i = 0; i < r; ++i)
    {

        hammingCode[pow(2, i) - 1] = -1;
    }

    int j = 0;
    for (int i = 0; i < (r + m); i++)
    {
        if (hammingCode[i] != -1)
        {
            hammingCode[i] = msgBits[j]-'0';
            j++;
        }
    }

    for (int i = 0; i < (r + m); i++)
    {
        if (hammingCode[i] != -1)
            continue;

        int x = log2(i + 1);
        int one_count = 0;
        for (int j = i + 2;
                j <= (r + m); ++j)
        {

            if (j & (1 << x))
            {
                if (hammingCode[j - 1] == 1)
                {
                    one_count++;
                }
            }
        }

        if (one_count % 2 == 0)
        {
            hammingCode[i] = 0;
        }
        else
        {
            hammingCode[i] = 1;
        }
    }

    return hammingCode;
}


void findHammingCode(string msgBit)
{
    int m = msgBit.size();

    while (pow(2, r) < (m + r + 1))
    {
        r++;
    }
    totalbits = m+r;

    int k=0;
    vector<int> ans
        = generateHammingCode(msgBit, m, r);
    datablockWithcheckbits.push_back(ans);
    k++;

    for (int i = 0; i < ans.size(); i++)
    {
        if (((i + 1) & (i)) == 0)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
            cout << ans[i];
        }
        else
        {
            SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            cout << ans[i] ;
        }

    }
    cout<<endl;

}

string calculateCRC(const string& serializedData, const string& generator)
{
    string dividend = serializedData;
    int generatorLength = generator.length();
    for (int i = 0; i < generatorLength - 1; i++)
    {
        dividend += '0';
    }
    for (int i = 0; i < serializedData.length(); i++)
    {
        if (dividend[i] == '1')
        {
            for (int j = 0; j < generatorLength; j++)
            {
                dividend[i + j] = (dividend[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }
    string checksum = dividend.substr(serializedData.length());
    cout<<serializedData;
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
    cout<<checksum<<endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return serializedData + checksum;

}


string simulateTransmission(const string& frame, double p)
{
    string receivedFrame = frame;
    srand(time(nullptr));

    for (int i = 0; i < frame.length(); i++)
    {
        double randomProbability = static_cast<double>(rand()) / RAND_MAX;

        if (randomProbability < p)
        {

            receivedFrame[i] = (frame[i] == '0') ? '1' : '0';
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
            cout<<receivedFrame[i];
            index.push_back(i);
        }
        else
        {
            SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            cout<<receivedFrame[i];
        }
    }
    cout<<endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return receivedFrame;
}

char binaryToAscii(string& binaryValue)
{

    bitset<8> binaryBits(binaryValue);
    char asciiChar = static_cast<char>(binaryBits.to_ulong());
    return asciiChar;
}


string errorDetectionOfCRC (const string& receivedData, const string& generator)
{
    string dividend = receivedData;
    int generatorLength = generator.length();
    for (int i = 0; i < receivedData.length() - generator.size() + 1; i++)
    {
        if (dividend[i] == '1')
        {
            for (int j = 0; j < generatorLength; j++)
            {
                dividend[i + j] = (dividend[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }
    string checksum = dividend.substr(receivedData.length()-generatorLength + 1);
    return  checksum;

}

int main()
{
    string dataString;
    int m;
    double p;
    string generator;
    int k=0;
    cout << "Enter data string: ";
    getline(cin, dataString);

    cout << "Enter m: ";
    cin >> m;

    cout << "Enter p: ";
    cin >> p;

    cin.ignore();

    cout << "Enter generator polynomial: ";
    getline(cin, generator);
    string paddedDataString = padDataString(dataString, m);
    cout<<endl;
    cout<<endl;
    cout << "data string after padding: " << paddedDataString << endl;
    cout<<endl;
    vector<string> dataBlock = createDataBlock(paddedDataString, m);
    cout<<"data block(ascii code of m characters per row : "<<endl;
    for(int i=0; i<dataBlock.size(); i++)
    {
        cout<<dataBlock[i]<<endl;
    }
    cout<<endl;
    cout<<"data bits after adding check bits : "<<endl;
    for(int i=0; i<dataBlock.size(); i++)
    {

        findHammingCode(dataBlock[i]);

    }
    string checksum;
    cout<<endl;
    cout<<"data bits after column-wise serialization : "<<endl;
    for(int i=0; i<totalbits; i++)
    {
        for(int j=0; j<datablockWithcheckbits.size(); j++)
        {
            cout<<datablockWithcheckbits[j][i];
            checksum+=to_string(datablockWithcheckbits[j][i]);

        }

    }
    cout<<endl;
    cout<<endl;

    cout << "data bits after appending CRC checksum (sent frame): " << endl;
    string framedData=calculateCRC(checksum, generator);
    cout<<endl;
    cout << "received frame: "<<endl;
    string receivedFrame = simulateTransmission(framedData, p);
    string errorDetection=errorDetectionOfCRC(receivedFrame, generator);
    cout<<endl;
    cout<<"result of CRC checksum matching : ";
    if( errorDetection.find('1') !=  string::npos)
    {
        cout<<"error detected"<<endl;
    }
    else
        cout<<"No error detected"<<endl;

    string removingChecksum = receivedFrame.substr(0,framedData.length()-generator.length()+1);
    int j=0;
    vector<string> str(dataBlock.size());
    vector<string> correctCodeword;
    vector<string> withoutParity;
    int indx=0;
    vector<string> booleanStr(dataBlock.size());

    int l = 0;
    for(int i=0, k= 0; i<removingChecksum.length(); i++)
    {
        if(k==dataBlock.size())
            k=0;

        str[k]+=removingChecksum[i];
        if(l < index.size() && index[l] == i)
        {
            l++;
            booleanStr[k] += "1";
        }
        else
            booleanStr[k] += "0";
        k++;

    }
    int n=0;
    cout<<endl;
    cout<<"data blocks after removing checksum bits : "<<endl;
    for(int i=0; i<str.size(); i++)
    {
        for(int j=0; j<str[i].size(); j++)
        {
            if(booleanStr[i][j] =='1')
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                cout<<str[i][j];
                n++;
            }
            else
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                cout<<str[i][j];
            }

        }
        cout<<endl;


    }
    for(int i=0; i<str.size(); i++)
    {
        string s=errorCorrection2(str[i]);
        correctCodeword.push_back(s);
    }

    vector<string> resultVector;

    for (const string& row : correctCodeword)
    {
        string modifiedRow = row;
        int i = 0;

        while (pow(2, i) - 1 < modifiedRow.length())
        {
            modifiedRow.erase(pow(2, i) - 1 - i, 1);
            i++;
        }

        resultVector.push_back(modifiedRow);
    }
    cout<<endl;
    cout<<"data blocks after removing check bits : "<<endl;
    for(int i=0; i<resultVector.size(); i++)
    {
        cout<<resultVector[i]<<endl;
    }
    cout<<endl;
    string result;
    for(int i=0; i<resultVector.size(); i++)
    {
        int jk=0;
        while(jk<m*8)
        {
            string substring = resultVector[i].substr(jk,8);
            result+=binaryToAscii(substring);
            jk+=8;
        }
    }
    cout<<"output frame : "<<result<<endl;

    return 0;
}

