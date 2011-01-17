/******************************************************************************
 * WiMeMac                                                                    *
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2011
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef WIMEMAC_COMPONENT_HPP
#define WIMEMAC_COMPONENT_HPP

#include <DLL/Layer2.hpp>

#include <WNS/pyconfig/View.hpp>

#include <vector>

namespace wimemac { namespace convergence {
	class IPhyServices;
} // convergence
} // wimemac

namespace wimemac {

	class Component :
		public dll::Layer2
	{
	public:
		/**
		 * @brief Constructor
		 */
		Component(
			wns::node::Interface* _node,
			const wns::pyconfig::View& _config);

		/**
		 * @brief Destructor
		 */
		virtual
		~Component();

		/**
		 * @brief Implements wns::node::component::Interface::onNodeCreated()
		 */
		virtual void
		onNodeCreated();

		/**
		 * @brief Implements wns::node::component::Interface::onWorldCreated()
		 */
		virtual void
		onWorldCreated();

		/**
		 * @brief Implements wns::node::component::Interface::onShutdown()
		 */
		virtual void
		onShutdown();
		//@}
	protected:
		/**
		 * @brief Announces the services
		 */
		virtual void
		doStartup();

	private:
		/**
		 * @brief Disallow copy constructor
		 */
		Component(const Component&);

		/**
		 * @brief Disallow assignment
		 */
		Component& operator=(const Component&);

		/**
		 * @brief Convergence to PHY
		 */
		wimemac::convergence::IPhyServices* lowerConvergence;
	}; // Component

} // wimemac


#endif // NOT defined WIMEMAC_COMPONENT_HPP


