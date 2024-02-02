// Program 1: The Binary Book Database
// Jan 31st, 2022
// Seiler Rivers
using namespace std;
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>

typedef char String[25];
struct BookRec
{
    unsigned int isbn;
    String name;
    String author; 
    int onhand;
    float price;
    String type;
};
enum TransactionType {Add, Delete, ChangeOnhand, ChangePrice};
struct TransactionRec
{
TransactionType ToDo;
BookRec B;
};

void toBinary(string fileName);

int main(int argc, char* argv[]){

    string fileName = "TestTransaction.dat";

    toBinary(fileName);
    return 0;
}

void toBinary(string fileName){

    fstream infile(fileName, ios::in);                                // filestream object to read in from text file
    fstream outfile ("TestTransact.out", ios::out | ios::binary);    // filestream object to write out to BINARY file, named myLibrary.out to not overwrite given test file "library.out"

        int lineNum = 1;        
        int prevISBN = 0;
        TransactionRec tRec;
        string line, toDo, isbn, name, author, onhand, price, type;
        // read in text file line by line and convert to binary
        while (getline(infile, line, '|')) // read in transaction Type
        {
            stringstream ss(line);
            getline(ss, toDo, '|');
            int DO = stoi(toDo);
            tRec.ToDo = TransactionType(DO);

            char ch;                                         // char object to catch '|' after the ints
            infile >>  tRec.B.isbn >> ch;
            infile.getline(tRec.B.name, 25, '|');                  // read 25 chars (length of char array in struct)
            infile.getline(tRec.B.author, 25, '|');                // OR read until '|'
            infile >> tRec.B.onhand >> ch >> tRec.B.price >> ch;      // catch '|' with char spacers
            infile.getline(tRec.B.type, 25);                       // no delimiter at end

/*
            cout << "Transaction type: " << tRec.ToDo << endl;
            cout << "isbn: " << tRec.B.isbn << endl;
            cout << "name: " << tRec.B.name <<endl;
            cout << "author: " << tRec.B.author << endl;
            cout << "onhand: " << tRec.B.onhand << endl;
            cout << "type: " << tRec.B.type << endl;*/


            outfile.write((char*) &tRec, sizeof(TransactionRec));   //write to outfile

        }
        outfile.close();       //close file, free up space
        infile.close();

        // print from binary file
        fstream binfile ("TestTransact.out", ios::in | ios::binary);
        infile.seekg(0, ios::beg);
        cout << endl;
        while (binfile.read((char *) &tRec, sizeof(TransactionRec) ))
        {
        cout<<setw(10) << setfill(' ') << tRec.ToDo
            <<setw(10)<<setfill('0')<<tRec.B.isbn
            <<setw(25)<<setfill(' ')<<tRec.B.name
            <<setw(25)<<tRec.B.author
            <<setw(3)<<tRec.B.onhand
            <<setw(6)<<fixed<<setprecision(2)<<tRec.B.price
            <<setw(10)<<tRec.B.type<<endl;
        }
        binfile.close();
    } 
