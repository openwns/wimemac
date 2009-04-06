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
#include <WIMEMAC/frame/BeaconScheduler.hpp>

using namespace wimemac::frame;

BeaconScheduler::BeaconScheduler(TransmissionTiming* _TransmissionTiming):
transmissionTiming(_TransmissionTiming) 
{
MESSAGE_SINGLE(NORMAL, logger, "BeaconScheduler: is built! ");	
}


void BeaconScheduler::SetStartOffsetTime(int OffsetSlot)
{
 	SlotDuration = 85E-6	;
	OffsetTime = OffsetSlot * SlotDuration;
	setTimeout(OffsetTime);
}

void BeaconScheduler::onTimeout()
{
	MESSAGE_SINGLE(NORMAL, logger, "BeaconScheduler: Time to transmit Beacon! ");	
	transmissionTiming->TimeToTransmit();
}	




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
