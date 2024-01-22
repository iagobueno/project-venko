/* Um cliente TCP/IP
   Elias P. Duarte Jr.
   Ultima Atualizacao: 17/08/22 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

typedef struct MESSAGE {
  int id;
  char msg[100];
} Message;

int main(int argc, char *argv[]) {
  int sockdescr;
  int numbytesrecv;
  struct sockaddr_in sa;
  struct hostent *hp;
  char buf[BUFSIZ + 1];
  char *host;
  char *dados;

  if (argc != 4) {
    puts("Uso correto: cliente <nome-servidor> <porta> <dados>");
    exit(1);
  }

  host = argv[1];
  dados = argv[3];

  if ((hp = gethostbyname(host)) == NULL) {
    puts("Nao consegui obter endereco IP do servidor.");
    exit(1);
  }

  bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
  sa.sin_family = hp->h_addrtype;

  sa.sin_port = htons(atoi(argv[2]));

  if ((sockdescr = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
    puts("Nao consegui abrir o socket.");
    exit(1);
  }

  if (connect(sockdescr, (struct sockaddr *)&sa, sizeof sa) < 0) {
    puts("Nao consegui conectar ao servidor");
    exit(1);
  }

  Message msg;
  strcpy(msg.msg, dados);
  msg.id = 1;

  char *msgData = malloc(sizeof(Message));
  memcpy(msgData, &msg, sizeof(Message));

  printf("Mensagem enviada (id = %d): %s\n", msg.id, msg.msg);

  if (write(sockdescr, msgData, sizeof(Message)) != sizeof(Message)) {
    puts("Nao consegui mandar os dados");
    exit(1);
  }

  read(sockdescr, buf, sizeof(Message));

  Message *msg2 = (Message *)buf;

  printf("Mensagem ecoada (id = %d): %s\n", msg2->id, msg2->msg);

  close(sockdescr);
  exit(0);
}