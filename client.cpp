#include <iostream>
#include <vector>
#include <map>

#include <algorithm>

#include <.helib/helib.h>
#include <helib/matmul.h>


#include <fstream>
#include <sstream>

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

#define HOST "127.0.0.1"
#define PORT 50001

void reqData(int sock, string operation, vector<string> operands);
void sendFile(string proc,string fn,Context obj, int sock);


int main() {
    //need input variables
    //Create helib context
    helib::Context context = helib::ContextBuilder<helib::BGV>()
            .m(16384)
            .p(257)
            .r(1)
            .bits(250)
            .c(3)
            .build();

    cout << "Security: " << context.securityLevel() << endl;
    //Generate secret key and public key
    helib::SecKey secret_key(context)
    secret_key.GenSecKey();
    const helib::PubKey &public_key = secret_key;

    //Create json containing public key
    ofstream outPublicKeyFile;
    outPublicKeyFile.open("pk.json", ios::out);
    if (outPublicKeyFile.is_open()) {
        publicKey.writeToJSON(outPublicKeyFile);
    } else {
        throw runtime_error("Could not open file 'pk.json'.");
    }

    //Create Json containing context
    ofstream outContextFile;
    outContextFile.open("context.json", ios::out);
    if (outContextFile.is_open()) {
        context.writeToJSON(outContextFile);
    } else {
        throw runtime_error("Could not open file 'context.json'.");
    }

    //Create socket
    int sock = 0, valread, val;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error !" << endl;
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //Connect to local host
    if (inet_pton(AF_INET, HOST, &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address ! This IP Address is not supported !" << endl;
        return -1;
    }
    //Connect socket
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed : Can't establish a connection over this socket !" << endl;
        return -1;
    }

    //function below can set timeout, i believe its set to None by default
    //setsockopt(s,)
    //Create input list to store inputs
    vector<string> inp_list;
    bool cond = true;

    while (cond) {
        cout << "enter preferred operation" << endl;
        string inp;
        cin >> inp;
        if(inp == "HE"){
            //If input is HE then send sevrer json of public key
            sendFile("HE","context.json",context,sock);
            read(sock, buffer, 1024);
        }
        else if (inp == "db_num") {
            //If input is database number
            fstream fin;
            fin.open("toy_dataset.csv", ios::in);
            //reader = csv.DictReader(toy_f);
            while(true){
                cout<<"only numerical categories and must be more than one"<,endl;
                cin>>inp;
                if(inp == "stop" && inp_list.size() > 0){
                    break;
                }if(find(fieldnames.begin(), fieldnames.end(), input) != fieldnames.end()){
                    inp_list.push_back(inp);
                }else{
                    cout<<"not a category name in the file"<<endl;
                }
            }
            int m = 1 , n = 4;
            map<string,string> cat_list;
            //for(auto& cat: inp_list){
            //    cat_lst[cat] =np.empty(n-m+1,dtype=PyCtxt);
            //}
            //int ind = 0;
            //for row in reader:
            //    if(int(row['Number']) < int(m)):
            //        continue;
            //    if(int(row['Number']) > int(n)){
            //        break;
            //    }
            //    for cat in inp_list:
            //        cat_lst[cat][ind] = HE.encryptFrac(float(row[cat]))
            //        print(HE.decryptFrac(cat_lst[cat][ind]))
            //    ind +=1
            //for column in cat_lst.keys():
            //    sendFile('db_down',f'{column}{m}to{n}.db',s)
        } else if (inp == "val") {
            //if input is val, request value from user
            cout << "enter value, if float round up or down" << endl;
            cin >> val;
        } else if (inp == "ls") {
            send(sock, "ls", strlen("ls"), 0);
            vector<string> vectorRead;
            long size = 0;
            read(sock, &size, sizeof( size ) );
            size = ntohl( size );
            for ( int i = 0; i < size; ++i ) {
                string stringRead;
                long length = 0;
                read(sock, &length, sizeof( length ) );
                length = ntohl(length);
                while (0 < length) {
                    int cread;
                    cread = read(sock,buffer, min(sizeof(buffer),length) );
                    stringRead.append( buffer, cread );
                    length -= cread;
                }
                vectorRead.push_back( stringRead );
            }
            for(auto& val: vectorRead){
                cout<<val;
            }
        } else if (inp == "op") {
            string op;
            cout << "Enter the operation you want done" << endl;
            cin >> op;
            vector<string> oper_lst;
            string operands = "";
            while (operands != "stop") {
                string operands;
                cout << "Enter the files you want to be operated on, they must be of the same encryption" << endl;
                cin>>operands;
                cout<<endl;
                oper_lst.push_back(operands);
                reqData(sock, op, oper_lst, context);
            }
        } else if (inp == "kill") {
            cond = false;
            break;
        } else {
            cout << "not a valid entry\n commands are db_num, HE, val, ls " << endl;
        }
    }
    return 0;
}

void reqData(int sock, string operation, vector<string> operands){
    char buffer[1024] = {0};
    write(sock,"Query",strlen("Query"));
    read(sock, buffer, 1024);
    write(sock,operation.c_str(),operation.size());
    cout<<"entering reqData"<<endl;
    read(sock, buffer, 1024);
    for (int i = 0; i < operands.size(); ++i ) {
        write(sock, operands[i].data(), operands[i].length());
    }
    auto msg = read(sock, buffer, 1024);
}

void sendFile(string proc,string fn,Context obj, int sock){
    char buffer[1024] = {0};
    write(sock,proc.c_str(),proc.size());
    cout<<"waiting for proc. ack"<<endl;
    auto msg = read(sock, buffer, 1024);
    cout<<"received process ack: "<<buffer<<endl<<fn<<endl;
    write(sock,fn.data(),fn.size());
    msg = read(sock, buffer, 1024);

    ofstream pk_f;
    pk_f.open(fn, ios::out);
    write(sock,obj.data(),obj.size());

    //write(sock,fn.data(),fn.size());
    //msg = read(sock, buffer, 1024);
    //cout<<"received  file size ack: "<<buffer<<endl;

    ifstream file_contents;
    file_contents.open(fn, ios::in);
    while(file_contents.read(buffer,1024)){
        write(sock,buffer,1024);
        cout<<"sending segment"<<endl;
    }
}