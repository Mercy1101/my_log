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

#ifndef MY_LOG_INCLUDE_PROFILER_H_
#define MY_LOG_INCLUDE_PROFILER_H_

#include <chrono>
#include <ctime>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <ratio>
#include <utility>

#include "log.hpp"

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
  }
  ~profiler_log_wrapper() { delete profiler_logger;
  }
 private:
  profiler_log_wrapper() {

  profiler_logger = new lee::rotating_file_sink<std::mutex>(std::string("log/profiler/profiler.log"));
    profiler_logger->set_level(DEFAULT_PROFILER_LOG_LEVEL); 
  }
  lee::rotating_file_sink<std::mutex> *profiler_logger = nullptr;

};

class ProfilerInstance {
 public:
  typedef std::chrono::steady_clock SteadyClock;
  typedef SteadyClock::time_point TimePoint;
  typedef std::chrono::duration<double, std::ratio<1, 1>>
      DurationTime;  //单位秒
  enum class MemoryUnit { KB_, MB_, GB_ };

  ProfilerInstance(const std::string& sFunc, const std::string& sFile,
                   const int iLine)
      : m_Func(sFunc),
        m_File(sFile),
        m_Line(iLine),
        duringTime(0),
        startTime(SteadyClock::now()),
        finishTime(SteadyClock::now()) {
    lee::profiler::profiler_log_wrapper::get_instance().log(lee::get_time_string()+
        "\nProfiler is Running in " + sFunc + " Line: " + std::to_string(iLine) +
        " Memory: " + std::to_string(memory()) + " KB(" +
        std::to_string(memory(MemoryUnit::MB_)) + " MB)\n ");
    start();
  }

  ~ProfilerInstance() {
    finish();
    lee::profiler::profiler_log_wrapper::get_instance().log(
        lee::get_time_string()+
        "\n1Function: "+m_Func+" \nSpand Time: " + std::to_string(millisecond()) +
        "ms( " + std::to_string(second()) +
        "s) \nMemory: " + std::to_string(memory()) + " KB(" +
        std::to_string(memory(MemoryUnit::MB_)) +
        " MB) \n"
        "In File: " +
        m_File + "  LINE: " + std::to_string(m_Line) +
        "\nEnd of Function: " + m_Func + "}\n");
  }

 private:
#define KB / 1024
#define MB KB / 1024
#define GB MB / 1024
 private:
  DurationTime duringTime;
  TimePoint startTime;
  TimePoint finishTime;

  std::string m_Func;  ///< 传入的需要分析的函数名
  std::string m_File;  ///< 传入的当前分析在那个文件
  int m_Line;          ///< 传入的当前分析在哪一行

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
    (void)mu;
    return 0;
#if 0
    size_t memory = 0;

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
    return memory;
#endif
  }
};  // end of class ProfilerInstance

}  // namespace profiler
}  // namespace lee

#define LEE_C11_PROFILER_MODE
#ifdef LEE_C11_PROFILER_MODE
#define PROFILER_F() \
  lee::profiler::ProfilerInstance pRoFiLeR__(__func__, __FILE__, __LINE__)
#else
#define PROFILER_F() nullptr
#endif  // end of LEE_C11_PROFILER_MODE

#endif
