#ifndef __RAMULATOR2_HH__
#define __RAMULATOR2_HH__

#include <deque>
#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
// Forward declare Ramulator2 top-level components
namespace Ramulator {
class IFrontEnd;
class IMemorySystem;
}  // namespace Ramulator

namespace NDPSim {
struct mem_fetch {
  uint64_t addr;
  bool write;
  bool request;
  void* origin_data;
  int size;
  void set_reply() { request = false; }
  bool is_write() const { return write; }
};

class Ramulator2 {
 public:
  Ramulator2() {}
  Ramulator2(unsigned memory_id, unsigned num_channels,
             std::string ramulator_config, std::string out, int log_interval)
      : memory_id(memory_id), num_channels(num_channels),
        config_path(ramulator_config),
        log_interval(log_interval) {
    init();
  }
  ~Ramulator2() {
    // Destructor implementation (if needed)
  }
  void init();
  bool full() const;
  void cycle();
  void finish();
  void print(FILE *fp = NULL);
  void push(class mem_fetch *mf);
  mem_fetch *return_queue_top() const;
  mem_fetch *return_queue_pop();
  void return_queue_push_back(mem_fetch *mf);
  bool returnq_full() const;

  // virtual bool is_active();
  // virtual void set_dram_power_stats(unsigned &cmd, unsigned &activity,
  //                                   unsigned &nop, unsigned &actpre,
  //                                   unsigned &pre2act, unsigned &rd,
  //                                   unsigned &wr, unsigned &req,
  //                                   unsigned &bytes) const;

 private:
  bool is_gpu;
  std::string std_name;
  std::string config_path;
  std::queue<mem_fetch *> request_queue;
  std::queue<mem_fetch *> return_queue;
  Ramulator::IFrontEnd *ramulator2_frontend;
  Ramulator::IMemorySystem *ramulator2_memorysystem;
  int memory_id;
  int num_channels;
  uint64_t cycle_count = 0;
  int log_interval = 10000;
  int num_reqs;
  int num_reads;
  int num_writes;
  int tot_reqs;
  int tot_reads;
  int tot_writes;
};

}  // namespace NDPSim
#endif  // __RAMULATOR2_HH__
