/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/


#ifndef GLUE_MIHCOMPONENT_HPP
#define GLUE_MIHCOMPONENT_HPP

#include <GLUE/Component.hpp>
#include <GLUE/MIH/MeasurementsMonitor.hpp>
#include <GLUE/MIH/LinkCommandProcessor.hpp>
#include <GLUE/MIH/CapabilityDiscoveryProvider.hpp>


namespace glue {
	class MIHComponent:
		 public glue::Component
	{
	public:
		/**
		 * @brief Constructor
		 */
		MIHComponent(
			wns::node::Interface* _node,
			const wns::pyconfig::View& _config);

		/**
		 * @brief Destructor
		 */
		virtual
		~MIHComponent(){};

	protected:
		virtual void
		doStartup();

	private:
		/**
		 * @brief Measurements Monitor FU will receive a set of
		 * measurements from other FUs,generate Link Events and deliver
		 * them to the MIH
		 */
		glue::mih::MeasurementsMonitor* measurementsMonitor;

		/**
		 * @brief LinkCommandProcessor is a receiver of MIH Commands
		 * For Glue it is a dummy class doing nothing
		 */
		glue::mih::LinkCommandProcessor* linkCommandProcessor;

		/**
		 * @brief CapabilityDiscoveryProvider will inform the MIHF about
		 * the local DLL supported events list
		 */
		glue::mih::CapabilityDiscoveryProvider* capabilityDiscoveryProvider;
	};

} //glue

#endif // NOT defined GLUE_MIHCOMPONENT_HPP
