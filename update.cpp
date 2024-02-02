#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iomanip>
using namespace std;

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

void createMap(string masterFIle, string transactionFile, map <unsigned int,int> bookMap);
void transaction(string transactionFile, map <unsigned int, int> bookMap);
void writeOut(string copyFile, map <unsigned int, int> bookMap);
void printUpdates();

int main(int argc, char* argv[]){

    //check number of files entered on command line. must be 3: master file, transaction file, updated file.
    if(argc < 3){
        cerr << "Incorrect number of files entered. Please input at least 3 files.";
        exit;
    }
    string masterFile = argv[1];
    string transactionFile = argv[2];
    string updatedFile = argv[3];

    map <unsigned int, int> bookMap;
    createMap(masterFile, transactionFile, bookMap);

    return 0;
}

// a method to read in the master binary file and add each record to a map sorted by isbn
void createMap(string masterFile, string transactionFile,  map <unsigned int,int> bookMap){

    // read binary master file and add to map
    BookRec book;
    fstream inMaster(masterFile, ios::in | ios :: binary);

    //if master file fails to open
    if(inMaster.fail()){
        cout<<endl;
        cout<<"### Cannot open file "<<masterFile<<endl;
        cout<<endl;
    }
    //if master file is empty, show error message
    inMaster.seekg(0, ios::end);
    if(inMaster.tellg() == 0){
        cerr << "Empty master file entered.";
    }
    inMaster.seekg(0,ios::beg);             //move to beginning of file after checking if empty

    // read each line and record starting byte offset (prevOffset) where isbn is stored
    int prevOffSet = 0;
    while(inMaster.read((char *) &book, sizeof(book))){
        bookMap[book.isbn] = prevOffSet;  //index where book record starts on each line
        prevOffSet = inMaster.tellg();    // move back to beginning of line
    }

    inMaster.clear();                   // clear file pointer, protects against dead pointer
    transaction(transactionFile, bookMap);
    inMaster.close();                   // close file pointer
}

