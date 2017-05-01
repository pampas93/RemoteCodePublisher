/////////////////////////////////////////////////////////////////////////
// Sockets.cpp - C++ wrapper for Win32 socket api                      //
// ver 5.1                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// CST 4-187, Syracuse University, 315 443-3948, jfawcett@twcny.rr.com //
//---------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                      //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////

#include "Sockets.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <exception>
#include "../Utilities/Utilities.h"

using Util = Utilities::StringHelper;
template<typename T>
using Conv = Utilities::Converter<T>;
using Show = StaticLogger<1>;

/////////////////////////////////////////////////////////////////////////////
// SocketSystem class members

//----< constructor starts up sockets by loading winsock lib >---------------

SocketSystem::SocketSystem()
{
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    Show::write("\n  WSAStartup failed with error = " + Conv<int>::toString(iResult));
  }
}
//-----< destructor frees winsock lib >--------------------------------------

SocketSystem::~SocketSystem()
{
  int error = WSACleanup();
  Show::write("\n  Socket System cleaning up");
}

/////////////////////////////////////////////////////////////////////////////
// Socket class members

//----< constructor sets TCP protocol and Stream mode >----------------------

Socket::Socket(IpVer ipver) : ipver_(ipver)
{
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
}
//----< transfer socket ownership with move assignment >---------------------

Socket& Socket::operator=(Socket&& s)
{
  if (this == &s) return *this;
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = s.hints.ai_socktype;
  hints.ai_protocol = s.hints.ai_protocol;
  return *this;
}
//----< get, set IP version >------------------------------------------------
/*
*  Note: 
*    Only instances of SocketListener are influenced by ipVer().
*    Clients will use whatever protocol the server supports.
*/
Socket::IpVer& Socket::ipVer()
{
  return ipver_;
}
//----< close connection >---------------------------------------------------

void Socket::close()
{
  if (socket_ != INVALID_SOCKET)
    ::closesocket(socket_);
}
//----< tells receiver there will be no more sends from this socket >--------

bool Socket::shutDownSend()
{
  ::shutdown(socket_, SD_SEND);
  if (socket_ != INVALID_SOCKET)
    return true;
  return false;
}

//----< tells receiver this socket won't call receive anymore >--------------

bool SocketPOST:Message
mode:oneway
toAddr:toAddr:localhost:8080
fromAddr:localhost:8081
content-length:4

quitK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< promotes Win32 socket to Socket >------------------------------------
/*
*  You have to set ip version if you want IP6 after promotion, e.g.:
*     s.ipVer() = IP6;
*/
Socket::Socket(::SOCKET sock) : socket_(sock)
{
  ipver_ = IP4;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
}
//----< transfer socket ownership with move constructor >--------------------

Socket::Socket(Socket&& s)
{
  socket_ = s.socket_;
  s.socket_ = INVALID_SOCKET;
  ipver_ = s.ipver_;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = s.hints.ai_family;
  hints.ai_socktype = 