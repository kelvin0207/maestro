/******************************************************************************
Copyright (c) 2019 Georgia Instititue of Technology
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author : Hyoukjun Kwon (hyoukjun@gatech.edu)
*******************************************************************************/


#include <iostream>
#include <memory>
#include <vector>
#include <list>

#include <boost/program_options.hpp>

#include "BASE_constants.hpp"
#include "BASE_base-objects.hpp"
#include "option.hpp"

#include "DFA_tensor.hpp"

#include "AHW_noc-model.hpp"

#include "CA_cost-analysis-results.hpp"

#include "API_configuration.hpp"
#include "API_user-interface-v2.hpp"


#include "DSE_cost-database.hpp"
#include "DSE_design_point.hpp"
#include "DSE_hardware_modules.hpp"
#include "DSE_csv_writer.hpp"

#include <fstream>

int main(int argc, char** argv)
{

  maestro::Options option;
  bool success = option.parse(argc, argv);

  if(!success) {
    std::cout << "[MAESTRO] Failed to parse program options" << std::endl;
  }

  std::ofstream outFile("output.log");
  // 检查文件是否成功打开
  if (!outFile.is_open()) {
      std::cerr << "无法打开文件" << std::endl;
      return 1;
  }

  maestro::InitializeBaseObjects(option.message_print_lv);

  int num_pes = option.np;
  outFile << "num_pes: " << num_pes << std::endl;
  outFile << "bw_sweep:" << option.bw_sweep << std::endl;
  outFile << "top_bw_only:" << option.top_bw_only << std::endl;

  /*
   * Hard coded part; will Fix it
   */

  // variable bandwidth, not used
  if(option.bw_sweep && option.top_bw_only) {
    int min_bw = option.bw_tick;

    for(int bw = option.min_noc_bw; bw <= option.max_noc_bw; bw += option.bw_tick) {
      std::shared_ptr<std::vector<bool>> noc_multcast = std::make_shared<std::vector<bool>>();
      std::shared_ptr<std::vector<int>> noc_latency = std::make_shared<std::vector<int>>();
      std::shared_ptr<std::vector<int>> noc_bw = std::make_shared<std::vector<int>>();

      if(option.top_bw_only) {
        noc_bw->push_back(bw);
        noc_bw->push_back(70000);
        noc_bw->push_back(70000);
        noc_bw->push_back(70000);
        noc_bw->push_back(70000);
        noc_bw->push_back(70000);

        noc_latency->push_back(option.hop_latency * option.hops);
        noc_latency->push_back(1);
        noc_latency->push_back(1);
        noc_latency->push_back(1);
        noc_latency->push_back(1);
        noc_latency->push_back(1);

        noc_multcast->push_back(option.mc);
        noc_multcast->push_back(true);
        noc_multcast->push_back(true);
        noc_multcast->push_back(true);
        noc_multcast->push_back(true);
        noc_multcast->push_back(true);
      }

      auto config = std::make_shared<maestro::ConfigurationV2>(
          option.dfsl_file_name,
          option.hw_file_name,
          noc_bw,
          noc_latency,
          noc_multcast,
          option.np,
          option.num_simd_lanes,
          option.bw,
          option.l1_size,
          option.l2_size,
          option.offchip_bw
          );

      auto api = std::make_shared<maestro::APIV2>(config);
      auto res = api->AnalyzeNeuralNetwork(option.print_res_to_screen, true);

    }
  }

  // use fixed bandwidth configuration
  else {
    // 声明了三个智能指针，指向bool、int和int类型的vector。这些智能指针用于存储网络的多播信息、延迟信息和带宽信息。
    std::shared_ptr<std::vector<bool>> noc_multcast = std::make_shared<std::vector<bool>>();
    std::shared_ptr<std::vector<int>> noc_latency = std::make_shared<std::vector<int>>();
    std::shared_ptr<std::vector<int>> noc_bw = std::make_shared<std::vector<int>>();

    //felix
    // 向noc_bw指向的vector中添加了四个元素，每个元素的值都是option.bw。这可能表示在当前配置下，所有四个带宽值都设置为option.bw。
    noc_bw->push_back(option.bw);
    noc_bw->push_back(option.bw);
    noc_bw->push_back(option.bw);
    noc_bw->push_back(option.bw);

    // 向noc_latency指向的vector中添加了四个元素，每个元素的值都是option.hop_latency和option.hops的乘积。这可能表示在当前配置下，所有四个延迟值都设置为
    noc_latency->push_back(option.hop_latency * option.hops);
    noc_latency->push_back(option.hop_latency * option.hops);
    noc_latency->push_back(option.hop_latency * option.hops);
    noc_latency->push_back(option.hop_latency * option.hops);

    // 向noc_multcast指向的vector中添加了四个true值。这可能表示在当前配置下，所有四个多播值都设置为true
    noc_multcast->push_back(true);
    noc_multcast->push_back(true);
    noc_multcast->push_back(true);
    noc_multcast->push_back(true);

    // 创建了一个maestro::ConfigurationV2对象的智能指针config。ConfigurationV2可能是maestro命名空间下的一个类，用于存储配置信息。构造函数传入了多个参数，包括文件名、带宽、延迟、多播信息等，这些参数可能用于初始化配置对象。
    auto config = std::make_shared<maestro::ConfigurationV2>(
        option.dfsl_file_name,
        option.hw_file_name,
        noc_bw,
        noc_latency,
        noc_multcast,
        option.np,
        option.num_simd_lanes,
        option.bw,
        option.l1_size,
        option.l2_size,
        option.offchip_bw
        );

    outFile << "config:" << config << std::endl;

    auto api = std::make_shared<maestro::APIV2>(config);

    // 调用了APIV2类的AnalyzeNeuralNetwork方法，传入三个布尔参数控制结果的输出方式，并将返回的分析结果存储在智能指针res中。
    auto res = api->AnalyzeNeuralNetwork(option.print_res_to_screen, option.print_res_to_csv_file, option.print_log_file);
  }
  /////////////////////////////////////////////////////////////////

  // 关闭文件
  outFile.close();


  return 0;
}
