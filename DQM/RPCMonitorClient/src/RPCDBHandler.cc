#include "DQM/RPCMonitorClient/interface/RPCDBHandler.h"

RPCDBHandler::RPCDBHandler(const edm::ParameterSet& iConfig):
  name_(iConfig.getUntrackedParameter<std::string>("name", "RPCDBHandler")),
  sinceTime_(iConfig.getUntrackedParameter<unsigned>("IOVRun", 0)),
  rpcDQMObject_(nullptr)
{
}

void RPCDBHandler::getNewObjects()
{
  cond::Time_t myTime = sinceTime_;

  size_t n_empty_run = 0;
  if ( tagInfo().size > 0 && (tagInfo().lastInterval.since + 1) < myTime ) {
    n_empty_run = myTime - tagInfo().lastInterval.since - 1;
  }

  if ( n_empty_run != 0 ) {
    std::unique_ptr<RPCDQMObject> r(new RPCDQMObject());
    m_to_transfer.push_back(std::make_pair((RPCDQMObject*)(r->Fake_RPCDQMObject()), tagInfo().lastInterval.since + 1));
  }

  m_to_transfer.push_back(std::make_pair(rpcDQMObject_, myTime));
}

void RPCDBHandler::initObject(RPCDQMObject* fObject)
{
  rpcDQMObject_ = fObject;
}

