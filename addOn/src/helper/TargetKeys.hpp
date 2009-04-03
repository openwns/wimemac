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
#ifndef WIMEMAC_HELPER_TARGETKEYS_H
#define WIMEMAC_HELPER_TARGETKEYS_H


#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Key.hpp>

#include <WNS/service/dll/Address.hpp>
#include <WIMEMAC/convergence/Upper.hpp>

namespace wimemac {namespace helper {



class MACIDKeyBuilder;

class TargetMACKey:
	public wns::ldk::Key
{
	public:
		TargetMACKey(const MACIDKeyBuilder* factory, const wns::ldk::CompoundPtr& compound, int);
		//TargetMACKey(const MACIDKeyBuilder* factory, const wns::ldk::CompoundPtr& compound, int)
		
		bool operator<(const wns::ldk::Key& other) const;
		std::string str() const;

		wns::service::dll::UnicastAddress rx;
		wns::service::dll::UnicastAddress tx;
};

class MACIDKeyBuilder:
	public wns::ldk::KeyBuilder
{
	public:
		MACIDKeyBuilder(const wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		virtual void onFUNCreated();
		virtual wns::ldk::ConstKeyPtr operator() (const wns::ldk::CompoundPtr& compound, int direction) const;

		const wns::ldk::fun::FUN* fun;
		struct Friends {
			wns::ldk::CommandReaderInterface* keyReader;
		} friends;
};
}
}


#endif