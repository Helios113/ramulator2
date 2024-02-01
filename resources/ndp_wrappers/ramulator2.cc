#include "ramulator2.hh"

#include "base/base.h"
#include "base/config.h"
#include "base/request.h"
#include "frontend/frontend.h"
#include "memory_system/memory_system.h"
#include "mem_fetch.h"

namespace NDPSim {


void Ramulator2::init() {
  num_reads = 0;
  num_writes = 0;
  num_reqs = 0;
  
  YAML::Node config =
      Ramulator::Config::parse_config_file(config_path, {});
  ramulator2_frontend = Ramulator::Factory::create_frontend(config);
  ramulator2_memorysystem = Ramulator::Factory::create_memory_system(config);
  ramulator2_frontend->connect_memory_system(ramulator2_memorysystem);
  ramulator2_memorysystem->connect_frontend(ramulator2_frontend);
}

bool Ramulator2::full(bool is_write) const {
  return request_queue.size() >= 64;
}

void Ramulator2::push(mem_fetch* mf) {
  request_queue.push(mf);
}

mem_fetch* Ramulator2::return_queue_top() const {
  return return_queue.front();
}

mem_fetch* Ramulator2::return_queue_pop() {
  mem_fetch* mf = return_queue.front();
  return_queue.pop();
  return mf;
}

void Ramulator2::finish() {
  ramulator2_frontend->finalize();
  ramulator2_memorysystem->finalize();
}

void Ramulator2::cycle() {
  if (!request_queue.empty()) {
    mem_fetch* mf = request_queue.front();
    auto callback = [this, mf](Ramulator::Request& req) {
      if (req.type_id == Ramulator::Request::Type::Read) {
        num_reads++;
        tot_reads++;
      } else if (req.type_id == Ramulator::Request::Type::Write) {
        num_writes++;
        tot_writes++;
      }
      return_queue.push(mf);
    };
    ramulator2_frontend->receive_external_requests(
        mf->is_write() ? 1 : 0, mf->get_ramulator_addr(), 0, callback);
    request_queue.pop();
  }
  ramulator2_memorysystem->tick();
}

}  // namespace NDPSim
