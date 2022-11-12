#include "header.h"
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;
sem_t minimal;

void aceptar_cliente( vector<int> &direcciones, int s, vector<int> &listpuerto)
{
    int puerto = PORT + 1;
    int amount = 0;
    while(true)
    { 
    
        int s1;
        struct sockaddr_in remote;
        int t;
        request req;
        strncpy(req.type, "PUERTO", 15);
        string puertotemp_str = to_string(puerto);
        char const* puerto2 = puertotemp_str.c_str();
        strncpy(req.msg, puerto2, MENSAJE_MAXIMO);
        if((s1 = accept(s, (struct sockaddr *) &remote, (socklen_t *) &t))== -1)
        {
            perror("aceptando la conexion entrante");
            exit(1);
        }
        listpuerto.push_back(puerto);    
        direcciones.push_back(s1);
        sendrequest(s1, req);
        puerto++;
    
        amount++;
        if(amount == 9)
        {   
            sem_post(&minimal);
        }
    
    }
}

int main()

{
    sem_init(&minimal, 0, 0);
    request req;
    int s, s1;
    vector <int> direcciones; 
    struct sockaddr_in local;
    struct sockaddr_in remote;
    thread threads;
    vector <int> listpuerto;
    vector <vector <int>> Portmatrix;
    vector <vector <int>> Sockmatrix; 
    vector <int> game_sockets;
    


    if ((s = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("socket");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(PORT);
    local.sin_addr.s_addr = INADDR_ANY; 

    if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        exit(1);  
    }

    if (listen(s, 10) == -1) 
    {
        perror("listen");
        exit(1);
    }

    int t = sizeof(remote);
    int i = 0;  
    threads = thread(aceptar_cliente, ref(direcciones), s, ref(listpuerto));
    sem_wait(&minimal);

    for (int a = 0; a < 3; a++)
    {   vector <int> rowsockets;
        vector <int> rowports;
        
        for (int j = 0; j < 3 ; j++)
        {
            game_sockets.push_back(direcciones[j + 3 * a]);
            rowports.push_back(listpuerto[j + 3 * a]);
            rowsockets.push_back(direcciones[j + 3 * a]);

        }
        
        Portmatrix.push_back(rowports);
        Sockmatrix.push_back(rowsockets);
    }

    strncpy(req.type, "NEW_CLIENTS", 15);
    for (int y = 0; y < 3; y++)
    {    
        for (int x = 0; x < 3; x++)
        {
            string vecinos;
            if (x - 1 >= 0 && x - 1 < Portmatrix.size())
            {
               vecinos += to_string (Portmatrix[y] [x - 1]);
               vecinos += "k";  
            }

            if (y - 1 >= 0 && y - 1 < Portmatrix.size())
            {
                vecinos += to_string (Portmatrix[y - 1] [x]);
                vecinos += "k"; 
            }
            
            if (x - 1 >= 0 && x - 1 < Portmatrix.size() && y - 1 >= 0 && y - 1 < Portmatrix.size())
            {
               vecinos += to_string (Portmatrix[y - 1] [x - 1]);
               vecinos += "k";   
            }

            if (x - 1 >= 0 && x - 1 < Portmatrix.size() && y + 1 >= 0 && y + 1 < Portmatrix.size())
            {
               vecinos += to_string (Portmatrix[y + 1] [x - 1]);
               vecinos += "k";   
            }

            if (x + 1 >= 0 && x + 1 < Portmatrix.size() && y + 1 >= 0 && y + 1 < Portmatrix.size())
            {
               vecinos += to_string (Portmatrix[y + 1] [x + 1]);  
               vecinos += "k"; 
            }

            if (x + 1 >= 0 && x + 1 < Portmatrix.size())
            {
               vecinos += to_string (Portmatrix[y] [x + 1]); 
               vecinos += "k";  
            }

            if (x + 1 >= 0 && x + 1 < Portmatrix.size() && y - 1 >= 0 && y - 1 < Portmatrix.size())
            {
               vecinos += to_string (Portmatrix[y - 1] [x + 1]); 
               vecinos += "k";  
            }

            if (y + 1 >= 0 && y + 1 < Portmatrix.size())
            {
                vecinos += to_string (Portmatrix[y + 1] [x]);
                vecinos += "k"; 
            }        
        
            strncpy(req.msg, vecinos.c_str(),MENSAJE_MAXIMO);        
            int socket = Sockmatrix[y][x];
            sendrequest (socket, req);
        }                    
    }

    for (int i = 0; i < 3; i++)
    {  
        for (int j = 0; j < 3; j++)
        {
            recvrequest(Sockmatrix[i][j], req);
            string type_req(req.type);
            if (type_req != "CLIENT_READY" )
            {
                perror("sending neigbours");
                exit(1);
            }            
        }        
    }
    
    while (true)
    {
        strncpy(req.type, "TICK", 15);
        broadcast(game_sockets, req);
        for (int i = 0; i < 3; i++)        {
            cout << "" << endl;
            for (int j = 0; j < 3; j++)
            {
                recvrequest(Sockmatrix[i][j], req);

                string type_req(req.type);

                if (type_req != "CLIENT_STATE" )
                {
                    perror("recieving state");
                    exit(1);
                }
                
                string msg_req(req.msg);

                if (msg_req == "MUERTO" )
                {
                    cout << "| X |";
                }
                else 
                {
                    cout << "| O |";
                }                
            }            
        }

        cout << "" << endl;
        cout << "-------------------------------------------------------------------------" << endl;
        sleep(10); 
    }
    
    
    
    threads.join();
    return 0;
}
