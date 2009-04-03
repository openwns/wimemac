import unittest

import wns.Node
import copper.Copper
import glue.support.Configuration

class ConfigurationTest(unittest.TestCase):

    def setUp(self):
        self.node = wns.Node.Node("testNode")
        self.wire = copper.Copper.Wire("testWire")
        self.phy = copper.Copper.Transceiver(self.node, "testPhy", self.wire, ber = 0.0, dataRate = 1e+3)


    def test_ShortCutComponent(self):
        glue.support.Configuration.ShortCutComponent(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_AcknowledgedModeShortCutComponent(self):
        glue.support.Configuration.AcknowledgedModeShortCutComponent(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SelectiveRepeatARQ(self):
        glue.support.Configuration.SelectiveRepeatARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SARSelectiveRepeatARQ(self):
        glue.support.Configuration.SARSelectiveRepeatARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_GoBackNARQ(self):
        glue.support.Configuration.GoBackNARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SARGoBackNARQ(self):
        glue.support.Configuration.SARGoBackNARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_StopAndWaitARQ(self):
        glue.support.Configuration.StopAndWaitARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SARStopAndWaitARQ(self):
        glue.support.Configuration.SARStopAndWaitARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_ShortCut(self):
        glue.support.Configuration.ShortCut(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)
