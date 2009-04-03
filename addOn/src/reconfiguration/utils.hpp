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

#ifndef GLUE_RECONFIGURATION_UTILS_HPP
#define GLUE_RECONFIGRUATION_UTILS_HPP

#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/fun/FUN.hpp>

#include <WNS/pyconfig/View.hpp>

namespace glue { namespace reconfiguration {

	void
	initNewFUNPart(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

	void
	deleteFUNPart(wns::ldk::fun::FUN* fun, wns::ldk::FunctionalUnit* lowerFU, wns::ldk::FunctionalUnit* upperFU);

} // reconfiguration
} // glue

#endif // NOT defined GLUE_RECONFIGURATION_UTILS_HPP


