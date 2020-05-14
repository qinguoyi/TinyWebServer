server: main.cpp ./timer/lst_timer.h ./timer/lst_timer.cpp ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./log/log.cpp ./log/log.h ./log/block_queue.h ./CGImysql/sql_connection_pool.cpp ./CGImysql/sql_connection_pool.h webserver.h webserver.cpp config.h config.cpp
	g++ -o server main.cpp ./timer/lst_timer.h ./timer/lst_timer.cpp ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./log/log.cpp ./log/log.h ./CGImysql/sql_connection_pool.cpp ./CGImysql/sql_connection_pool.h webserver.h webserver.cpp config.h config.cpp -lpthread -lmysqlclient -g

CGISQL.cgi:./CGImysql/sign.cpp ./lock/locker.h
	g++ -o ./root/CGISQL.cgi ./CGImysql/sign.cpp ./lock/locker.h -lmysqlclient -lpthread

clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
