/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/reconfiguration/utils.hpp>

#include <WNS/ldk/Link.hpp>
#include <WNS/pyconfig/View.hpp>

#include <algorithm>

namespace glue { namespace reconfiguration {
	typedef wns::ldk::Link::ExchangeContainer FUContainer;
} // reconfiguration
} // glue

using namespace glue::reconfiguration;

void
glue::reconfiguration::initNewFUNPart(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config)
{
	int nFunctionalUnits = config.len("functionalUnit");

	for(int ii = 0; ii < nFunctionalUnits; ++ii)
	{
		wns::pyconfig::View nodeConfig(config, "functionalUnit", ii);
		std::string name = nodeConfig.get<std::string>("commandName");

		fun->getFunctionalUnit(name)->onFUNCreated();
	}
} // initNewFUNPart


void
glue::reconfiguration::deleteFUNPart(wns::ldk::fun::FUN* fun, wns::ldk::FunctionalUnit* lowerFU, wns::ldk::FunctionalUnit* upperFU)
{
	FUContainer deletionList;
	FUContainer delivererFUs;
	FUContainer nextDelivererFUs;

	delivererFUs.push_back(lowerFU);
	deletionList = delivererFUs;

	while (!delivererFUs.empty())
	{

		for (unsigned int i = 0; i < delivererFUs.size(); ++i)
		{
			FUContainer help = delivererFUs[i]->getDeliverer()->get();
			nextDelivererFUs.insert(nextDelivererFUs.end(),
						help.begin(),
						help.end());
		}

		delivererFUs.clear();

		for (unsigned int j = 0; j < nextDelivererFUs.size(); ++j)
		{
			if (nextDelivererFUs[j] == upperFU)
				continue;

			if (std::find(deletionList.begin(),
				      deletionList.end(),
				      nextDelivererFUs[j])
			    != deletionList.end())
				continue;

			delivererFUs.push_back(nextDelivererFUs[j]);
			deletionList.push_back(nextDelivererFUs[j]);
		}

		nextDelivererFUs.clear();
	}

	for (unsigned int k = 0; k < deletionList.size(); ++k)
	{
		if (deletionList[k]->getName() != "None")
			fun->removeFunctionalUnit(deletionList[k]->getName());

		delete deletionList[k];
	}

} // deleteFUNPart



