# Plano de teste de Erros e LOGs

## Introdução

Em programas estruturados é comum as funções retornarem algum código de erro em
caso de problemas. A função pode retornar 0 caso tudo ocorra bem, ou -1 em caso
algo dê errado.

O que se torna estranho nesse tipo de função? Na maioria das vezes o retorno é
apenas um flag indicando se tudo correu bem.

Na orientação a objetos temos o conceito de exceção, mecanismo padrão para
tratamento de erros. Eles são mais claros, tornando mais fácil encontrar a fonte
do problema.

Neste documento, pretendo cobrir alguns casos de erros e falar sobre o tratamento
e o arquivo de log.

## Try and Catch

O tratamento de erros foram majoritariamente feitos utilizando o mecanismo padrão
de tratamentos de erro try and catch. Os blocos try executam todo o bloco de
código se nenhuma exceção for lançada e param imediatamente caso o contrário.

## Exemplos

### Servidor não consegue abrir arquivo de log | mainServer.cpp {11..21}

Ao tentar instanciar a classe ServerLog, se o servidor não consegue abrir
o arquivo, ele lança uma exceção que é tratada e interrompe o programa.

```c++
ServerLog* log{ nullptr };
try {
    log = new ServerLog{};
}
catch (int& e) {
    if (e == 1) {
        std::cerr << "Exception opening/reading ./log/server.log" << std::endl;
        delete log;
        exit(1);
    }
}
```

```bash
./server.elf 9090
Exception opening/reading ./log/server.log
```

Nesse caso, o diretório ./log não existia.

### Número de argumentos | mainServer.cpp {24..28}

Este é um caso que não há a necessidade de criar exeções, mas o servidor não
é iniciado com um número de argumentos diferente do esperado.

```c++
if (argc < 2 || argc > 3) {
        log->writeLog("Server FAILED to Start. Invalid number of arguments.");
        log->Help();
        delete log;
        exit(1);
}
```

A partir do momento em que a instância do ServerLog foi criada propriamente,
os erros de execução são escritos no arquivo de Log e na saída de erro para
auxiliar na depuração.

```bash
Wed Jan 24 22:42:34 2024 Venko server[8102]: Server FAILED to Start. Invalid number of arguments..
```

O formato de output do log:
Data | Nome[pid do processo]: Mensagem.

### Instanciar o servidor

Ao tentar criar uma instância do servidor, se qualquer etapa falhar, uma exceção
é lançada. Alguns exemplos:

### Port range inválido

```bash
./server.elf 1000
./server.elf 989898
./server.elf 3
```

```bash
Wed Jan 24 22:46:07 2024 Venko server[8179]: Starting Venko Project 1.0.0. server....
Wed Jan 24 22:46:07 2024 Venko server[8179]: Server FAILED to Start. Invalid port range at command line..
```

```bash
Wed Jan 24 22:35:05 2024 Venko server[7626]: Starting Venko Project 1.0.0. server....
Wed Jan 24 22:35:05 2024 Venko server[7626]: Server FAILED to Start. Invalid port range at command line..
``` 

```bash
Wed Jan 24 22:35:05 2024 Venko server[7626]: Starting Venko Project 1.0.0. server....
Wed Jan 24 22:35:05 2024 Venko server[7626]: Server FAILED to Start. Invalid port range at command line..
```

### 2 instâncias na mesma porta

Ao tentar abrir duas instâncias de um servidor em uma mesma porta, a segunda
instância cria o socket mas falha ao tentar fazer o bind do endereço ao socket,
pois a porta já está sendo utilizada. Uma exceção é lançada e o segundo programa
para sua execução.

```bash
# primeira instância
$ ./server.elf 9090
Starting Venko Project 1.0.0. server....
Server STARTED and is listening on port 9090..
```

```bash
# segunda instância
$ ./server.elf 9090
Wed Jan 24 22:51:14 2024 Venko server[8311]: Starting Venko Project 1.0.0. server....
Wed Jan 24 22:51:14 2024 Venko server[8311]: Server FAILED to Start. Error binding address to socket..
```

Isso pode acontecer também quando o servidor não se encerra por completo.

### Início completo

Se a instância do servidor é iniciada sem maiores problemas, ele escreve no log
junto à porta ao qual está atendendo.

```bash
Starting Venko Project 1.0.0. server....
Server STARTED and is listening on port 9090..
```

## Cliente

A ideia principal do cliente, também, é tratar a maior quantidades de erros
localmente, antes de enviar para o servidor. Evitando assim, comunicação
desnecessária.

Não achei necessário para este projeto fazer uma classe de log para o cliente,
apenas outputs são escritos na saída padrão e de erro pela interface para
que ele possa conectar propriamente.
Alguns exemplos:

### Endereço inexistente

```bash
./client.elf 9090 macalan.c3sl.ufpr.bsdfa
FAILED to Connect. Error resolving address.
```

### Endereço existe mas não há instância do servidor

```bash
FAILED to Connect. Error while opening a connection on socket file descriptor
```

### Comando não existente

```bash
[sasuke@macalan.c3sl.ufpr.br]$ send tsp.in  
Command not found.

If you're lost, type "help" and hit Enter.

[sasuke@macalan.c3sl.ufpr.br]$ post tsp.in  
Command not found.

If you're lost, type "help" and hit Enter.
```

## Execução completa entre múltiplos clientes em máquinas virtuais diferentes

O log de uma execução completa e sem erros

```bash
Wed Jan 24 23:10:53 2024 Venko server[370365]: Starting Venko Project 1.0.0. server....
Wed Jan 24 23:10:53 2024 Venko server[370365]: Server STARTED and is listening on port 9091..
Wed Jan 24 23:12:18 2024 Venko server[370365]: Server Accepted a new connection!.
Wed Jan 24 23:12:22 2024 Venko server[370365]: Directory for user naruto was created successfully.
Wed Jan 24 23:12:22 2024 Venko server[370365]: Session opened for user naruto from 200.17.202.11..
Wed Jan 24 23:12:52 2024 Venko server[370365]: Server Accepted a new connection!.
Wed Jan 24 23:12:55 2024 Venko server[370365]: Session opened for user sasuke from 200.17.202.34..
Wed Jan 24 23:13:22 2024 Venko server[370365]: File tsp.in received from user sasuke.
Wed Jan 24 23:14:16 2024 Venko server[370365]: File japan.png received from user naruto.
Wed Jan 24 23:14:21 2024 Venko server[370365]: File tsp.c sended to user sasuke.
Wed Jan 24 23:14:27 2024 Venko server[370365]: Client sasuke CLOSED connection with Server.
Wed Jan 24 23:14:38 2024 Venko server[370365]: Client naruto CLOSED connection with Server.
```

Nele podemos ver que:
- O servidor inicia na porta 9091
- O servidor aceita uma conexão do usuário "naruto", cria um diretório pois é a
primeira vez que o usuário conecta.
- o servidor aceita uma conexão do usuário "sasuke".
- Ambos se conectam de diferentes endereços
- Ambos são antendidos ao mesmo tempo, enviando e recebendo arquivos
- Ambos encerram a conexão com o servidor
