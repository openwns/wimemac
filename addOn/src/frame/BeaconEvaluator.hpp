/*******************************************************************************
 * WiMeMAC                                                                     *
 * __________________________________________________________________________  *
 *                                                                             *
 * Copyright (C) 2009                                                          *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                                *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                   *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                    *
 * email: <hor@comnets.rwth-aachen.de>, www: http://wns.comnets.rwth-aachen.de/*
 ******************************************************************************/

#ifndef WIMEMAC_FRAME_BEACONEVALUATOR_H
#define WIMEMAC_FRAME_BEACONEVALUATOR_H
#include <WNS/service/dll/Address.hpp>
#include <WNS/container/Registry.hpp>
#include <WIMEMAC/frame/BeaconCommand.hpp>
#include <WIMEMAC/drp/DRPmap.hpp>
#include <WIMEMAC/drp/DRPManager.hpp>
#include <WNS/logger/Logger.hpp>

#include <WIMEMAC/helper/IDRPQueueInterface.hpp>

namespace wimemac {
namespace frame {

	class BeaconEvaluator
		{
		public:
			BeaconEvaluator();
			void BeaconExamination(BeaconCommand* BeaconCommand, bool isDRPconnection,wns::logger::Logger _logger );
			void setFriend(helper::IDRPQueueInterface* QueueInterface);
			void CollectDRPmaps();
						
		private:
		typedef wns::container::Registry<wns::service::dll::UnicastAddress, drp::DRPManager*> ContainerType;
		
		ContainerType DRPConnections;
		ContainerType::const_iterator it;
		drp::DRPManager* DRPmanager;
		drp::DRPmap* DRPmapManager;
		wns::logger::Logger logger;
		helper::IDRPQueueInterface* queueInterface;
		struct Friends
		{
			helper::IDRPQueueInterface* QueueInterface;
		} friends;
		
		
		};
}
}
#endif

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
