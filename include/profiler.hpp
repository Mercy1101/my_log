///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.
///
/// @file   profiler.hpp
/// @brief  性能测试工具
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-19 14:23:58
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef INCLUDE_PROFILER_HPP_
#define INCLUDE_PROFILER_HPP_

#include <chrono>
#include <ctime>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <ratio>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/resource.h>
#include <sys/time.h>

#endif

#include "my_log/log.hpp"

namespace lee {
namespace profiler {

constexpr lee::level_enum DEFAULT_PROFILER_LOG_LEVEL = lee::level_enum::trace;

class profiler_log_wrapper {
 public:
  static profiler_log_wrapper& get_instance() {
    static std::once_flag flag;
    static profiler_log_wrapper* instance = nullptr;
    std::call_once(flag, [&]() { instance = new profiler_log_wrapper(); });
    return *instance;
  }
  void log(const std::string& log) {
    profiler_logger->log(log);
    profiler_logger->flush();
  }
  ~profiler_log_wrapper() { delete profiler_logger; }

 private:
  profiler_log_wrapper() {
    profiler_logger = new lee::rotating_file_sink<std::mutex>(
        std::string("log/profiler/profiler.log"));
    profiler_logger->set_level(DEFAULT_PROFILER_LOG_LEVEL);
  }
  lee::rotating_file_sink<std::mutex>* profiler_logger = nullptr;
};

class ProfilerInstance {
 public:
  typedef std::chrono::steady_clock SteadyClock;
  typedef SteadyClock::time_point TimePoint;
  typedef std::chrono::duration<double, std::ratio<1, 1>>
      DurationTime;  //单位秒
  enum class MemoryUnit { KB_, MB_, GB_ };

  ProfilerInstance(const std::thread::id thread_id, const std::string& sFunc,
                   const std::string& sFile, const int iLine)
      : m_Func(sFunc),
        m_File(sFile),
        m_Line(iLine),
        duringTime(0),
        startTime(SteadyClock::now()),
        finishTime(SteadyClock::now()) {
    if (m_File.find_last_of('/') != std::string::npos) {
      m_File = m_File.substr(m_File.find_last_of('/') + 1);
    }
    std::ostringstream oss;
    oss << thread_id;
    pid_ = oss.str();
    start();
  }

  ~ProfilerInstance() {
    finish();
    std::string millsec = std::to_string(millisecond());
    if (millsec.find('.') != std::string::npos) {
      millsec = millsec.substr(0, millsec.find('.') + 3 + 1);
    }
    std::string sec = std::to_string(second());
    if (sec.find('.') != std::string::npos) {
      sec = millsec.substr(0, sec.find('.') + 3 + 1);
    }
    std::string log;
    log += lee::get_log_time_string();
    log += "[profiler]";
    log += "Spand time: ";
    log += millsec;
    log += " ms(";
    log += sec;
    log += " s), ";
    log += "Member usage: ";
    log += std::to_string(memory(MemoryUnit::MB_));
    log += " MB(";
    log += std::to_string(memory());
    log += " KB)";
    log += " <In Function: ";
    log += m_Func;
    log += ", File: ";
    log += m_File;
    log += ", Line: ";
    log += std::to_string(m_Line);
    log += ", PID: ";
    log += pid_;
    log += ">\n";
    lee::profiler::profiler_log_wrapper::get_instance().log(log);
  }

 private:
#define KB / 1024
#define MB KB / 1024
#define GB MB / 1024
 private:
  std::string m_Func;  ///< 传入的需要分析的函数名
  std::string m_File;  ///< 传入的当前分析在那个文件
  int m_Line;          ///< 传入的当前分析在哪一行
  std::string pid_;
  DurationTime duringTime;
  TimePoint startTime;
  TimePoint finishTime;

 public:
  void start()  // 开始计时
  {
    startTime = SteadyClock::now();
  }

  void finish()  // 结束计时
  {
    finishTime = SteadyClock::now();
    duringTime =
        std::chrono::duration_cast<DurationTime>(finishTime - startTime);
  }

  void dumpDuringTime(std::ostream& os = std::cout)  // 打印时间
  {
    os << "total " << duringTime.count() * 1000 << " milliseconds" << std::endl;
  }

  double second()  // 以秒为单位返回时间
  {
    return duringTime.count();
  }

  double millisecond()  // 以毫秒为单位返回时间
  {
    return duringTime.count() * 1000;
  }

  size_t memory(MemoryUnit mu = MemoryUnit::KB_)  // 查询当前程序的内存使用量
  {
    size_t memory = 0;

#ifdef WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = GetCurrentProcess();
    if (!GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
      throw std::runtime_error("GetProcessMemoryInfo failed");
    CloseHandle(hProcess);

    switch (mu) {
      case MemoryUnit::KB_:
        memory = pmc.WorkingSetSize KB;
        break;
      case MemoryUnit::MB_:
        memory = pmc.WorkingSetSize MB;
        break;
      case MemoryUnit::GB_:
        memory = pmc.WorkingSetSize GB;
        break;
    }
#else
    memory = static_cast<size_t>(get_sys_mem_use());
    switch (mu) {
      case MemoryUnit::KB_:
        memory = memory KB;
        break;
      case MemoryUnit::MB_:
        memory = memory MB;
        break;
      case MemoryUnit::GB_:
        memory = memory GB;
        break;
    }
#endif
    return memory;
  }
  int get_sys_mem_use() {  //获取系统当前可用内存
    int mem_free = -1;     //空闲的内存，=总内存-使用了的内存
    int mem_total = -1;    //当前系统可用总内存
    int mem_buffers = -1;  //缓存区的内存大小
    int mem_cached = -1;   //缓存区的内存大小
    char name[20];

    FILE* fp;
    char buf1[128], buf2[128], buf3[128], buf4[128], buf5[128];
    int buff_len = 128;
    fp = fopen("/proc/meminfo", "r");
    if (fp == nullptr) {
      fclose(fp);
      throw std::runtime_error("GetSysMemInfo() error! file not exist");
    }
    if (nullptr == fgets(buf1, buff_len, fp) ||
        nullptr == fgets(buf2, buff_len, fp) ||
        nullptr == fgets(buf3, buff_len, fp) ||
        nullptr == fgets(buf4, buff_len, fp) ||
        nullptr == fgets(buf5, buff_len, fp)) {
      fclose(fp);
      throw std::runtime_error("GetSysMemInfo() error! fail to read!");
      return -1;
    }
    fclose(fp);
    sscanf(buf1, "%s%d", name, &mem_total);
    sscanf(buf2, "%s%d", name, &mem_free);
    /// sscanf(buf4, "%s%d", name, &mem_buffers);
    /// sscanf(buf5, "%s%d", name, &mem_cached);
    /// int memLeft = mem_free + mem_buffers + mem_cached;
    return mem_total - mem_free;
  }
};  // end of class ProfilerInstance

}  // namespace profiler
}  // namespace lee

#define LEE_C11_PROFILER_MODE
#ifdef LEE_C11_PROFILER_MODE
#define PROFILER_F()                                                     \
  lee::profiler::ProfilerInstance pRoFiLeR__(std::this_thread::get_id(), \
                                             __func__, __FILE__, __LINE__)
#else
#define PROFILER_F() nullptr
#endif  // end of LEE_C11_PROFILER_MODE

#endif
