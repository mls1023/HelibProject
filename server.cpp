#include <iostream>
#include <vector>
#include <map>

#include <helib/helib.h>
#include <helib/matmul.h>


#include <fstream>
#include <sstream>

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#define HOST "127.0.0.1"
#define PORT 50001

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        return -1;
    }

    // Forcefully attaching socket to the port 50001
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 50001
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {
        return -1;
    }

    if (listen(server_fd, 1) < 0) {
        return -1;
    }
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address,
                          (socklen_t*)&addrlen))< 0) {
        return -1;
    }
    valread = read(new_socket, buffer, 1024);
    cout<<"waiting for input"<<endl;
    cout<<"recieve input"<<endl;
    string val;
    cin>> val;
    while(true){
        if(val == "HE"){
        }

        }else if(val == "ctxt"){

        }else if(val == "ls"){

        }else if(val == "db_down"){
            string f = buffer;
            read(new_socket, buffer, 1024);
            val = buffer;

        }else if(val == "Query") {
            read(new_socket, buffer, 1024);
            string op = buffer;
            string resp = "enter the files that will be computed";
            write(new_socket, resp.data(), resp.length());
            read(new_socket, buffer, 1024);
            if (val == "addv") {


            }
            cout<<"exiting query operations on server";
            read(new_socket, buffer, 1024);
        }else{
            cout<<"exiting program";
            close(new_socket);
            break;
        }


    }
}