// a method to read and perform transactions from the binary transaction file
void transaction(string transactionFile, map <unsigned int, int> bookMap){

    fstream inTransaction(transactionFile, ios::in | ios:: out | ios::binary);  //read IN transaction
    TransactionRec tRec;


    //if transaction file fails to open
    if(inTransaction.fail()){
        cout<<endl;
        cout<<"### Cannot open file "<<transactionFile<<endl;
        cout<<endl;
    }
    //if master file is empty, show error message
    inTransaction.seekg(0, ios::end);
    if(inTransaction.tellg() == 0){
        cerr << "Empty transaction file entered.";
    }
    inTransaction.seekg(0,ios::beg);             //move to beginning of file after checking if empty

    system("cp library.out copy.out");                                          // make a copy of master file to edit (doesn't modify original)
    fstream copyWrite("copy.out", ios::in | ios::out | ios::binary);            // write to and read from copy
    fstream errorWrite("ERRORS", ios::out);                                     // write transaction errors to error file

    int line = 1;
    while(inTransaction.read((char *) &tRec, sizeof(tRec))){        // line by line in transaction file
        
        BookRec copyRec;                                            // represents book in copy file

        if(tRec.ToDo == 0){                                         // add a record
            if(bookMap.find((tRec.B).isbn) == bookMap.end()){       // if record is NOT already in map
                //write new record to copy output file
                copyWrite.seekg(0, ios::end);                       //move copy file pointer to end                        
                bookMap[(tRec.B).isbn] = copyWrite.tellg();         //add to map
                copyWrite.write((char*) &(tRec.B), sizeof(tRec));   // append to END OF copy file
            }
            else{                                                   // record already in file
                cout << "Error in transaction number " << line << ": cannot add--duplicate key " << (tRec.B).isbn << endl;
                errorWrite.seekg(0, ios::end);
                errorWrite << "Error in transaction number " << line << ": cannot add--duplicate key " << (tRec.B).isbn << endl;
            }
        }
        else if(tRec.ToDo == 1){                                    // delete a record
            
            if(bookMap.find((tRec.B).isbn) != bookMap.end())        // if a record IS already in map, delete
                bookMap.erase((tRec.B).isbn);                       // remove the record from the map
            else{                                                   // record not in file
                cout << "Error in transaction number " << line << ": cannot delete--no such key " << (tRec.B).isbn << endl;
                errorWrite.seekg(0, ios::end);
                errorWrite << "Error in transaction number " << line << ": cannot delete--no such key " << (tRec.B).isbn << endl;
            }   
        }
        else if(tRec.ToDo == 2){                                    //change the on hand ammount

            if(bookMap.find((tRec.B).isbn) != bookMap.end()){       // check if isbn exists
                // get byte offset                                       
                // open copy and move to byte offset                    // Copy book = book in copy file
                //get book and get onhand                               // TRec book = same book with update in transaction file
                copyWrite.seekg(bookMap[(tRec.B).isbn]);                // move to byte offset where Tbook is in copy
                copyWrite.read((char*) &(copyRec), sizeof(copyRec));    // read the Cbook at byte offset (book whose count to change) in copy
                copyRec.onhand = copyRec.onhand + tRec.B.onhand;        // update onhand value by adding Tbook's onhand to Cbooks' onhand

                if(copyRec.onhand < 0){                                 // negative onhand, change to 0 and write out
                    cout << "Error in transaction number " << line << ": count = " << copyRec.onhand << " for key " <<  (tRec.B).isbn << endl;
                    errorWrite.seekg(0, ios::end);
                    errorWrite << "Error in transaction number " << line << ": count = " << copyRec.onhand << " for key " <<  (tRec.B).isbn << endl;
                    copyRec.onhand = 0;
                }
                copyWrite.seekg(bookMap[(tRec.B).isbn]);                 // move to byte offset and replace onhand value
                copyWrite.write((char*) &(copyRec), sizeof(copyRec));    // write out book with updated onhand 
            }
            else{                                                        // book not found
                cout << "Error in transaction number " << line << ": cannot change count--no such key " << (tRec.B).isbn << endl;
                errorWrite.seekg(0, ios::end);
                errorWrite << "Error in transaction number " << line << ": cannot change count--no such key " << (tRec.B).isbn << endl;
                
            }
        }
        else if(tRec.ToDo == 3){                                    // change the prince

            if(bookMap.find((tRec.B).isbn) != bookMap.end()){       // check if isbn exists

                copyWrite.seekg(bookMap[tRec.B.isbn]);              // move byte offset where book is
                copyWrite.read((char*) &copyRec, sizeof(copyRec));  // read book at byte offset
                copyRec.price = tRec.B.price;                       // change price
                copyWrite.seekg(bookMap[tRec.B.isbn]);              // move to byte offset
                copyWrite.write((char*) &copyRec, sizeof(copyRec)); // write out updated book price
            }
            else{                                                   // key not found
                cout << "Error in transaction number " << line << ": cannot change price--no such key " << (tRec.B).isbn << endl;
                errorWrite.seekg(0, ios::end);
                errorWrite << "Error in transaction number " << line << ": cannot change price--no such key " << (tRec.B).isbn << endl;
            }
        }
        line++;
    }
    // close pointers
    inTransaction.close();
    copyWrite.close();
    errorWrite.close();
    writeOut("copy.out", bookMap);
}

// a method to write out the sorted map to the updated file 
void writeOut(string copyFile, map <unsigned int, int> bookMap){

    BookRec book;
    fstream inCopy(copyFile, ios::in | ios:: binary);               // read in from master copy file
    fstream updateOut("update.out", ios::out | ios:: binary);       // write out to update binary file

    map<unsigned int, int>::iterator j;                             // iterate through map and add to update.out
    for(j = bookMap.begin(); j != bookMap.end(); ++j)
    {
        inCopy.clear();                               
        inCopy.seekg(j->second, ios::beg);
        inCopy.read((char *) &book, sizeof(book));
        updateOut.write((char *) &book, sizeof(book));
    }

    updateOut.close();        // close file pointers
    inCopy.close();
    system("rm copy.out");    //remove copy file
    printUpdates();
}

// print the updated binary file to screen in pretty format
void printUpdates(){

    BookRec book;
    // print from binary file
    fstream binfile ("update.out", ios::in | ios::binary);
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