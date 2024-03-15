#include "master.h"
#include <server/Server.h>
using namespace ZServer;
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <map>
#include <filesystem>
#include <cmrc/cmrc.hpp>
#include "multiproccesing/WorkersPool.hpp"

CMRC_DECLARE(app1);
#define EMBEDDED_EXE_FILE "embeded.exe"

/// Credits: https://stackoverflow.com/a/13059195
/// https://stackoverflow.com/questions/13059091/
struct membuf : std::streambuf
{
   membuf(char const *base, size_t size)
   {
      char *p(const_cast<char *>(base));
      this->setg(p, p, p + size);
   }
   virtual ~membuf() = default;
};

struct memstream : virtual membuf, std::istream
{

   memstream(char const *base, char *const end)
       : membuf(base, reinterpret_cast<uintptr_t>(end) - reinterpret_cast<uintptr_t>(base)), std::istream(static_cast<std::streambuf *>(this)) {}

   memstream(char const *base, size_t size)
       : membuf(base, size), std::istream(static_cast<std::streambuf *>(this)) {}
};

std::string extractExe()
{
   auto fs = cmrc::app1::get_filesystem();
   auto fd = fs.open(EMBEDDED_EXE_FILE);
   auto filePath = (std::filesystem::temp_directory_path() / EMBEDDED_EXE_FILE).string();
   auto is = memstream(const_cast<char *>(fd.begin()), const_cast<char *>(fd.end()));
   auto ofs = std::ofstream(filePath, std::ios::binary);
   ofs << is.rdbuf();
   ofs.flush();
   ofs.close();
   return filePath;
}
void startMaster()
{
   auto filePath = extractExe();

   int slaves = 1;
   std::string memName = "mem";
   WorkersPool p(filePath, slaves, memName);
   p.createWorkers();

   SocketTCP sock;
   sock.bind(80);
   sock.listen();
   while (true)
   {
      SocketTCP client = sock.accept();
      p.addTask(client.sockfd);

   }
   
}