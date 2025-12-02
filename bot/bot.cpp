#include <cctype>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// --- Parametri di Connessione ---
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 6667;
const char* BOT_NICK = "bot";
const char* BOT_USER = "bot 0 * :User List Bot";
const char* BOT_PASS = "pass";
const std::string bw_list[] = {"CAZZO", "VAFFANCULO", "MERDA", "PUTTANA", "FROCIO", "NEGRO", "NIGGA"};
const int bw_n = 6;

// Funzione helper per inviare dati
void send_command(int sock_fd, const std::string& command) {
    std::string full_command = command + "\r\n";
    send(sock_fd, full_command.c_str(), full_command.length(), 0);
    std::cout << "[BOT OUT] > " << full_command;
}

void elabBuffer(std::string buffer, int sock_fd)
{
    const std::string required_prefix = "BOT :JOIN #";
    const size_t jsindex = 5;
    if (buffer.size() >= required_prefix.length() + 1)
    {
        if (buffer.find(required_prefix) == 0)
        {
            std::string channel_name = buffer.substr(required_prefix.length());
            if (channel_name.find(' ') == std::string::npos)
            {
                send_command(sock_fd, buffer.substr(jsindex));
            }
        }
    }
    std::string tou_buf = buffer;
    for (size_t n = 0; n < buffer.length(); ++n)
        tou_buf[n] = std::toupper(buffer[n]);
    if (buffer.find("PRIVMSG ") != std::string::npos)
    {
        // Parsing del Nickname (tra : e !)
        size_t nick_start = 1;
        size_t nick_end = buffer.find("!");
        if (nick_end == std::string::npos)
            return;
        
        //estrazione nickname
        std::string nickname = buffer.substr(nick_start, nick_end - nick_start);
        size_t privmsg_pos = buffer.find("PRIVMSG ");
        size_t channel_end = buffer.find(" :");
        if (privmsg_pos == std::string::npos || channel_end == std::string::npos)
            return;
        //+ 8 perche' e' la lunghezza di 'PRIVMSG '
        size_t channel_start = privmsg_pos + 8;
        std::string channel_info = buffer.substr(channel_start, channel_end - channel_start);
        //essendoci tutto il resto della stringa dall'# andiamo a cercare il primo spazio e lo usiamo per ottenere nome del canale 
        size_t channel_name_end = channel_info.find(' '); 
        std::string channel = channel_info.substr(0, channel_name_end);

        for (int i = 0; i < bw_n; i++)
        {
            if (tou_buf.find(bw_list[i]) != std::string::npos)
            {
                std::cout << "[BOT MOD] Prohibited word from " << nickname << " in " << channel << std::endl;
                std::string reason = "Shaman verdict: FORBIDDEN WORD!⚡";
                std::string kick = "KICK " + channel + " " + nickname + " :" + reason + "\r\n";
                
                send_command(sock_fd, kick);
                return ;
            }
        }
    }
}

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[512];

    std::cout << "Avvio del Bot IRC...\n";

    //CREAZIONE DEL SOCKET
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione del socket");
        return 1;
    }

    //CONFIGURAZIONE INDIRIZZO SERVER
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    //CONVERSIONE IP STRINGA -> BINARIO
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Indirizzo IP non valido/non supportato");
        close(sock_fd);
        return 1;
    }

    //CONNESSIONE
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Errore di connessione al server IRC";
        close(sock_fd);
        return 1;
    }

    std::cout << "Connesso a " << SERVER_IP << ":" << SERVER_PORT << ".\n";

    //Invio dei Comandi di Registrazione
    send_command(sock_fd, std::string("PASS ") + BOT_PASS);
    send_command(sock_fd, std::string("NICK ") + BOT_NICK);
    send_command(sock_fd, std::string("USER ") + BOT_USER);

    // 5. Loop di Ricezione (Ricevere la risposta di benvenuto)
    // Un bot deve ricevere i messaggi dal server per sapere quando è connesso e per reagire.
    while (1) {
        // Ricevi dati dal server
        int bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "[BOT IN]  < " << buffer;
            elabBuffer(buffer, sock_fd);
        } else if (bytes_received == 0) {
            std::cout << "Connessione chiusa dal server.\n";
            break;
        } else {
            perror("Errore di ricezione");
            break;
        }
    }
    // Chiusura del socket
    close(sock_fd);
    return 0;
}