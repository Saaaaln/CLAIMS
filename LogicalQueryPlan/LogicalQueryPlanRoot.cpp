/*
 * LogicalQueryPlanRoot.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: wangli
 */

#include "LogicalQueryPlanRoot.h"
#include "../Resource/NodeTracker.h"
#include "../IDsGenerator.h"
#include "../BlockStreamIterator/ParallelBlockStreamIterator/ExpandableBlockStreamExchangeEpoll.h"
#include "../BlockStreamIterator/ParallelBlockStreamIterator/BlockStreamExpander.h"
#include "../BlockStreamIterator/BlockStreamPrint.h"
#include "../PerformanceMonitor/BlockStreamPerformanceMonitorTop.h"
LogicalQueryPlanRoot::LogicalQueryPlanRoot(NodeID collecter,LogicalOperator* child,const outputFashion& fashion)
:collecter_(collecter),child_(child),fashion_(fashion){
	// TODO Auto-generated constructor stub

}

LogicalQueryPlanRoot::~LogicalQueryPlanRoot() {
	// TODO Auto-generated destructor stub
}

BlockStreamIteratorBase* LogicalQueryPlanRoot::getIteratorTree(const unsigned& block_size){
	getDataflow();
	BlockStreamIteratorBase* child_iterator=child_->getIteratorTree(block_size);
	Dataflow dataflow=child_->getDataflow();
	Schema* schema=getSchema(dataflow.attribute_list_);
	NodeTracker* node_tracker=NodeTracker::getInstance();

	BlockStreamExpander::State expander_state;
	expander_state.block_count_in_buffer_=10;
	expander_state.block_size_=block_size;
	expander_state.thread_count_=5;
	expander_state.child_=child_iterator;
	expander_state.schema_=getSchema(dataflow.attribute_list_);
	BlockStreamIteratorBase* expander=new BlockStreamExpander(expander_state);


	ExpandableBlockStreamExchangeEpoll::State state;
	state.block_size=block_size;
	state.child=expander;//child_iterator;
	state.exchange_id=IDsGenerator::getInstance()->generateUniqueExchangeID();
	state.schema=schema;
	state.upper_ip_list.push_back(node_tracker->getNodeIP(collecter_));
	state.partition_key_index=0;
	std::vector<NodeID> lower_id_list=getInvolvedNodeID(dataflow.property_.partitioner);
	for(unsigned i=0;i<lower_id_list.size();i++){
		const std::string ip=node_tracker->getNodeIP(lower_id_list[i]);
		assert(ip!="");
		state.lower_ip_list.push_back(ip);
	}

	BlockStreamIteratorBase* exchange=new ExpandableBlockStreamExchangeEpoll(state);

	BlockStreamIteratorBase* ret;
	switch(fashion_){
		case PRINT:{
			BlockStreamPrint::State print_state(schema,exchange,block_size);
			ret=new BlockStreamPrint(print_state);
			break;
		}
		case PERFORMANCE:{
			BlockStreamPerformanceMonitorTop::State performance_state(schema,exchange,block_size);
			ret=new BlockStreamPerformanceMonitorTop(performance_state);
			break;
		}
	}

	return ret;
}
Dataflow LogicalQueryPlanRoot::getDataflow(){
	Dataflow ret=child_->getDataflow();
	printf("Communication cost:%d, predicted ouput size=%d\n",ret.property_.commnication_cost,ret.property_.partitioner.getAggregatedDatasize());
	return child_->getDataflow();
}

bool LogicalQueryPlanRoot::GetOptimalPhysicalPlan(Requirement requirement,PhysicalPlanDescriptor& final_physical_plan_desc, const unsigned & block_size){
	std::vector<PhysicalPlanDescriptor> candidate_physical_plan;
	Requirement req;
	req.setRequiredLocations(std::vector<NodeID>(collecter_));
	PhysicalPlanDescriptor physical_plan;

	/** no requirement**/
	if(child_->GetOptimalPhysicalPlan(Requirement(),physical_plan)){

		NetworkTransfer transfer=req.requireNetworkTransfer(physical_plan.dataflow);
		if(transfer==NONE){
			candidate_physical_plan.push_back(physical_plan);
		}
		else if((transfer==OneToOne)||(transfer==Shuffle)){
			/* the input data flow should be transfered in the network to meet the requirement
			 * TODO: implement OneToOne Exchange
			 * */
			physical_plan.cost+=physical_plan.dataflow.getAggregatedDatasize();

			ExpandableBlockStreamExchangeEpoll::State state;
			state.block_size=block_size;
			state.child=physical_plan.plan;//child_iterator;
			state.exchange_id=IDsGenerator::getInstance()->generateUniqueExchangeID();
			state.schema=getSchema(physical_plan.dataflow.attribute_list_);
			state.upper_ip_list.push_back(NodeTracker::getInstance()->getNodeIP(collecter_));
			state.partition_key_index=0;
			std::vector<NodeID> lower_id_list=getInvolvedNodeID(physical_plan.dataflow.property_.partitioner);
			for(unsigned i=0;i<lower_id_list.size();i++){
				const std::string ip=NodeTracker::getInstance()->getNodeIP(lower_id_list[i]);
				assert(ip!="");
				state.lower_ip_list.push_back(ip);
			}

			BlockStreamIteratorBase* exchange=new ExpandableBlockStreamExchangeEpoll(state);
			physical_plan.plan=exchange;

		}
	}

	/** with requirement**/
	if(child_->GetOptimalPhysicalPlan(req,physical_plan)){
		candidate_physical_plan.push_back(physical_plan);
	}

	PhysicalPlanDescriptor best_plan=getBestPhysicalPlanDescriptor(candidate_physical_plan);

	PhysicalPlan final_plan;
	switch(fashion_){
		case PRINT:{
			BlockStreamPrint::State print_state(getSchema(best_plan.dataflow.attribute_list_),best_plan.plan,block_size);
			final_plan=new BlockStreamPrint(print_state);
			break;
		}
		case PERFORMANCE:{
			BlockStreamPerformanceMonitorTop::State performance_state(getSchema(best_plan.dataflow.attribute_list_),best_plan.plan,block_size);
			final_plan=new BlockStreamPerformanceMonitorTop(performance_state);
		}
	}


	final_physical_plan_desc.cost=best_plan.cost;
	final_physical_plan_desc.dataflow=best_plan.dataflow;
	final_physical_plan_desc.plan=final_plan;

	if(requirement.passLimits(final_physical_plan_desc.cost))
		return true;
	else
		return false;
}
