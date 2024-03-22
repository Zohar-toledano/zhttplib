#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <map>
#include <random>
#include <filesystem>


// #include <cmrc/cmrc.hpp>
#include "master.h"
#include "osResources/SharedObject.hpp"
#include "WorkersPool.hpp"
using namespace ZServer;

// CMRC_DECLARE(app1);

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

// std::string extractExe()
// {
//    auto fs = cmrc::app1::get_filesystem();
//    auto fd = fs.open(EMBEDDED_EXE_FILE);
//    auto filePath = (std::filesystem::temp_directory_path() / EMBEDDED_EXE_FILE).string();
//    auto is = memstream(const_cast<char *>(fd.begin()), const_cast<char *>(fd.end()));
//    auto ofs = std::ofstream(filePath, std::ios::binary);
//    ofs << is.rdbuf();
//    ofs.flush();
//    ofs.close();
//    return filePath;
// }
void startMaster()
{
   // auto filePath = extractExe();
   std::string filePath = "C:\\Users\\Zohar\\Downloads\\Zserver\\build\\ZServer\\Debug\\embeded.exe";
   std::string dllPath = "C:\\Users\\Zohar\\Downloads\\Zserver\\build\\ZServer\\Debug\\user_code.dll";


   int slaves = 2;
   std::string memName = "memoooo";
   WorkersPool p(filePath, slaves, memName,dllPath);
   p.createWorkers();

   SocketTCP sock;
   sock.bind(80);
   sock.listen();
   while (true)
   {
      SocketTCP client = sock.accept();
      p.addTask(client.sockfd);

   }
   // std::random_device rd;
   // std::mt19937 gen(rd());
   // std::uniform_int_distribution<> distr(2,200 );

   //  // Generate a random number
   // for (size_t i = 0; i < 1000; i++)
   // {
   //    int num = distr(gen);
   //    std::cout<<"adding task: "<<num<<std::endl;
   //    p.addTask(num);
   //    Sleep(num);
   // }
   
}