udp_sender_receiver --> contains simple udp sender and receiver 

tcp_sender_receiver --> contains tcp sender and tcp reciver ( three way handshaking )

---------------the above codes can handle only single client at a time and rest all client have to wait till first client fininshes it sending and receiving task-----------------------------

concurrent_server -->
 concurrent server using fork is implemented, so, that server can now execute all users in parallel.

But the problem whith fork is ,if somehow the parents process got killed than the child processes will become zombii!!.

To avoid this we can use select() call. As implemnted in 
--> tcp_using_select
