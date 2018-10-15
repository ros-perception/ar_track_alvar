#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Software License Agreement (BSD License)
#
# Copyright (c) 2017, TORK (Tokyo Opensource Robotics Kyokai Association) 
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Tokyo Opensource Robotics Kyokai Association
#    nor the names of its contributors may be used to endorse or promote 
#    products derived from this software without specific prior written 
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import numpy
import math
import unittest

from geometry_msgs.msg import Pose
import rospy
import tf
from tf.transformations import quaternion_from_euler

class TestArAlvarRos(unittest.TestCase):
    '''
    Tests are based on http://download.ros.org/data/ar_track_alvar/ar_track_alvar_4markers_tork_2017-01-30-15-03-19.bag
    '''

    def setUp(self):
        rospy.init_node('test_armarker_ros_detect')
        self.tflistener = tf.TransformListener()

    def tearDown(self):
        True  # TODO impl something meaningful

    def _lookup_tf(self, origin, target):
        '''
        DEPRECATED: This does not return meaningful values for some reason.
        @param origin: lookupTransform's 1st argument.
        @param target: lookupTransform's 2nd argument.
        @rtype: ([str], [str])
        @return: Translation and rotation.
        '''
        while not rospy.is_shutdown():
            try:
                (trans,rot) = self.tflistener.lookupTransform(origin, target, rospy.Time(0))
                break
            except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException) as e:
                rospy.logerr(str(e) + ' target={}'.format(target))
                continue
        return (trans, rot)
        
    def test_markers(self):
        '''
        Aiming the real output taken from a bag file.
        '''
        # The values in this list are ordered in the marker's number. 
        tf_expected = [[[0.04464459977845401, 0.05341412598745035, 0.26796900627543024], [0.6726999185589797, 0.7391474391806558, -0.01739267319701629, -0.028868280906256056]],
                       [[-0.04805772245624329, 0.039528315926071665, 0.26775882622136327], [0.48207151562664247, 0.8758763282975102, -0.016363763970395625, -0.013414118615296202]],
                       [[0.007233278235745441, 0.015615692018491452, 0.26619586686955365], [0.08546919545682985, 0.9959809257461487, 0.00424040439, -0.02677659572186436]],
                       [[0.06223014382428272, 0.014613815037010106, 0.26226145707174475], [-0.46400320825216246, 0.8850390875261293, 0.032644264656690035, -0.018471282241381157]]]
        for i in range (0, len(tf_expected)):
            while not rospy.is_shutdown():
                try:
                    target_frame = '/ar_marker_{}'.format(i)
                    (trans, rot) = self.tflistener.lookupTransform('/camera', target_frame, rospy.Time(0))
                    break
                except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException) as e:
                    rospy.logerr(str(e) + ' target_frame={}'.format(target_frame))
                    continue
            # Compare each translation element (x, y, z) 
            for v_ret, v_expected in zip(trans, tf_expected[i][0]):
                # Given that sigfig ignores the leading zero, we only compare the first sigfig.
                numpy.testing.assert_approx_equal(
                    v_ret, v_expected, significant=1)
            # Compare each orientation element (x, y, z, w) 
            for v_ret, v_expected in zip(rot, tf_expected[i][1]):
                numpy.testing.assert_approx_equal(
                    v_ret, v_expected, significant=1)

if __name__ == '__main__':
    import rostest
    rostest.rosrun('ar_track_alvar', 'test_ar_alvar_ros', TestArAlvarRos)
