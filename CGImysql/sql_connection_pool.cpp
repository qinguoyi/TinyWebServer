#include<mysql/mysql.h>
#include<stdio.h>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<list>
#include<pthread.h>
#include<iostream>
#include "sql_connection_pool.h"

using namespace std;

connection_pool* connection_pool::connPool = NULL;

//构造初始化
connection_pool::connection_pool(string url,string User,string PassWord,string DBName,int Port,unsigned int MaxConn)
{
	this->url = url;
	this->Port = Port;
	this->User = User;
	this->PassWord = PassWord;
	this->DatabaseName=DBName;

	pthread_mutex_lock(&lock);
	for(int i = 0; i < MaxConn; i++)
	{
		MYSQL con;//原代码使用指针 会连接不上
		
    		if(!mysql_init(&con))
		{
			cout<<"mysql Error:"<<mysql_error(&con);
			exit(1);
		}

		if(!mysql_real_connect(&con,url.c_str(),User.c_str(),PassWord.c_str(),DBName.c_str(),Port,NULL,0))
		{
			cout<<"mysql connnect Error: "<<mysql_error(&con);
			exit(1);
		}
		connList.push_back(&con);
		++FreeConn;
	}

	this->MaxConn = MaxConn;
	this->CurConn = 0;
	pthread_mutex_unlock(&lock);
}

//获得实例，只会有一个
connection_pool* connection_pool::GetInstance(string url,string User,string PassWord,string DBName,int Port,unsigned int MaxConn)
{
	//先判断是否为空，若为空则创建，否则直接返回现有
	if(connPool == NULL)
	{
		connPool = new connection_pool(url,User,PassWord,DBName,Port,MaxConn);
	}

	return connPool;
}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL* connection_pool::GetConnection()
{
	MYSQL * con = NULL;
	pthread_mutex_lock(&lock);
	//reserve.wait();
	if(connList.size() > 0)
	{
		con = connList.front();
		connList.pop_front();
		
		--FreeConn;
		++CurConn;
		
		pthread_mutex_unlock(&lock);
		return con;
	}

	return NULL;
}

//释放当前使用的连接
bool connection_pool::ReleaseConnection(MYSQL * con)
{
	pthread_mutex_lock(&lock);
	if(con != NULL)
	{
		connList.push_back(con);
		++FreeConn;
		--CurConn;

		pthread_mutex_unlock(&lock);
		//reserve.post();
		return true;
	}
	
	return false;
}

//销毁数据库连接池
void connection_pool::DestroyPool()
{
	pthread_mutex_lock(&lock);
	if(connList.size() > 0)
	{
		list<MYSQL *>::iterator it;
		for(it = connList.begin(); it != connList.end(); ++it)
		{
			MYSQL * con = *it;
			mysql_close(con);
		}
		CurConn = 0;
		FreeConn = 0;
		connList.clear();
	}
}

//当前空闲的连接数
int connection_pool::GetFreeConn()
{
	return this->FreeConn;
}


connection_pool::~connection_pool()
{
	DestroyPool();
}
