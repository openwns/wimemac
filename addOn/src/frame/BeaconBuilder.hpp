/******************************************************************************
 * WiMeMAC                                                                    *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2009                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 5, D-52074 Aachen, Germany                                  *
 * phone: ++49-241-80-27920 (phone), fax: ++49-241-80-22242                   *
 * email: hor@comnets.de, www: http://wns.comnets.rwth-aachen.de/             *
 *****************************************************************************/
#ifndef WIMEMAC_FRAME_BEACONBUILDER_H
#define WIMEMAC_FRAME_BEACONBUILDER_H
#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/convergence/Lower.hpp>
#include <WIMEMAC/frame/BeaconCommand.hpp>
#include <WIMEMAC/frame/BeaconEvaluator.hpp>

#include <WIMEMAC/helper/IDRPQueueInterface.hpp>
#include <vector>



namespace wimemac {
namespace frame {
typedef std::vector<bool> Vector;

	class BeaconBuilder :
		
		public wns::ldk::fu::Plain<BeaconBuilder, BeaconCommand>,
		public BeaconEvaluator
		
	  {
		public:

			BeaconBuilder(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config_ );
			void doOnData(const wns::ldk::CompoundPtr& );
			void doSendData(const wns::ldk::CompoundPtr& );
			void BuildDTPmap();
			bool doIsAccepting(const wns::ldk::CompoundPtr& compound) const;
			void doWakeup();

			
			//void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;


		protected:

			virtual void onFUNCreated();

		private:
			void calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;
			wns::ldk::CompoundPtr CreateBeacon();
			wns::ldk::CompoundPtr currentBeacon;
			wns::logger::Logger logger;
			//wns::pyconfig::View config;
		
			bool isDRPconnection;


			struct Friends
			{
				wimemac::convergence::BroadcastUpper* broadcastUpper;
				wimemac::helper::IDRPQueueInterface* queueInterface;
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
