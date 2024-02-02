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

void toBinary(string fileName);
void printRecord(BookRec book);

int main(int argc, char* argv[]){

    string fileName = "library.dat";

    toBinary(fileName);
    return 0;
}


void toBinary(string fileName){

    fstream infile(fileName, ios::in);                                // filestream object to read in from text file
    fstream outfile ("library.out", ios::out | ios::binary);    // filestream object to write out to BINARY file, named myLibrary.out to not overwrite given test file "library.out"

    if (infile.fail())
    {
        cout<<endl;
        cout<<"### Cannot open file "<<fileName<<endl;
        cout<<endl;
    }
    else
    {
        int lineNum = 1;        
        int prevISBN = 0;
        BookRec	book;
        string line, isbn, name, author, onhand, price, type;
        // read in text file line by line and convert to binary
        while (getline(infile, line, '|')) // only read isbn, check if negative and ignore if so
        {
            stringstream ss(line);
            getline(ss, isbn, '|');
            book.isbn = stoul(isbn);

            char ch;                                         // char object to catch '|' after the ints
            infile.getline(book.name, 25, '|');                  // read 25 chars (length of char array in struct)
            infile.getline(book.author, 25, '|');                // OR read until '|'
            infile >> book.onhand >> ch >> book.price >> ch;      // catch '|' with char spacers
            infile.getline(book.type, 25);                       // no delimiter at end

            // check to see if isbn is negative. if so, break and move to next line
            if(isbn[0] == '-'){      //ignore record
                cerr << "> Illegal ISBN number encourntered on line " << lineNum << " of data file - record ignored." << endl;
                lineNum++;
                continue;
            }
            // out of order error
            if(book.isbn <= prevISBN){     //print record, add to output file
                cerr << "> ISBN number out of sequence on line " << lineNum << " of data file." << endl;          
                outfile.write((char*) &book, sizeof(BookRec));   //write to outfile
                printRecord(book);
            }
            if (book.price < 0){      // print record, ignore in output file
                cerr << "> Negative price on line " << lineNum << " of data file - record ignored." << endl;
                printRecord(book);
            }
            if(book.onhand < 0){
                cerr << "> Negative amount onhand on line " << lineNum << " of data file - record ignored." << endl;
                printRecord(book);
            }
            else{   // if no errors, write to outfile
                outfile.write((char*) &book, sizeof(BookRec));   //write to outfile
            }
            prevISBN = book.isbn;
            lineNum++;

        }
        outfile.close();       //close file, free up space
        infile.close();

        // print from binary file
        fstream binfile ("library.out", ios::in | ios::binary);
        infile.seekg(0, ios::beg);
        cout << endl;
        while (binfile.read((char *) &book, sizeof(BookRec) ) )
        {
        cout<<setw(10)<<setfill('0')<<book.isbn
            <<setw(25)<<setfill(' ')<<book.name
            <<setw(25)<<book.author
            <<setw(3)<<book.onhand
            <<setw(6)<<fixed<<setprecision(2)<<book.price
            <<setw(10)<<book.type<<endl;
        }
        binfile.close();
    } 
}

void printRecord(BookRec book){
    cout<<setw(10)<<setfill('0')<<book.isbn
        <<setw(25)<<setfill(' ')<<book.name
        <<setw(25)<<book.author
        <<setw(3)<<book.onhand
        <<setw(6)<<fixed<<setprecision(2)<<book.price
        <<setw(10)<<book.type<<endl;

}