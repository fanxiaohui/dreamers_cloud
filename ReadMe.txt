±‡“Î√¸¡Ó:
1: main.c
	gcc -g  main.c  -o cloud -lpthread
2: main.c  ./websocket/websocket_common.c
	gcc -g  main.c  ./websocket/websocket_common.c  -o cloud -lpthread
3. main.c  ./websocket/websocket_common.c ./Mode/Mode_Common.c
	gcc -g  main.c  ./websocket/websocket_common.c ./Mode/Mode_Commond.c -o cloud -lpthread
4. main.c  ./websocket/websocket_common.c ./Mode/Mode_Common.c ./Init/Net_init.c
	gcc -g  main.c  ./websocket/websocket_common.c ./Mode/Mode_Commond.c \
	./Init/ Net_init.c -o cloud -lpthread

5. main.c  ./websocket/websocket_common.c ./Mode/Mode_Common.c \
   ./Init/Net_init.c ./Pthread/Pthread_Net.c
	gcc -g  main.c  ./websocket/websocket_common.c ./Mode/Mode_Commond.c ./Init/Net_init.c 
	\ ./Pthread/Pthread_Net.c -o cloud -lpthread

6. main.c  ./websocket/websocket_common.c ./Mode/Mode_Common.c \
   ./Init/Net_init.c ./Pthread/Pthread_Net.c ./Cjson/cjson.c
	gcc -g  main.c  ./websocket/websocket_common.c ./Mode/Mode_Commond.c ./Init/Net_init.c 
	\ ./Pthread/Pthread_Net.c ./Cjson/cjson.c -o cloud -lpthread -lm

7.gcc -g  main.c  ./websocket/websocket_common.c ./Mode/Mode_Commond.c ./Init/
Net_init.c  ./Pthread/Pthread_Net.c ./Cjson/cjson.c ./Init/Config_init.c -o 
cloud -lpthread -lm

8.gcc -g  main.c  ./websocket/websocket_common.c ./Mode/Mode_Commond.c ./Init/
Net_init.c  ./Pthread/Pthread_Net.c ./Cjson/cjson.c ./Init/Config_init.c ./net
/net.c -o cloud -lpthread -lm


‘À––√¸¡Ó:
Run: valgrind --tool=memcheck --leak-check=full ./cloud 






valgrind  µ˜ ‘≥ˆ¥Ì:
==5045== Thread 5:
==5045== Conditional jump or move depends on uninitialised value(s)
==5045==    at 0x4C2F1D0: strcmp (in /usr/lib/valgrind/vgpreload_memcheck-
amd64-linux.so)

==5045==    by 0x4041AB: mode_Recv_state (Mode_Commond.c:54)
==5045==    by 0x40546A: service_pthread (Pthread_Net.c:45)
==5045==    by 0x4E3F183: start_thread (pthread_create.c:312)
==5045==    by 0x545537C: clone (clone.S:111)

