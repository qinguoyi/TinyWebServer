server: main.c ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./log/log.cpp ./log/log.h ./log/block_queue.h ./CGImysql/sql_connection_pool.cpp ./CGImysql/sql_connection_pool.h
	g++ -o server main.c ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./log/log.cpp ./log/log.h ./CGImysql/sql_connection_pool.cpp ./CGImysql/sql_connection_pool.h -lpthread -lmysqlclient

CGISQL.cgi:./CGImysql/sign.cpp ./CGImysql/sql_connection_pool.cpp ./CGImysql/sql_connection_pool.h
	g++ -o ./root/CGISQL.cgi ./CGImysql/sign.cpp ./CGImysql/sql_connection_pool.cpp ./CGImysql/sql_connection_pool.h -lmysqlclient -lpthread


clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
