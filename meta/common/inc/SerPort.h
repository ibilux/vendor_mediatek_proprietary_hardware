#pragma once
#include "Device.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <dirent.h>
#include <errno.h>
#include "Display.h"


class CCCI : public Device
{
public:
	CCCI(const char*);
	virtual signed int read(unsigned char *buf, unsigned int len);
	virtual signed int write(const unsigned char *p, unsigned int len);

private:
	static signed int open(const char*);
};

class CCB : public Device
{
public:
	CCB();

private:
	int init();
	unsigned int checkMdStatus();
	void *ccb_memcpy(void *dst,void *src, size_t n);
	void ccb_data_copy(void* dst, void* src, unsigned int length, void* alignment_addr);
public:
	virtual signed int read(unsigned char*, unsigned int);
	virtual signed int write(const unsigned char*, unsigned int);
};

class SerPort : public Device
{
public:
	SerPort(const char*);
	SerPort();
	~SerPort();
	virtual void setSerPortExitFlag();

protected:
	virtual signed int open(const char*);
	static void initTermIO(int portFd);
};


class UartPort : public SerPort
{
public:
	UartPort(const char*);
};

class UsbPort : public SerPort
{
public:
	UsbPort(const char*);
	~UsbPort();

public:
	virtual signed int read(unsigned char*, unsigned int);
	virtual signed int write(const unsigned char*, unsigned int);
	virtual void update();
	

private:
	void close();
	int isReady() const;

private:
	const char *m_devPath;
	int m_usbFlag;
};

class MetaSocket : public SerPort
{
public:
	MetaSocket(const char*);
	~MetaSocket();
public:
	virtual signed int open(const char*);
	virtual signed int read(unsigned char*, unsigned int);
	virtual signed int write(const unsigned char*, unsigned int);
	virtual void close();
    virtual void setSerPortExitFlag();
private:
	signed int connect();
	void disconnect();
	void createDisplay();
	void destoryDisplay();
	void setConnectedFlag();
private:
	int m_nClientFd;
	bool m_bConnect;
	Display *  m_display;
    int m_nSocketConnectExitFlag;

};
